#ifndef HUB2_H
#define HUB2_H

#include <memory>
#include <queue>
#include <unordered_map>
#include <QObject>

#include "IRemoteControl.h"
#include "IRadio.h"

class Hub2 : public QObject, public IRemoteControl, std::enable_shared_from_this<Hub2>
{
    Q_OBJECT

public:
    Hub2(const std::shared_ptr<IRadio>& radio,
         QObject *parent = nullptr);

    // IRemoteControl interface
public:
    void execute(Command command, DeviceIndex device) override;
    void execute(Command command, std::vector<DeviceIndex>&& devices) override;
    void executeOnAll(Command command) override;

private:
    const std::shared_ptr<IRadio> m_radio;

public:
    struct Task
    {
        IRadio::Command command;
        std::string address;

        int maxRetryCount = 10; // cannot be const otherwise Task cannot be moved
        int retryCount = 0;

        Task(IRadio::Command command, std::string address)
            : command(command)
            , address(std::move(address))
        {}

        Task() = default;

        Task(const Task& o) = delete;
        Task(Task&& o) = default;

        Task& operator=(const Task& o) = delete;
        Task& operator=(Task&& o) = default;

        bool valid() const
        {
            return !address.empty();
        }
    };

    struct Device
    {
        std::string address;

        Device(std::string&& address)
            : address(std::move(address))
        {}

        Device(const Device& o) = delete;
        Device(Device&& o) = default;

        Device& operator=(const Device& o) = delete;
        Device& operator=(Device&& o) = default;
    };

private:
    std::queue<Task> m_radioTaskQueue;
    std::unordered_map<DeviceIndex, Device> m_devices;
    Task m_executingTask;
    bool m_radioTaskExecuting = false;

    void createRadioTasks(Command command, const std::vector<DeviceIndex>& devices);
    void executeNextRadioTask();
    void handleRadioSendCallback(IRadio::Command command, IRadio::Result result);
};

#endif // HUB2_H
