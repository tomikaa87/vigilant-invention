#include "MockHub.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(MockHubLog)
Q_LOGGING_CATEGORY(MockHubLog, "MockHub")

QDebug& operator<<(QDebug& dbg, IRemoteControl::Command command)
{
    switch (command)
    {
        case IRemoteControl::Command::ShutterDown:
            dbg << "ShutterDown";
            break;

        case IRemoteControl::Command::ShutterUp:
            dbg << "ShutterUp";
            break;
    }

    return dbg;
}

QDebug& operator<<(QDebug& dbg, IRemoteControl::DeviceIndex index)
{
    dbg << static_cast<int>(index);
    return dbg;
}

QDebug& operator<<(QDebug& dbg, const std::vector<IRemoteControl::DeviceIndex>& indices)
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

    dbg.space();

    return dbg;
}

MockHub::MockHub(const std::shared_ptr<radio::IRadio>& radio)
    : m_radio(radio)
{
    qCInfo(MockHubLog) << "created";
}

void MockHub::execute(IRemoteControl::Command command, IRemoteControl::DeviceIndex device)
{
    qCInfo(MockHubLog) << "executing" << command << "on" << device;
}

void MockHub::execute(IRemoteControl::Command command, std::vector<IRemoteControl::DeviceIndex>&& devices)
{
    qCInfo(MockHubLog) << "executing" << command << "on" << devices;
}

void MockHub::executeOnAll(IRemoteControl::Command command)
{
    qCInfo(MockHubLog) << "executing" << command << "on all devices";
}
