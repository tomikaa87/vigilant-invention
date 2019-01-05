/*
 * UI.h
 *
 *  Created on: 2019. jan. 3.
 *      Author: tomikaa
 */

#ifndef UI_H_
#define UI_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

void UI_Init();
void UI_SendTouchEvent(uint8_t x, uint8_t y, bool pressed);
void UI_Task();

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

namespace UI
{
class Widget;

void registerWidget(Widget* widget);
void unregisterWidget(Widget* widget);
}

#endif

#endif /* UI_H_ */
