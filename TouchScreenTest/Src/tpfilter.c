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


#define HALF_WINDOW_SIZE    2
#define WINDOW_SIZE         (2 * HALF_WINDOW_SIZE + 1)

#define SWAP(A, B) { uint16_t T = (A); (A) = (B); (B) = T; }

/* Fast, destructive median selection, credit to Nicolas Devillard for
 * the original C implementation.
 */
static uint16_t quickSelect(uint16_t* values, uint8_t count)
{
    uint8_t low = 0;
    uint8_t high = count - 1;
    uint8_t median = (low + high) / 2;

    for (;;)
    {
        // One element only
        if (high <= low)
            return values[median];

        // Two elements only
        if (high == low + 1)
        {
            if (values[low] > values[high])
                SWAP(values[low], values[high]);
            return values[median];
        }

        // Find median of low, middle and high items; swap to low position
        uint8_t middle = (low + high) / 2;
        if (values[middle] > values[high])
            SWAP(values[middle], values[high]);
        if (values[low] > values[high])
            SWAP(values[low], values[high]);
        if (values[middle] > values[low])
            SWAP(values[middle], values[low]);

        // Swap low item (now in position middle) into position (low + 1)
        SWAP(values[middle], values[low + 1]);

        // Work from each end towards middle, swapping items when stuck
        uint8_t low2 = low + 1;
        uint8_t high2 = high;
        for (;;)
        {
            do
            {
                ++low2;
            }
            while (values[low] > values[low2]);

            do
            {
                --high2;
            }
            while (values[low] < values[high2]);

            if (low2 > high2)
                break;

            SWAP(values[low2], values[high2]);
        }

        // Swap middle item (in position low) back into correct position
        SWAP(values[low], values[high2]);

        // Reset active partition
        if (high2 <= median)
            low = low2;
        if (high2 >= median)
            high = high2 - 1;
    }
}

static void updateFilter(uint16_t value, uint8_t* windowIndex, uint16_t* window, uint16_t* median)
{
    uint16_t removeValue = window[*windowIndex];
    window[*windowIndex] = value;

    if (value > *median && removeValue <= *median)
    {
        uint16_t lowestAboveMedian = UINT16_MAX;
        uint8_t count = 0;

        for (uint8_t i = 0; i < WINDOW_SIZE; ++i)
        {
            if (window[i] > *median)
            {
                ++count;

                if (window[i] < lowestAboveMedian)
                    lowestAboveMedian = window[i];
            }
        }

        if (count > HALF_WINDOW_SIZE)
            *median = lowestAboveMedian;
    }
    else if (value < *median && removeValue >= *median)
    {
        uint16_t highestBelowMedian = 0;
        uint8_t count = 0;

        for (uint8_t i = 0; i < WINDOW_SIZE; ++i)
        {
            if (window[i] < *median)
            {
                ++count;

                if (window[i] > highestBelowMedian)
                    highestBelowMedian = window[i];
            }
        }

        if (count > HALF_WINDOW_SIZE)
            *median = highestBelowMedian;
    }

    if (++(*windowIndex) >= WINDOW_SIZE)
        *windowIndex = 0;
}

void TPFilter_Input(uint16_t* x, uint16_t* y)
{
    static uint16_t xWindow[WINDOW_SIZE];
    static uint16_t xMedian = 0;
    static uint8_t xWindowIndex = 0;

    static uint16_t yWindow[WINDOW_SIZE];
    static uint16_t yMedian = 0;
    static uint8_t yWindowIndex = 0;

    static bool firstSample = true;

    // Fill up the window for the first time
    if (firstSample)
    {
        firstSample = false;

        for (uint8_t i = 0; i < WINDOW_SIZE; ++i)
        {
            updateFilter(*x, &xWindowIndex, xWindow, &xMedian);
            updateFilter(*y, &yWindowIndex, yWindow, &yMedian);
        }

        return;
    }

    updateFilter(*x, &xWindowIndex, xWindow, &xMedian);
    updateFilter(*y, &yWindowIndex, yWindow, &yMedian);

#if 0
    printf("xWindow:");
    for (uint8_t i = 0; i < WINDOW_SIZE; ++i)
        printf(" %04d", xWindow[i]);
    printf("\r\n");

    printf("yWindow:");
    for (uint8_t i = 0; i < WINDOW_SIZE; ++i)
        printf(" %04d", yWindow[i]);
    printf("\r\n");
#endif

    *x = xMedian;
    *y = yMedian;
}



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

#define IIR_N   6
#define IIR_D   10

void iirFilterInit(IIRFilter* filter)
{
    filter->s = 0;
}

uint16_t iirFilterRun(IIRFilter* filter, uint16_t sample, bool reset)
{
    if (reset)
    {
        filter->s = sample;
        return sample;
    }

    filter->s = (IIR_N * filter->s + (IIR_D - IIR_N) * sample + IIR_D / 2) / IIR_D;

    return filter->s;
}

#define DEBOUNCE_P  5u

#define DEBOUNCE_IS_ON(FILTER) (FILTER.s >= DEBOUNCE_P)

void debounceFilterInit(DebounceFilter* filter)
{
    filter->s = 0;
}

bool debounceFilterRun(DebounceFilter* filter, bool x)
{
    if (!x)
    {
        filter->s = 0;
        return false;
    }

    if (filter->s < DEBOUNCE_P)
        ++filter->s;

    return filter->s >= DEBOUNCE_P;
}

void TPFilter_InitChannel(TPFilter_Channel* channel)
{
    iirFilterInit(&(channel->i));
    medianFilterInit(&(channel->m));
    debounceFilterInit(&(channel->d));
}

uint16_t TPFilter_InputSample(TPFilter_Channel* channel, uint16_t sample, bool pressed)
{
    uint16_t median = medianFilterRun(&(channel->m), sample);
    bool rst = !debounceFilterRun(&(channel->d), pressed);
    return iirFilterRun(&(channel->i), median, rst);
}

bool TPFilter_IsPressed(TPFilter_Channel* channel)
{
    return DEBOUNCE_IS_ON(channel->d);
}
