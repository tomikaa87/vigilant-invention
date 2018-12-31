/*
 * tpfilter.c
 *
 *  Created on: 2018. dec. 30.
 *      Author: tomikaa
 */


#include "tpfilter.h"

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>


#define IIR_N                   6
#define IIR_D                   10

#define DEBOUNCE_P              5u
#define DEBOUNCE_IS_ON(FILTER)  (FILTER.sample >= DEBOUNCE_P)

void medianFilterCmpSwap(uint16_t* a, uint16_t* b)
{
    if (*a > *b)
    {
        uint16_t t = *a;
        *a = *b;
        *b = t;
    }
}

void medianFilterInit(MedianFilter* filter)
{
    memset(filter->samples, 0, 5 * sizeof(uint16_t));
}

uint16_t medianFilterRun(MedianFilter* filter, uint16_t sample)
{
    uint16_t* s = filter->samples;

    s[0] = s[1];
    s[1] = s[2];
    s[2] = s[3];
    s[3] = s[4];
    s[4] = sample;

    uint16_t t[5];
    memcpy(t, s, 5 * sizeof(uint16_t));

    medianFilterCmpSwap(&t[0], &t[1]);
    medianFilterCmpSwap(&t[2], &t[3]);
    medianFilterCmpSwap(&t[0], &t[2]);
    medianFilterCmpSwap(&t[1], &t[4]);
    medianFilterCmpSwap(&t[0], &t[1]);
    medianFilterCmpSwap(&t[2], &t[3]);
    medianFilterCmpSwap(&t[1], &t[2]);
    medianFilterCmpSwap(&t[3], &t[4]);
    medianFilterCmpSwap(&t[2], &t[3]);

    return t[2];
}

void iirFilterInit(IIRFilter* filter)
{
    filter->sample = 0;
}

uint16_t iirFilterRun(IIRFilter* filter, uint16_t sample, bool reset)
{
    if (reset)
    {
        filter->sample = sample;
        return sample;
    }

    filter->sample = (IIR_N * filter->sample + (IIR_D - IIR_N) * sample + IIR_D / 2) / IIR_D;

    return filter->sample;
}

void debounceFilterInit(DebounceFilter* filter)
{
    filter->sample = 0;
}

bool debounceFilterRun(DebounceFilter* filter, bool x)
{
    if (!x)
    {
        filter->sample = 0;
        return false;
    }

    if (filter->sample < DEBOUNCE_P)
        ++filter->sample;

    return filter->sample >= DEBOUNCE_P;
}

void TPFilter_InitChannel(TPFilter_Channel* channel)
{
    iirFilterInit(&(channel->iirFilter));
    medianFilterInit(&(channel->medianFilter));
    debounceFilterInit(&(channel->debounceFilter));
}

uint16_t TPFilter_InputSample(TPFilter_Channel* channel, uint16_t sample, bool pressed)
{
    uint16_t median = medianFilterRun(&(channel->medianFilter), sample);
    bool rst = !debounceFilterRun(&(channel->debounceFilter), pressed);
    return iirFilterRun(&(channel->iirFilter), median, rst);
}

bool TPFilter_IsPressedDebounced(TPFilter_Channel* channel)
{
    return DEBOUNCE_IS_ON(channel->debounceFilter);
}
