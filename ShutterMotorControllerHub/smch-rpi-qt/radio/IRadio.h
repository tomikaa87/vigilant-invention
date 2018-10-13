#ifndef IRADIO_H
#define IRADIO_H

#include <functional>
#include <string>

namespace radio
{

class IRadio
{
public:
    virtual ~IRadio() = default;

    enum class Command
    {
        Shutter1Up,
        Shutter1Down,
        Shutter2Up,
        Shutter2Down,
        AllUp,
        AllDown
    };

    enum class Result
    {
        Succeeded,
        Busy,
        PacketLost
    };

    using SendCallback = std::function<void(Command command, Result result)>;

    virtual void send(Command command,
                      const std::string& address,
                      SendCallback&& callback) = 0;
};

}

#endif // IRADIO_H
