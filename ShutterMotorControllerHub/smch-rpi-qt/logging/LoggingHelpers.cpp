#include "LoggingHelpers.h"

QDebug& operator<<(QDebug& dbg, const hub::Task& task)
{
    dbg.nospace()
            << "Task{ command: " << task.command
            << ", address: " << task.address.c_str()
            << ", retries: " << task.retryCount
            << " }";
    return dbg.space();
}

QDebug& operator<<(QDebug& dbg, const hub::Command& command)
{
    switch (command)
    {
    case hub::Command::ShutterDown:
        dbg << "ShutterDown";
        break;

    case hub::Command::ShutterUp:
        dbg << "ShutterUp";
        break;
    }

    return dbg;
}

QDebug& operator<<(QDebug& dbg, const hub::DeviceIndex& index)
{
    int i = static_cast<int>(index);
    dbg.nospace() << "DeviceIndex{dev=" << i / 2 << ", sub=" << i % 2 << "}";
    return dbg.maybeSpace();
}

QDebug& operator<<(QDebug& dbg, radio::Command command)
{
    switch (command)
    {
    case radio::Command::AllDown:
        dbg << "AllDown";
        break;

    case radio::Command::AllUp:
        dbg << "AllUp";
        break;

    case radio::Command::Shutter1Down:
        dbg << "Shutter1Down";
        break;

    case radio::Command::Shutter1Up:
        dbg << "Shutter1Up";
        break;

    case radio::Command::Shutter2Down:
        dbg << "Shutter2Down";
        break;

    case radio::Command::Shutter2Up:
        dbg << "Shutter2Up";
        break;
    }

    return dbg;
}

QDebug& operator<<(QDebug& dbg, radio::Result result)
{
    switch (result)
    {
    case radio::Result::PacketLost:
        dbg << "PacketLost";
        break;

    case radio::Result::Succeeded:
        dbg << "Succeeded";
        break;
    }

    return dbg;
}
