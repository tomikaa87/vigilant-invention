#ifndef MOCKRADIO_H
#define MOCKRADIO_H

#include "IRadio.h"

class MockRadio : public IRadio
{
public:
    MockRadio();

    // IRadio interface
public:
    void send(Command command, const std::string& address, SendCallback&& callback) override;
};

#endif // MOCKRADIO_H
