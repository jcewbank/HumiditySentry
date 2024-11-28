#ifndef UI_CNVS_H
#define UI_CNVS_H
#include "pico/stdlib.h"

typedef struct {
    uint16_t *          buf;
    uint32_t            buf_sz;
    uint16_t            width;
    uint16_t            height;
} ui_cnvs_t;

#endif