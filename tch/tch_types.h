#ifndef TCH_TYPES_H
#define TCH_TYPES_H

#include "pico/stdlib.h"

typedef struct {
    uint16_t    x;
    uint16_t    y;
} tch_point_t;

typedef struct {
    uint32_t    down;
    uint32_t    up;
} tch_press_event_t;

typedef struct {
    bool                pressed;
	tch_point_t         coords;
    tch_press_event_t   last_press;
} tch_state_t;

#endif