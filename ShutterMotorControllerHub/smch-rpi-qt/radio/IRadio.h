#pragma once

#include "Command.h"
#include "Response.h"

#include <future>
#include <string>

namespace radio
{

class IRadio
{
public:
    virtual ~IRadio() = default;

    virtual std::future<Response> sendCommand(Command command, const std::string& address) = 0;
    virtual std::future<Response> readStatus(const std::string& address) = 0;
};

}
