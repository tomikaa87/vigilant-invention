#ifndef UTILS_H
#define UTILS_H

#include <cstdint>

namespace utils
{
    inline uint16_t composeTime(int h, int m)
    {
        return static_cast<uint16_t>(h << 8 | (m & 0xff));
    }
}

#endif // UTILS_H
