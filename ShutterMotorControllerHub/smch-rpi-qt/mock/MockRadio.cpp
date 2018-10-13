#include "MockRadio.h"

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(MockRadioLog)
Q_LOGGING_CATEGORY(MockRadioLog, "MockRadio")

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

MockRadio::MockRadio()
{
    qCInfo(MockRadioLog) << "created";
}

std::future<radio::Result> MockRadio::send(radio::Command command, const std::string &address)
{

}

//void MockRadio::send(radio::Command command,
//                     const std::string& address,
//                     radio::SendCallback&& callback)
//{
//    qCInfo(MockRadioLog) << "sending command" << command << "to" << address.c_str();
//    callback(command, qrand() % 10 > 7 ? radio::Result::Succeeded : radio::Result::PacketLost);
//}
