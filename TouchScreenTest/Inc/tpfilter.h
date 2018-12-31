/*
 * tpfilter.h
 *
 *  Created on: 2018. dec. 30.
 *      Author: tomikaa
 */

#ifndef TPFILTER_H_
#define TPFILTER_H_


#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    uint16_t samples[5];
} MedianFilter;

typedef struct
{
    uint16_t s;
} IIRFilter;

typedef struct
{
    uint16_t s;
} DebounceFilter;

typedef struct
{
    uint16_t x;
    uint16_t y;
    uint16_t pressed;
} TPFilter_Sample;

typedef struct
{
    MedianFilter m;
    IIRFilter i;
    DebounceFilter d;
} TPFilter_Channel;

void TPFilter_InitChannel(TPFilter_Channel* channel);
uint16_t TPFilter_InputSample(TPFilter_Channel* channel, uint16_t sample, bool pressed);
bool TPFilter_IsPressed(TPFilter_Channel* channel);
void TPFilter_Input(uint16_t* x, uint16_t* y);


#endif /* TPFILTER_H_ */
