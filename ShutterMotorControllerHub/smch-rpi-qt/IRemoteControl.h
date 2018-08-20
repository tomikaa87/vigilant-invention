#ifndef IREMOTECONTROL_H
#define IREMOTECONTROL_H

#include <vector>

class IRemoteControl
{
public:
    enum class Command
    {
        ShutterUp,
        ShutterDown
    };

    enum class DeviceIndex
    {
        D0_0, D0_1,
        D1_0, D1_1,
        D2_0, D2_1,
        D3_0, D3_1,
        D4_0, D4_1,
        D5_0, D5_1,
        D6_0, D6_1,
        D7_0, D7_1,
        D8_0, D8_1,
        D9_0, D9_1
    };

    virtual void execute(Command command, DeviceIndex device) = 0;
    virtual void execute(Command command, std::vector<DeviceIndex>&& devices) = 0;
    virtual void executeOnAll(Command command) = 0;

    virtual ~IRemoteControl() = default;
};

#endif // IREMOTECONTROL_H
