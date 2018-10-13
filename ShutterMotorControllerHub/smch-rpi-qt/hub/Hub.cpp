#include "Hub.h"

#include <algorithm>
#include <iterator>
#include <thread>

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(HubLog)
Q_LOGGING_CATEGORY(HubLog, "Hub")


namespace hub
{

Hub::Hub(const std::shared_ptr<radio::IRadio>& radio, QObject* parent)
    : QObject{ parent }
    , m_radio{ radio }
    , m_thread{ std::make_unique<std::thread>([this] { threadProc(); }) }
{
    // TODO remove test data
    m_devices.emplace(DeviceIndex::D0_0, Device{ "SMRR0" });
    m_devices.emplace(DeviceIndex::D0_1, Device{ "SMRR0" });
}

Hub::~Hub()
{
    m_shutdownThread = true;
    m_thread->join();
}

std::shared_future<void> hub::Hub::scanDevices()
{
    QueueElement e;

    e.second = [] {
        qCDebug(HubLog) << "scan executed";
    };

    return enqueueElement(std::move(e));
}

std::shared_future<void> Hub::execute(Command command, DeviceIndex device)
{
    return createRadioTasks(command, std::vector<DeviceIndex>{ device });
}

std::shared_future<void> Hub::execute(Command command, std::vector<DeviceIndex>&& devices)
{
    return createRadioTasks(command, std::move(devices));
}

std::shared_future<void> Hub::executeOnAll(Command command)
{

}

void Hub::threadProc()
{
    while (!m_shutdownThread)
    {
        QueueElement e;

        do {
            std::unique_lock<std::mutex> l{ m_queueMutex };
            m_queueCV.wait(l, [this] { return !m_queue.empty() && !m_shutdownThread; });
            e = std::move(m_queue.front());
            m_queue.pop();
        } while (false);

        e.second();

        e.first.set_value();
    }
}

std::shared_future<void> Hub::enqueueElement(Hub::QueueElement&& e)
{
    auto f = e.first.get_future();

    do {
        std::lock_guard<std::mutex> l{ m_queueMutex };
        m_queue.push(std::move(e));
    } while (false);

    m_queueCV.notify_one();

    return f;
}

// Debug helpers

//QDebug& operator<<(QDebug& dbg, IRemoteControl::Command command);
//QDebug& operator<<(QDebug& dbg, IRemoteControl::DeviceIndex index);

//template <typename Alloc>
//QDebug operator<<(QDebug dbg, const std::vector<IRemoteControl::DeviceIndex, Alloc>& indices)
//{
//    dbg.nospace();

//    bool first = true;
//    for (auto i : indices)
//    {
//        if (!first)
//            dbg << ", ";
//        dbg << i;
//        first = false;
//    }

//    dbg.maybeSpace();

//    return dbg;
//}

//QDebug& operator<<(QDebug& dbg, radio::Command command);

//QDebug& operator<<(QDebug& dbg, const hub::Task& task)
//{
//    dbg.nospace()
//            << "Task{ command: " << task.command
//            << ", address: " << task.address.c_str()
//            << ", retries: " << task.retryCount
//            << " }";
//    return dbg.space();
//}

//QDebug& operator<<(QDebug& dbg, radio::Result result)
//{
//    switch (result)
//    {
//        case radio::Result::Busy:
//            dbg << "Busy";
//            break;

//        case radio::Result::PacketLost:
//            dbg << "PacketLost";
//            break;

//        case radio::Result::Succeeded:
//            dbg << "Succeeded";
//            break;
//    }

//    return dbg;
//}

//// Debug helpers

//namespace hub
//{

//void Hub::execute(IRemoteControl::Command command,
//                   IRemoteControl::DeviceIndex device)
//{
//    qCInfo(HubLog) << "executing" << command << "on" << device;

//    createRadioTasks(command, std::vector<DeviceIndex>{ device });
//}

//void Hub::execute(IRemoteControl::Command command,
//                   std::vector<IRemoteControl::DeviceIndex>&& devices)
//{
//    qCInfo(HubLog) << "executing" << command << "on" << devices;

//    createRadioTasks(command, std::move(devices));
//}

//void Hub::executeOnAll(IRemoteControl::Command command)
//{
//    qCInfo(HubLog) << "executing" << command << "on all devices";

//    std::vector<DeviceIndex> indices;
//    indices.reserve(m_devices.size());

//    std::transform(std::begin(m_devices),
//                   std::end(m_devices),
//                   std::back_inserter(indices),
//                   [](const std::pair<const IRemoteControl::DeviceIndex, Hub::Device>& p) { return p.first; });

//    createRadioTasks(command, std::move(indices));
//}

radio::Command mapToRadioCommand(DeviceIndex index,
                                 Command command)
{
    int indexValue = static_cast<int>(index);
    bool primaryControl = indexValue % 2 == 0;

    switch (command)
    {
        case Command::ShutterDown:
            return primaryControl ? radio::Command::Shutter1Down : radio::Command::Shutter2Down;

        case Command::ShutterUp:
            return primaryControl ? radio::Command::Shutter1Up : radio::Command::Shutter2Up;
    }

    // TODO return
}

std::shared_future<void> Hub::createRadioTasks(Command command, const std::vector<DeviceIndex>& devices)
{
    if (m_devices.empty())
    {
        qCWarning(HubLog) << "no active devices";
        return{};
    }

//    qCDebug(HubLog).nospace()
//            << "Creating tasks for { command: " << command
//            << ", devices: { " << devices << " }";

    std::map<std::string, radio::Command> radioCommands;

    // Map device indices and commands to addresses and low level radio commands.
    // Also group commands addressed to the same device.
    for (const auto& d : devices)
    {
        int indexValue = static_cast<int>(d);
        int radioDeviceIndex = indexValue / 2;

        std::string deviceAddress{ "SMRR0" };
        deviceAddress[4] = static_cast<char>('0' + radioDeviceIndex);

        auto& groupedCommand = radioCommands[deviceAddress];
        auto newCommand = mapToRadioCommand(d, command);

        if (groupedCommand == radio::Command::AllDown || groupedCommand == radio::Command::AllUp)
        {
            continue;
        }
        else if ((groupedCommand == radio::Command::Shutter1Down && newCommand == radio::Command::Shutter2Down) ||
                 (groupedCommand == radio::Command::Shutter2Down && newCommand == radio::Command::Shutter1Down))
        {
            groupedCommand = radio::Command::AllDown;
        }
        else if ((groupedCommand == radio::Command::Shutter1Up && newCommand == radio::Command::Shutter2Up) ||
                 (groupedCommand == radio::Command::Shutter2Up && newCommand == radio::Command::Shutter1Up))
        {
            groupedCommand = radio::Command::AllUp;
        }
        else
        {
            groupedCommand = newCommand;
        }
    }

    std::vector<std::shared_future<void>> futures;
    futures.reserve(radioCommands.size());

    for (auto&& command : radioCommands)
    {
        QueueElement e;

        e.second = [command { std::move(command) }] {
            qCDebug(HubLog) << "executing radio command on:" << command.first.c_str();
        };

        auto&& f = enqueueElement(std::move(e));
        futures.push_back(std::move(f));
    }

    QueueElement e;

    // Shared future is necessary because the lambda is being copied into the std::function
    e.second = [futures{ std::move(futures) }] {
        for (auto& f : futures)
            f.wait();
    };

    return enqueueElement(std::move(e));
}

//void Hub::executeNextRadioTask()
//{
//    if (m_radioTaskQueue.empty())
//    {
//        qCInfo(HubLog) << "no more radio tasks in the queue";
//        return;
//    }

//    if (m_radioTaskExecuting)
//    {
//        qCWarning(HubLog) << "a radio task is already being executed";
//        return;
//    }

//    m_radioTaskExecuting = true;

//    auto task = std::move(m_radioTaskQueue.front());
//    m_radioTaskQueue.pop();

//    qCDebug(HubLog) << "executing radio task:" << task;
//    qCDebug(HubLog) << "remaining tasks:" << m_radioTaskQueue.size();

//    m_executingTask = std::move(task);

//    m_radio->send(m_executingTask.command, m_executingTask.address, [this](radio::Command command, radio::Result result) {
//        handleRadioSendCallback(command, result);
//    });
//}

//void Hub::handleRadioSendCallback(radio::Command command, radio::Result result)
//{
//    if (command != m_executingTask.command)
//    {
//        qCWarning(HubLog) << "radio send result mismatch";
//        return;
//    }

//    if (result != radio::Result::Succeeded)
//    {
//        qCWarning(HubLog) << "radio task failed with result:" << result;

//        if (m_executingTask.retryCount < m_executingTask.MaxRetryCount)
//        {
//            qCWarning(HubLog) << "retrying" << m_executingTask;
//            ++m_executingTask.retryCount;
//            m_radioTaskQueue.push(std::move(m_executingTask));
//        }
//        else
//        {
//            qCWarning(HubLog) << "radio task failed, giving up:" << m_executingTask;
//        }
//    }
//    else
//    {
//        qCInfo(HubLog) << "radio task successfully executed:" << m_executingTask;
//    }

//    m_radioTaskExecuting = false;

//    executeNextRadioTask();
//}

}
