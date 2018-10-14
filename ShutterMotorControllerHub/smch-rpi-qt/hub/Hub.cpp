#include "Hub.h"
#include "logging/LoggingHelpers.h"

#include <algorithm>
#include <iterator>
#include <thread>

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(HubLog)
Q_LOGGING_CATEGORY(HubLog, "Hub")

namespace hub
{

Hub::Hub(const std::shared_ptr<radio::IRadio>& radio, QObject* parent)
    : QObject{ parent }
    , m_radio{ radio }
{}

Hub::~Hub()
{
    m_queue.flushAndStopSync();
}

void hub::Hub::scanDevices()
{
    m_queue.enqueue([this] {
        std::string address{ "SMRR0" };

        for (char i = 0; i < 10; ++i)
        {
            address[4] = '0' + i;
            qCDebug(HubLog) << "looking for" << address.c_str();

            for (int j = 0; j < m_transmitRetryCount; ++j)
            {
                qCDebug(HubLog) << "  trying to read status...";

                auto f = m_radio->readStatus(address);
                f.wait();
                auto response = f.get();

                if (response.result == radio::Result::Succeeded)
                {
                    qCDebug(HubLog) << "  receiver found";

                    // TODO This redundancy should be fixed
                    auto primary = static_cast<DeviceIndex>(static_cast<int>(DeviceIndex::D0_0) + i);
                    auto secondary = static_cast<DeviceIndex>(static_cast<int>(primary) + 1);

                    Device dev{ address };
                    dev.firmwareVersion = std::string{ response.message->payload.status.firmware_ver };

                    qCDebug(HubLog) << "  firmware version:" << dev.firmwareVersion.c_str();

                    m_devices.emplace(primary, dev);
                    m_devices.emplace(secondary, dev);

                    break;
                }
                else
                {
                    qCDebug(HubLog) << "  packet lost";
                }
            }
        }
    });
}

void Hub::execute(Command command, DeviceIndex device)
{
    qCInfo(HubLog) << "executing" << command << "on" << device;

    executeCommandAsync(command, std::vector<DeviceIndex>{ device });
}

void Hub::execute(Command command, std::vector<DeviceIndex>&& devices)
{
    qCInfo(HubLog) << "executing" << command << "on" << devices;

    executeCommandAsync(command, std::move(devices));
}

void Hub::executeOnAll(Command command)
{
    qCInfo(HubLog) << "executing" << command << "on all devices";

    std::vector<DeviceIndex> indices;
    indices.reserve(m_devices.size());

    std::transform(std::begin(m_devices),
                   std::end(m_devices),
                   std::back_inserter(indices),
                   [](const std::pair<const hub::DeviceIndex, hub::Device>& p) { return p.first; });

    executeCommandAsync(command, std::move(indices));
}

radio::Command mapToRadioCommand(DeviceIndex index,
                                 Command command)
{
    int indexValue = static_cast<int>(index);
    bool primaryControl = indexValue % 2 == 0;

    switch (command)
    {
        case Command::ShutterDown:
            return primaryControl ? radio::Command::Shutter1Down : radio::Command::Shutter2Down;

        case Command::ShutterUp:
            return primaryControl ? radio::Command::Shutter1Up : radio::Command::Shutter2Up;
    }

    // TODO return proper value instead of this random one
    return radio::Command::Shutter1Down;
}

void Hub::executeCommandAsync(Command command, const std::vector<DeviceIndex>& devices)
{
    m_queue.enqueue([this, command, devices] {

        if (m_devices.empty())
        {
            qCWarning(HubLog) << "no active devices";
            return;
        }

        qCDebug(HubLog).nospace()
                << "Creating tasks for { command: " << command
                << ", devices: { " << devices << " } }";

        std::map<std::string, radio::Command> radioCommands;

        // Map device indices and commands to addresses and low level radio commands.
        // Also group commands addressed to the same device.
        for (const auto& d : devices)
        {
            int indexValue = static_cast<int>(d);
            int radioDeviceIndex = indexValue / 2;

            std::string deviceAddress{ "SMRR0" };
            deviceAddress[4] = static_cast<char>('0' + radioDeviceIndex);

            auto& groupedCommand = radioCommands[deviceAddress];
            auto newCommand = mapToRadioCommand(d, command);

            if (groupedCommand == radio::Command::AllDown || groupedCommand == radio::Command::AllUp)
            {
                continue;
            }
            else if ((groupedCommand == radio::Command::Shutter1Down && newCommand == radio::Command::Shutter2Down) ||
                     (groupedCommand == radio::Command::Shutter2Down && newCommand == radio::Command::Shutter1Down))
            {
                groupedCommand = radio::Command::AllDown;
            }
            else if ((groupedCommand == radio::Command::Shutter1Up && newCommand == radio::Command::Shutter2Up) ||
                     (groupedCommand == radio::Command::Shutter2Up && newCommand == radio::Command::Shutter1Up))
            {
                groupedCommand = radio::Command::AllUp;
            }
            else
            {
                groupedCommand = newCommand;
            }
        }

        for (const auto& rc : radioCommands)
        {
            for (int i = 0; i < m_transmitRetryCount; ++i)
            {
                auto f = m_radio->sendCommand(rc.second, rc.first);
                f.wait();
                auto response = f.get();
                qCDebug(HubLog) << "radio response:" << response.result;

                if (response.result == radio::Result::Succeeded)
                {
                    qCDebug(HubLog) << "command packet transmitted";
                    break;
                }

                qCDebug(HubLog) << "command packet lost";
            }
        }
    });
}

}
