#include "MockRadio.h"
#include "logging/LoggingHelpers.h"
#include "radio/radio_protocol.h"
#include "radio/Utils.h"

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(MockRadioLog)
Q_LOGGING_CATEGORY(MockRadioLog, "MockRadio")

MockRadio::MockRadio()
{
    qCInfo(MockRadioLog) << "created";
}

std::future<radio::Response> MockRadio::sendCommand(radio::Command command, const std::string& address)
{
    qCDebug(MockRadioLog) << "sending command" << command << "to" << address.c_str();

    auto&& task = std::make_shared<radio::Task>(radio::createCommandRequest(command, address));

    m_queue.enqueue([task] {
        qCDebug(MockRadioLog)
                << "transmitting command" << task->request.msg.payload.command
                << "to" << task->request.address.c_str();
        std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 200));
        task->promise.set_value(createCommandResponse(rand() % 10 > 4));
    });

    return task->promise.get_future();
}

std::future<radio::Response> MockRadio::readStatus(const std::string& address)
{
    qCDebug(MockRadioLog) << "reading status of" << address.c_str();

    auto&& task = std::make_shared<radio::Task>(radio::createStatusRequest(address));

    m_queue.enqueue([task] {
        qCDebug(MockRadioLog) << "transmitting status request of" << task->request.address.c_str();
        std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 500));
        task->promise.set_value(createStatusResponse(rand() % 10 > 6));
    });

    return task->promise.get_future();
}

radio::Response MockRadio::createCommandResponse(bool simulatePacketLoss)
{
    radio::Response r;

    r.result = simulatePacketLoss ? radio::Result::PacketLost : radio::Result::Succeeded;

    if (!simulatePacketLoss)
    {
        protocol_msg_t msg;
        protocol_msg_init(&msg);

        msg.msg_type = PROTO_MSG_RESULT;
        msg.payload.result.result_code = PROTO_RESULT_OK;

        r.message = std::make_unique<protocol_msg_t>(msg);
    }

    return r;
}

radio::Response MockRadio::createStatusResponse(bool simulatePacketLoss)
{
    radio::Response r;

    r.result = simulatePacketLoss ? radio::Result::PacketLost : radio::Result::Succeeded;

    if (!simulatePacketLoss)
    {
        protocol_msg_t msg;
        protocol_msg_init(&msg);

        msg.msg_type = PROTO_MSG_READ_STATUS_RESULT;
        strcpy(msg.payload.status.firmware_ver, "9.9.9");
        msg.payload.status.last_commands[0] = PROTO_CMD_SHUTTER_1_DOWN;

        r.message = std::make_unique<protocol_msg_t>(msg);
    }

    return r;
}
