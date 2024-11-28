#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/timer.h"
#include "hardware/clocks.h"
#include "pico/cyw43_arch.h"
#include "hardware/uart.h"
#include "hardware/i2c.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c1
#define I2C_SDA 2
#define I2C_SCL 3

// UART defines
// By default the stdout UART is `uart0`, so we will use the second one
#define UART_ID uart1       
#define BAUD_RATE 115200

// Use pins 4 and 5 for UART1
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define UART_TX_PIN 4
#define UART_RX_PIN 5

// Waveshare design puts the backlight PWM on GPIO13
#define BACKLIGHT_GPIO_PIN 13

// SPI speed bounces around a bit during execution
#define TP_SPI_BAUDRATE (3 * MHZ)
#define LCD_SPI_BAUDRATE (62.5 * MHZ)

#endif