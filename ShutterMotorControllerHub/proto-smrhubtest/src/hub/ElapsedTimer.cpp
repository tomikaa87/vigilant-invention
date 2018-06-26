/*
 * ElapsedTimer.cpp
 *
 *  Created on: 2018. máj. 31.
 *      Author: tkarpati
 */

#include <hub/ElapsedTimer.h>

uint32_t ElapsedTimer::sTicks = 0;

void ElapsedTimer::tick()
{
    ++sTicks;
}

ElapsedTimer::ElapsedTimer()
    : mStartTicks(sTicks)
{
}

void ElapsedTimer_tick()
{
    ElapsedTimer::tick();
}

void ElapsedTimer::reset()
{
    mStartTicks = sTicks;
}

uint32_t ElapsedTimer::elapsed() const
{
    return sTicks - mStartTicks;
}
