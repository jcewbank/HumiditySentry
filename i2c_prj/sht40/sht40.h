// sht40.h

#ifndef SHT40_H
#define SHT40_H

#include "../pi_platform/settings.h"

// Config
#define SHT40_POLL_RATE_MS (100)

// Types
typedef uint8_t sht40_precision_t8;
enum {
    SHT40_PRECISION_LOW = 0,
    SHT40_PRECISION_MED,
    SHT40_PRECISION_HIGH
};

typedef uint8_t sht40_err_t8;
enum {
    SHT40_ERR_OK = 0,
    SHT40_ERR_CRC_TEMP,
    SHT40_ERR_CRC_RH,
};

// Table 4.5 Command Overview
typedef uint8_t sht40_cmd_t8;
enum {
    SHT40_CMD_MEAS_TRH_LOW = 0xFD,
    SHT40_CMD_MEAS_TRH_MED = 0xF6,
    SHT40_CMD_MEAS_TRH_HI = 0xE0,
    SHT40_CMD_READ_SN = 0x89,
    SHT40_CMD_SOFT_RESET = 0x94,
    SHT40_CMD_HEATER_200MW_1000MS = 0x39,
    SHT40_CMD_HEATER_200MW_100MS = 0x32,
    SHT40_CMD_HEATER_110MW_1000MS = 0x2F,
    SHT40_CMD_HEATER_110MW_100MS = 0x24,
    SHT40_CMD_HEATER_20MW_1000MS = 0x1E,
    SHT40_CMD_HEATER_20MW_100MS = 0x15,
};

typedef struct {
    float   temp;
    float   rh;
} sht40_trh_data_t;

typedef union {
    uint16_t    value;
    uint8_t     buf[2];
} buf_16_t;

typedef struct {
    buf_16_t    data;
    uint8_t     crc;
} sht40_raw_trh_data_t;

typedef struct {
    sht40_precision_t8  precision;
} sht40_inst_t;

// Configuration
#define SHT40_I2C_ADDR (0x44)   // Adafruit SHT40 board. 
                                // Could be 0x44, 0x45, or 0x46 from manufacturer
                                
#define SHT40_I2C_BUF_SZ (6 * sizeof(uint8_t))
                                // Send buffer is 1 byte, receive is 6 bytes

// Public functions
void sht40_init(sht40_trh_data_t* data);

sht40_err_t8 sht40_sample(sht40_trh_data_t *);

#endif