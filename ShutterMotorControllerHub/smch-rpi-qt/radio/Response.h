#pragma once

#include "Result.h"
#include "radio_protocol.h"
#include <list>

namespace radio
{

struct Response
{
    Result result;
    std::list<protocol_msg_t> messages;
};

}
