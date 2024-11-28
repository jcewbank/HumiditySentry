#include "LCD_Driver.h"
#include "../../pi_platform/settings.h"
#include "pico/stdlib.h"

#define BYTES_PER_PIXEL (2u)

void LCD2_Flush(uint16_t* fb, uint32_t fb_sz);

void LCD2_SetColor(
    COLOR       Color,
    uint16_t*   fb,
    uint32_t    fb_sz
    );

void LCD2_SetColor_Rect(
    COLOR       Color,
    uint16_t    x,
    uint16_t    y,
    uint16_t    w,
    uint16_t    h,
    uint16_t    cnvs_width,
    uint16_t*   fb,
    uint32_t    fb_sz
    );

void LCD2_SetColor_HorizontalLine(
    COLOR       Color,
    uint16_t    x,
    uint16_t    y,
    uint16_t    w,
    uint16_t    cnvs_width,
    uint16_t*   fb,
    uint32_t    fb_sz
    );

void LCD2_SetColor_VerticalLine(
    COLOR       Color,
    uint16_t    x,
    uint16_t    y,
    uint16_t    h,
    uint16_t    cnvs_width,
    uint16_t*   fb,
    uint32_t    fb_sz
    );