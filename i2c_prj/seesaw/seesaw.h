// seesaw.h : based on the Adafruit C++ library
// (c) Adafruit etc

/*
 * This is part of Adafruit's seesaw driver for the Arduino platform.  It is
 * designed specifically to work with the Adafruit products that use seesaw
 * technology.
 *
 * These chips use I2C to communicate, 2 pins (SCL+SDA) are required
 * to interface with the board.
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * Written by Dean Miller for Adafruit Industries.
 *
 * BSD license, all text here must be included in any redistribution.
 *
 */

#ifndef SEESAW_H
#define SEESAW_H

#include "../../pi_platform/settings.h"

// Configuration
#define SEESAW_SOIL_I2C_ADDR (0x36)   // Adafruit soil sensor board. 
                                // Could be 0x44, 0x45, or 0x46 from manufacturer
                                
#define SEESAW_SOIL_I2C_BUF_SZ (6 * sizeof(uint8_t))
                                // Send buffer is 1 byte, receive is 6 bytes

/** Module Base Addreses
 *  The module base addresses for different seesaw modules.
 */
enum {
  SEESAW_STATUS_BASE = 0x00,
  SEESAW_GPIO_BASE = 0x01,
  SEESAW_SERCOM0_BASE = 0x02,
  SEESAW_TIMER_BASE = 0x08,
  SEESAW_ADC_BASE = 0x09,
  SEESAW_DAC_BASE = 0x0A,
  SEESAW_INTERRUPT_BASE = 0x0B,
  SEESAW_DAP_BASE = 0x0C,
  SEESAW_EEPROM_BASE = 0x0D,
  SEESAW_NEOPIXEL_BASE = 0x0E,
  SEESAW_TOUCH_BASE = 0x0F,
  SEESAW_KEYPAD_BASE = 0x10,
  SEESAW_ENCODER_BASE = 0x11,
  SEESAW_SPECTRUM_BASE = 0x12,
};

/** status module function address registers
 */
enum {
  SEESAW_STATUS_HW_ID = 0x01,
  SEESAW_STATUS_VERSION = 0x02,
  SEESAW_STATUS_OPTIONS = 0x03,
  SEESAW_STATUS_TEMP = 0x04,
  SEESAW_STATUS_SWRST = 0x7F,
};

/** touch module function address registers
 */
enum {
  SEESAW_TOUCH_CHANNEL_OFFSET = 0x10,
};

typedef struct {
    i2c_inst_t*         i2c_port;
    uint8_t             addr;
    bool                present;
    uint16_t            chip_id;
    uint32_t            options;

    // From the date/product code
    uint16_t            pid;
    uint8_t             date_year;
    uint8_t             date_month;
    uint8_t             date_day;

    uint32_t            _prod_date_code;
} seesaw_dev_t;

typedef struct {
    float               temp;
    uint16_t            moisture;
    bool                valid;
} seesaw_soil_data_t;

bool seesaw_init(seesaw_dev_t* dev, uint8_t addr, bool reset);
void seesaw_get_prod_date_code(seesaw_dev_t* dev);
float seesaw_get_temp(seesaw_dev_t* dev);
uint16_t seesaw_touch_read(seesaw_dev_t* dev, uint8_t pin);

#endif