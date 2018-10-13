#ifndef MOCKHUB_H
#define MOCKHUB_H

#include <memory>

#include "hub/IHub.h"
#include "IRemoteControl.h"
#include "radio/IRadio.h"

class MockHub : public IRemoteControl
{
public:
    MockHub(const std::shared_ptr<radio::IRadio>& radio);

private:
    const std::shared_ptr<radio::IRadio> m_radio;
    std::unordered_map<uint8_t, IHub::RemoteDevice> m_devices;

    // IRemoteControl interface
public:
    void execute(Command command, DeviceIndex device) override;
    void execute(Command command, std::vector<DeviceIndex>&& devices) override;
    void executeOnAll(Command command) override;
};

#endif // MOCKHUB_H
