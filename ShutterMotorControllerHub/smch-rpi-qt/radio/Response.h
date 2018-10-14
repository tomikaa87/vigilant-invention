#pragma once

#include "Result.h"
#include "radio_protocol.h"
#include <memory>

namespace radio
{

struct Response
{
    Result result;
    std::unique_ptr<protocol_msg_t> message;
};

}
