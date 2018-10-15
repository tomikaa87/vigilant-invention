#pragma once

#include <string>
#include "radio_protocol.h"

namespace radio
{

struct Request
{
    Request(protocol_msg_t&& msg, const std::string& address)
        : msg(std::move(msg))
        , address(address)
    {}

    protocol_msg_t msg;
    std::string address;
};

}
