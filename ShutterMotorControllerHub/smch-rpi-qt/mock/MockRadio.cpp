#include "MockRadio.h"

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(MockRadioLog)
Q_LOGGING_CATEGORY(MockRadioLog, "MockRadio")

QDebug& operator<<(QDebug& dbg, IRadio::Command command)
{
    switch (command)
    {
        case IRadio::Command::AllDown:
            dbg << "AllDown";
            break;

        case IRadio::Command::AllUp:
            dbg << "AllUp";
            break;

        case IRadio::Command::Shutter1Down:
            dbg << "Shutter1Down";
            break;

        case IRadio::Command::Shutter1Up:
            dbg << "Shutter1Up";
            break;

        case IRadio::Command::Shutter2Down:
            dbg << "Shutter2Down";
            break;

        case IRadio::Command::Shutter2Up:
            dbg << "Shutter2Up";
            break;
    }

    return dbg;
}

MockRadio::MockRadio()
{
    qCInfo(MockRadioLog) << "created";
}

void MockRadio::send(IRadio::Command command,
                     const std::string& address,
                     IRadio::SendCallback&& callback)
{
    qCInfo(MockRadioLog) << "sending command" << command << "to" << address.c_str();
    callback(command, qrand() % 10 > 7 ? IRadio::Result::Succeeded : IRadio::Result::PacketLost);
}
