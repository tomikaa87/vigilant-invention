#pragma once

#include <cstdint>

union ClientPacketHeader
{
    struct
    {
        uint16_t magic;
        uint16_t payloadLength;
        uint16_t endMagic;
    };

    char data[6];
};