#pragma once

#include "Command.h"
#include "Result.h"

#include <future>
#include <string>

namespace radio
{

class IRadio
{
public:
    virtual ~IRadio() = default;

    virtual std::future<Result> send(Command command, const std::string& address) = 0;
};

}
