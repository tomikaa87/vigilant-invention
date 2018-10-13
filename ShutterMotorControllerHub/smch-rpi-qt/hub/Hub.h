#pragma once

#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <queue>

#include <QObject>

#include "Device.h"
#include "DeviceIndex.h"
#include "IHub.h"
#include "IRemoteControl.h"
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
    std::shared_future<void> scanDevices() override;

    // IRemoteControl interface
public:
    std::shared_future<void> execute(hub::Command command, DeviceIndex device) override;
    std::shared_future<void> execute(hub::Command command, std::vector<DeviceIndex>&& devices) override;
    std::shared_future<void> executeOnAll(hub::Command command) override;

private:
    using QueueElement = std::pair<std::promise<void>, std::function<void()>>;

    std::shared_ptr<radio::IRadio> m_radio;
    std::unique_ptr<std::thread> m_workerThread;
    std::mutex m_workQueueMutex;
    std::condition_variable m_workQueueCV;
    std::atomic_bool m_shutdownWorkerThread = { false };
    std::queue<QueueElement> m_workQueue;
    std::map<DeviceIndex, Device> m_devices;

    void threadProc();
    std::shared_future<void> enqueueElement(QueueElement&& e);
    std::shared_future<void> createRadioTasks(Command command, const std::vector<DeviceIndex>& devices);
};

}
