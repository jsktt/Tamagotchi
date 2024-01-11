/*
 * Graphics.h
 *
 *  Created on: Dec 30, 2019
 *      Author: Matthew Zhong
 */

#ifndef HAL_GRAPHICS_H_
#define HAL_GRAPHICS_H_

#include <HAL/LcdDriver/Crystalfontz128x128_ST7735.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>

#define FG_COLOR GRAPHICS_COLOR_BLACK
#define BG_COLOR GRAPHICS_COLOR_WHITE

void GFX_construct(Graphics_Context *gfxContext, uint32_t defaultForeground, uint32_t defaultBackground);

#endif /* HAL_GRAPHICS_H_ */
