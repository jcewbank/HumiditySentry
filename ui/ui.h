#ifndef UI_H
#define UI_H
#include "pico/stdlib.h"

#define PARENT(W) \
    ((ui_widget_t *)((ui_widget_t *)((W)))->parent)

typedef struct {
    char pad;
} ui_ctx_t;

#endif