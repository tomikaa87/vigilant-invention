#pragma once

#include "Request.h"
#include "Response.h"
#include <future>
#include <string>

namespace radio
{

struct Task
{
    Task(Request&& request)
        : request{ std::move(request) }
    {}

    Request request;
    std::promise<Response> promise;
};

}
