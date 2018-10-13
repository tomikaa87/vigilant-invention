#ifndef HUB2_H
#define HUB2_H

#include <memory>
#include <queue>
#include <map>
#include <QObject>

#include "IRemoteControl.h"
#include "radio/IRadio.h"
#include "hub/Task.h"

namespace hub
{

class Hub : public QObject, public IRemoteControl, public std::enable_shared_from_this<Hub>
{
    Q_OBJECT

public:
    Hub(const std::shared_ptr<radio::IRadio>& radio,
         QObject *parent = nullptr);

    // IRemoteControl interface
public:
    void execute(Command command, DeviceIndex device) override;
    void execute(Command command, std::vector<DeviceIndex>&& devices) override;
    void executeOnAll(Command command) override;

private:
    const std::shared_ptr<radio::IRadio> m_radio;

public:
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
    std::queue<hub::Task> m_radioTaskQueue;
    std::map<DeviceIndex, Device> m_devices;
    hub::Task m_executingTask;
    bool m_radioTaskExecuting = false;

    void createRadioTasks(Command command, const std::vector<DeviceIndex>& devices);
    void executeNextRadioTask();
    void handleRadioSendCallback(radio::IRadio::Command command, radio::IRadio::Result result);
};

}

#endif // HUB2_H
