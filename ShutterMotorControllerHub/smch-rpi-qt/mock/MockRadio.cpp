#include "MockRadio.h"

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(MockRadioLog)
Q_LOGGING_CATEGORY(MockRadioLog, "MockRadio")

MockRadio::MockRadio()
{
    qCInfo(MockRadioLog) << "created";
}

std::future<radio::Result> MockRadio::send(radio::Command, const std::string&)
{
    return{};
}

//void MockRadio::send(radio::Command command,
//                     const std::string& address,
//                     radio::SendCallback&& callback)
//{
//    qCInfo(MockRadioLog) << "sending command" << command << "to" << address.c_str();
//    callback(command, qrand() % 10 > 7 ? radio::Result::Succeeded : radio::Result::PacketLost);
//}
