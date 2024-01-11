/*
 * Graphics.c
 *
 *  Created on: Dec 30, 2019
 *      Author: Matthew Zhong
 */

#include <HAL/Graphics.h>

void GFX_construct(Graphics_Context *gfxContext, uint32_t defaultForeground, uint32_t defaultBackground)
{
    // initializing the display
    Crystalfontz128x128_Init();
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);

    // setting up the graphics
    Graphics_initContext(gfxContext, &g_sCrystalfontz128x128, &g_sCrystalfontz128x128_funcs);
    Graphics_setFont(gfxContext, &g_sFontFixed6x8);

    Graphics_setForegroundColor(gfxContext, defaultForeground);
    Graphics_setBackgroundColor(gfxContext, defaultBackground);
    Graphics_clearDisplay(gfxContext);
}

