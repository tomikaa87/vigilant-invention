#pragma once

#include "radio/IRadio.h"

class MockRadio : public radio::IRadio
{
public:
    MockRadio();

    // IRadio interface
public:
    std::future<radio::Result> send(radio::Command command, const std::string &address) override;
};
