#ifndef UI_WINDOW_H
#define UI_WINDOW_H

#include "ui.h"
#include "ui_widget.h"
#include "ui_container.h"
#include "ui_colors.h"

typedef struct {
    // Common widget params
    ui_widget_t     base;

    // Display properties
    ui_color_t16    border_color_outer;
    ui_color_t16    border_color_inner;
    ui_color_t16    background_color;
} ui_window_t;

void ui_window_init(ui_window_t* wnd, ui_draw_function_t draw_fn);

#endif