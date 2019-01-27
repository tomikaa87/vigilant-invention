#pragma once

#include "IdeHelper.h"

#include <cstdint>

union ClientPacketHeader final
{
    struct
    {
        uint16_t magic;
        uint16_t payloadLength;
        uint16_t endMagic;
    };

    char data[6] = { 0 };
};