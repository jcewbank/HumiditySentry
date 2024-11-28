#ifndef UI_TEXT_H
#define UI_TEXT_H

#include "pico/stdio.h"
#include "string.h"
#include "ui_widget.h"
#include "ui_container.h"
#include "fonts.h"

typedef struct {
    // Common widget params
    ui_widget_t     base;

    // Widget-specific
    char*           txt;
    uint8_t         sz;
    ui_color_t      bg_color;
    ui_color_t      fg_color;
    sFONT*          font;
} ui_text_t;

void ui_text_init(ui_text_t* ui_text, char* str, uint8_t sz);

#endif