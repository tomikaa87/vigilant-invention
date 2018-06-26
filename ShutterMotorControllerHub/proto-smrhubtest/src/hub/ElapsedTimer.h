/*
 * ElapsedTimer.h
 *
 *  Created on: 2018. máj. 31.
 *      Author: tkarpati
 */

#ifndef HUB_ELAPSEDTIMER_H_
#define HUB_ELAPSEDTIMER_H_

#ifdef __cplusplus

#include <stdint.h>

class ElapsedTimer
{
public:
    static void tick();

    ElapsedTimer();

    void reset();
    uint32_t elapsed() const;

private:
    static uint32_t sTicks;

    uint32_t mStartTicks = 0;
};

extern "C"
{
#endif

void ElapsedTimer_tick();

#ifdef __cplusplus
}
#endif

#endif /* HUB_ELAPSEDTIMER_H_ */
