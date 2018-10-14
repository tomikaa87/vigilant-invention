#pragma once

#include "radio/IRadio.h"
#include "radio/Task.h"
#include "OperationQueue.h"

#include <memory>

class MockRadio : public radio::IRadio
{
public:
    MockRadio();
    ~MockRadio();

    // IRadio interface
public:
    std::future<radio::Response> sendCommand(radio::Command command, const std::string& address) override;
    std::future<radio::Response> readStatus(const std::string& address) override;

private:
    OperationQueue m_queue;
    std::atomic_bool m_busy = { false };

    static radio::Response createCommandResponse(bool simulatePacketLoss = false);
    static radio::Response createStatusResponse(bool simulatePacketLoss = false);

};
