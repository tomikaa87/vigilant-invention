#pragma once

#include "ClientPacketHeader.h"

#include <cstdint>

class ClientPacketParser
{
public:
    ClientPacketParser();
    ~ClientPacketParser();

    void reset();
    void input(const char* data, const uint16_t length);

    // With this API some copying could be spared
    //uint16_t availableBufferSpace() const;
    //char* inputBuffer();

    bool isCompleted() const;

    const char* payload() const;

private:
    char m_buffer[1024] = { 0 };
    uint16_t m_bufferCursor = 0;
    uint16_t m_readCursor = 0;
    const char* m_payload = nullptr;
    bool m_completed = false;

    const ClientPacketHeader* m_packetHeader = nullptr;

    void processBuffer();
};

