#ifndef UI_COLORS_H
#define UI_COLORS_H
#include "pico/stdlib.h"

// Note: these are byte-swapped for direct insertion in
// the framebuffer.
typedef enum {
    UI_COLOR_WHITE   = 0xFFFF,
    UI_COLOR_BLACK   = 0x0000,
    UI_COLOR_BLUE    = 0x1F00,
    UI_COLOR_BRED    = 0X1FF8,
    UI_COLOR_GRED    = 0XE0FF,
    UI_COLOR_GBLUE   = 0XFF07,
    UI_COLOR_RED     = 0x00F8,
    UI_COLOR_MAGENTA = 0x1FF8,
    UI_COLOR_GREEN   = 0xE007,
    UI_COLOR_CYAN    = 0xFF7F,
    UI_COLOR_YELLOW  = 0xE0FF,
    UI_COLOR_BROWN   = 0X40BC,
    UI_COLOR_BRRED   = 0X07FC,
    UI_COLOR_GRAY    = 0X3084,
    UI_COLOR_PINK    = 0x19F8,
    UI_COLOR_MINT    = 0xC94F,

    UI_COLOR_CNT = 14 
    } ui_color_t16;

// ST7789VW manual 8.8.36 RGB 5-6-5 format
typedef struct {
    uint16_t blue   : 5;
    uint16_t green  : 6;
    uint16_t red    : 5;
    } ui_color_rgb_t16;

typedef union {
    ui_color_rgb_t16    rgb;
    uint16_t            raw;
    } ui_color_t;

#endif