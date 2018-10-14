#include "Utils.h"

#include <cassert>

namespace radio
{

Request createStatusRequest(const std::string& address)
{
    protocol_msg_t msg;
    protocol_msg_init(&msg);

    msg.msg_type = PROTO_MSG_READ_STATUS;

    return Request{ std::move(msg), address };
}

Request createCommandRequest(Command command, const std::string& address)
{
    protocol_msg_t msg;
    protocol_msg_init(&msg);

    msg.msg_type = PROTO_MSG_COMMAND;
    msg.payload.command = commandToProtocolCmd(command);

    return Request{ std::move(msg), address };
}

protocol_cmd_t commandToProtocolCmd(Command command)
{
    switch (command)
    {
    case Command::AllDown:
        return PROTO_CMD_ALL_DOWN;

    case Command::AllUp:
        return PROTO_CMD_ALL_UP;

    case Command::Shutter1Down:
        return PROTO_CMD_SHUTTER_1_DOWN;

    case Command::Shutter1Up:
        return PROTO_CMD_SHUTTER_1_UP;

    case Command::Shutter2Down:
        return PROTO_CMD_SHUTTER_2_DOWN;

    case Command::Shutter2Up:
        return PROTO_CMD_SHUTTER_2_UP;
    }

    assert(false);

    return PROTO_CMD_NOP;
}

}
