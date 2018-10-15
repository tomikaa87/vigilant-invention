#pragma once

#include <functional>
#include <map>
#include <memory>

#include <QObject>

#include "Device.h"
#include "DeviceIndex.h"
#include "IHub.h"
#include "IRemoteControl.h"
#include "OperationQueue.h"
#include "Task.h"

#include "radio/IRadio.h"

namespace hub
{

class Hub : public QObject, public IHub, public IRemoteControl
{
public:
    Hub(const std::shared_ptr<radio::IRadio>& radio, QObject* parent = nullptr);
    ~Hub() override;

    // IHub interface
public:
    std::future<void> scanDevices() override;

    // IRemoteControl interface
public:
    void execute(hub::Command command, DeviceIndex device) override;
    void execute(hub::Command command, std::vector<DeviceIndex>&& devices) override;
    void executeOnAll(hub::Command command) override;

private:
    OperationQueue m_queue;
    std::shared_ptr<radio::IRadio> m_radio;
    std::map<DeviceIndex, Device> m_devices;
    int m_transmitRetryCount = 3;

    void executeCommandAsync(Command command, const std::vector<DeviceIndex>& devices);
};

}
