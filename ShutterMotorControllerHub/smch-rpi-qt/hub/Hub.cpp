#include "Hub.h"

#include <algorithm>
#include <iterator>

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(HubLog)
Q_LOGGING_CATEGORY(HubLog, "Hub")


// Debug helpers

QDebug& operator<<(QDebug& dbg, IRemoteControl::Command command);
QDebug& operator<<(QDebug& dbg, IRemoteControl::DeviceIndex index);

template <typename Alloc>
QDebug operator<<(QDebug dbg, const std::vector<IRemoteControl::DeviceIndex, Alloc>& indices)
{
    dbg.nospace();

    bool first = true;
    for (auto i : indices)
    {
        if (!first)
            dbg << ", ";
        dbg << i;
        first = false;
    }

    dbg.maybeSpace();

    return dbg;
}

QDebug& operator<<(QDebug& dbg, IRadio::Command command);

QDebug& operator<<(QDebug& dbg, const hub::Task& task)
{
    dbg.nospace()
            << "Task{ command: " << task.command
            << ", address: " << task.address.c_str()
            << ", retries: " << task.retryCount
            << " }";
    return dbg.space();
}

QDebug& operator<<(QDebug& dbg, IRadio::Result result)
{
    switch (result)
    {
        case IRadio::Result::Busy:
            dbg << "Busy";
            break;

        case IRadio::Result::PacketLost:
            dbg << "PacketLost";
            break;

        case IRadio::Result::Succeeded:
            dbg << "Succeeded";
            break;
    }

    return dbg;
}

// Debug helpers

namespace hub
{

Hub::Hub(const std::shared_ptr<IRadio>& radio, QObject* parent)
    : QObject{ parent }
    , m_radio{ radio }
{
    // TODO remove test data
    m_devices.emplace(DeviceIndex::D0_0, Device{ "SMRR0" });
    m_devices.emplace(DeviceIndex::D0_1, Device{ "SMRR0" });
}

void Hub::execute(IRemoteControl::Command command,
                   IRemoteControl::DeviceIndex device)
{
    qCInfo(HubLog) << "executing" << command << "on" << device;

    createRadioTasks(command, std::vector<DeviceIndex>{ device });
}

void Hub::execute(IRemoteControl::Command command,
                   std::vector<IRemoteControl::DeviceIndex>&& devices)
{
    qCInfo(HubLog) << "executing" << command << "on" << devices;

    createRadioTasks(command, std::move(devices));
}

void Hub::executeOnAll(IRemoteControl::Command command)
{
    qCInfo(HubLog) << "executing" << command << "on all devices";

    std::vector<DeviceIndex> indices;
    indices.reserve(m_devices.size());

    std::transform(std::begin(m_devices),
                   std::end(m_devices),
                   std::back_inserter(indices),
                   [](const std::pair<const IRemoteControl::DeviceIndex, Hub::Device>& p) { return p.first; });

    createRadioTasks(command, std::move(indices));
}

IRadio::Command mapToRadioCommand(IRemoteControl::DeviceIndex index,
                                  IRemoteControl::Command command)
{
    int indexValue = static_cast<int>(index);
    bool primaryControl = indexValue % 2 == 0;

    switch (command)
    {
        case IRemoteControl::Command::ShutterDown:
            return primaryControl ? IRadio::Command::Shutter1Down : IRadio::Command::Shutter2Down;

        case IRemoteControl::Command::ShutterUp:
            return primaryControl ? IRadio::Command::Shutter1Up : IRadio::Command::Shutter2Up;
    }

    // TODO return
}

void Hub::createRadioTasks(Command command, const std::vector<DeviceIndex>& devices)
{
    if (m_devices.empty())
    {
        qCWarning(HubLog) << "no active devices";
        return;
    }

    qCDebug(HubLog).nospace()
            << "Creating tasks for { command: " << command
            << ", devices: { " << devices << " }";

    std::map<std::string, IRadio::Command> radioCommands;

    // Map device indices and commands to addresses and low level radio commands
    for (const auto& d : devices)
    {
        int dValue = static_cast<int>(d);
        int radioDeviceIndex = dValue / 2;

        std::string deviceAddress{ "SMRR0" };
        deviceAddress[4] = static_cast<char>('0' + radioDeviceIndex);

        auto& taskRadioCommand = radioCommands[deviceAddress];
        auto radioCommand = mapToRadioCommand(d, command);

        if (taskRadioCommand == IRadio::Command::AllDown || taskRadioCommand == IRadio::Command::AllUp)
        {
            continue;
        }
        else if ((taskRadioCommand == IRadio::Command::Shutter1Down && radioCommand == IRadio::Command::Shutter2Down) ||
                 (taskRadioCommand == IRadio::Command::Shutter2Down && radioCommand == IRadio::Command::Shutter1Down))
        {
            taskRadioCommand = IRadio::Command::AllDown;
        }
        else if ((taskRadioCommand == IRadio::Command::Shutter1Up && radioCommand == IRadio::Command::Shutter2Up) ||
                 (taskRadioCommand == IRadio::Command::Shutter2Up && radioCommand == IRadio::Command::Shutter1Up))
        {
            taskRadioCommand = IRadio::Command::AllUp;
        }
        else
        {
            taskRadioCommand = radioCommand;
        }
    }

    for (auto&& c : radioCommands)
    {
        qCDebug(HubLog) << "command" << c.second << "will be sent to" << c.first.c_str();
        m_radioTaskQueue.emplace(c.second, c.first);
    }

    if (!m_radioTaskExecuting)
        executeNextRadioTask();
}

void Hub::executeNextRadioTask()
{
    if (m_radioTaskQueue.empty())
    {
        qCInfo(HubLog) << "no more radio tasks in the queue";
        return;
    }

    if (m_radioTaskExecuting)
    {
        qCWarning(HubLog) << "a radio task is already being executed";
        return;
    }

    m_radioTaskExecuting = true;

    auto task = std::move(m_radioTaskQueue.front());
    m_radioTaskQueue.pop();

    qCDebug(HubLog) << "executing radio task:" << task;
    qCDebug(HubLog) << "remaining tasks:" << m_radioTaskQueue.size();

    m_executingTask = std::move(task);

    m_radio->send(m_executingTask.command, m_executingTask.address, [this](IRadio::Command command, IRadio::Result result) {
        handleRadioSendCallback(command, result);
    });
}

void Hub::handleRadioSendCallback(IRadio::Command command, IRadio::Result result)
{
    if (command != m_executingTask.command)
    {
        qCWarning(HubLog) << "radio send result mismatch";
        return;
    }

    if (result != IRadio::Result::Succeeded)
    {
        qCWarning(HubLog) << "radio task failed with result:" << result;

        if (m_executingTask.retryCount < m_executingTask.MaxRetryCount)
        {
            qCWarning(HubLog) << "retrying" << m_executingTask;
            ++m_executingTask.retryCount;
            m_radioTaskQueue.push(std::move(m_executingTask));
        }
        else
        {
            qCWarning(HubLog) << "radio task failed, giving up:" << m_executingTask;
        }
    }
    else
    {
        qCInfo(HubLog) << "radio task successfully executed:" << m_executingTask;
    }

    m_radioTaskExecuting = false;

    executeNextRadioTask();
}

}
