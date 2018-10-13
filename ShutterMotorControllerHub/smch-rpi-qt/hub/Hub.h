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
    std::shared_ptr<radio::IRadio> m_radio;
    std::unique_ptr<std::thread> m_thread;

    using QueueElement = std::pair<std::promise<void>, std::function<void()>>;
    std::queue<QueueElement> m_queue;

    std::mutex m_queueMutex;
    std::condition_variable m_queueCV;
    std::mutex m_threadProcMutex;
    std::atomic_bool m_shutdownThread = { false };
    std::map<DeviceIndex, Device> m_devices;

    void threadProc();
    std::shared_future<void> enqueueElement(QueueElement&& e);

    std::shared_future<void> createRadioTasks(Command command, const std::vector<DeviceIndex>& devices);
};

//class Hub : public QObject, public IRemoteControl, public std::enable_shared_from_this<Hub>
//{
//    Q_OBJECT

//public:
//    Hub(const std::shared_ptr<radio::IRadio>& radio,
//         QObject *parent = nullptr);

//    // IRemoteControl interface
//public:
//    void execute(Command command, DeviceIndex device) override;
//    void execute(Command command, std::vector<DeviceIndex>&& devices) override;
//    void executeOnAll(Command command) override;

//private:
//    const std::shared_ptr<radio::IRadio> m_radio;


//private:
//    std::queue<hub::Task> m_radioTaskQueue;
//    std::map<DeviceIndex, Device> m_devices;
//    hub::Task m_executingTask;
//    bool m_radioTaskExecuting = false;

//    void createRadioTasks(Command command, const std::vector<DeviceIndex>& devices);
//    void executeNextRadioTask();
//    void handleRadioSendCallback(radio::IRadio::Command command, radio::IRadio::Result result);
//};

}
