#ifndef I2C_UTIL_H
#define I2C_UTIL_H

#include "pico/stdlib.h"

uint8_t calculateCRC8(const uint8_t *data, size_t length);

#endif