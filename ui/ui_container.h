#ifndef UI_CONTAINER_H
#define UI_CONTAINER_H

#include "pico/stdlib.h"

typedef struct {
    // Physical properties
    uint16_t    x;
    uint16_t    y;
    uint16_t    w;
    uint16_t    h;
    uint16_t    x_ofst;
    uint16_t    y_ofst;
    bool        rel_ofst;
} ui_container_t;

#endif