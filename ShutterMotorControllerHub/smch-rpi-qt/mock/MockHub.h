#pragma once

#include <memory>

#include "hub/IHub.h"
#include "hub/IRemoteControl.h"
#include "radio/IRadio.h"

class MockHub : public hub::IHub
{
public:
    MockHub(const std::shared_ptr<radio::IRadio>& radio);

    // IHub interface
public:
    void scanDevices() override;

private:
    const std::shared_ptr<radio::IRadio> m_radio;
//    std::map<uint8_t, IHub::RemoteDevice> m_devices;
};
