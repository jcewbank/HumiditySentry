#include "pi_platform/settings.h"
#include "pi_platform/util.h"
// #include "i2c_prj/sht40/sht40.h"
#include "i2c_prj/i2c_main.h"
#include "humidifier/humidifier_main.h"
// #include "examples/lcd_main.h"
#include "ui/ui_mainwindow.h"

#include "tch/tch_main.h"

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/timer.h"
#include "DEV_Config.h"
#include "LCD_Driver.h"
// #include "LCD_Touch.h"

#include "hardware/spi.h"

#include "string.h"
#include <math.h>

extern volatile tch_state_t touch_state;

typedef uint8_t lcd_dspl_state_t8;
enum {
    LCD_DSPL_STATE_ON,
    LCD_DSPL_STATE_DIM,
    LCD_DSPL_STATE_INACTV,
    LCD_DSPL_STATE_OFF,

    LCD_DSPL_STATE_CNT
};

static struct {
    uint64_t    last_user_updt;
    lcd_dspl_state_t8
                state;
} lcd_dspl_state;

#define DO_PRINT_SENSORS (false)

// I2C functions
// bool i2c_sample_sht40() {
//     sht40_trh_data_t lcl_temp_rh_data;
//     memset(&lcl_temp_rh_data, 0, sizeof(sht40_trh_data_t ));
//     sht40_err = sht40_sample(&lcl_temp_rh_data);
//     memcpy((void *)&temp_rh_data, &lcl_temp_rh_data, sizeof(sht40_trh_data_t));
// }

// Gather sensor and touch screen data on Core 1
#define SENSOR_PERIOD_MS (100)
#define TP_PERIOD_MS (1)
#define UI_PERIOD_MS (16)
void core1_main(){
    // Initialize sensors
    i2c_prj_init();
    humidifier_init();

    while(true){
        i2c_prj_probe_sensors();
        humidifier_main();
        sleep_ms(SENSOR_PERIOD_MS);
    }
}

void setup_spi(uint baud_rate) {
    spi_init(SPI_PORT, baud_rate);  // Set the SPI clock speed (baud rate)
 
    // Set up the SPI pins (MISO, MOSI, SCK, and CS)
    gpio_set_function(LCD_CLK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(LCD_MOSI_PIN, GPIO_FUNC_SPI);
    gpio_set_function(LCD_MISO_PIN, GPIO_FUNC_SPI);
 
    // Optional: Configure SPI in master mode, CPOL/CPHA, etc.
    // spi_set_format(SPI_PORT, 8, SPI_POL0_PHA0, SPI_MSB_FIRST);
}

// Function to change SPI clock frequency at runtime
void change_spi_frequency(uint baud_rate) {
    // Disable SPI temporarily (optional, to avoid issues during the change)
    spi_deinit(SPI_PORT);
    
    // Reconfigure SPI with the new baud rate (frequency)       
    setup_spi(baud_rate);
    
    // Re-enable SPI if you deinitialized it
    spi_init(SPI_PORT, baud_rate);
}

int main(){
    static uint64_t last_proc = 0;
    uint64_t this_proc = 0;

    int ok = PlatformInit();
    if(!ok){
        return(ok);
    }

    multicore_launch_core1(core1_main);

    // Initialize the user interface
    ui_init();
    tch_init();
    sleep_ms(100);

    // Default 4Mhz. Pi manual recommends max 50MHz but 125MHz is possible.
    change_spi_frequency(LCD_SPI_BAUDRATE);

    while(true){
        this_proc = get_absolute_time();

        // Sample the touch panel every 1ms
        if((last_proc + (TP_PERIOD_MS * 1000)) < this_proc){
            tch_main();
        }

        // Do UI processing every 16ms
        if((last_proc + (UI_PERIOD_MS * 1000)) < this_proc){
            ui_main();
        }

        // Dim and disable the LCD panel after inactivity
        if(touch_state.pressed){
            lcd_dspl_state.last_user_updt = this_proc;
            if(lcd_dspl_state.state != LCD_DSPL_STATE_ON){
                lcd_dspl_state.state = LCD_DSPL_STATE_ON;
                LCD_SetBackLight(0xFFFF);
            }
        }
        else{
            #define LCD_DSPL_TIMEOUT_DIM_US (5 * 1000 * 1000)
            #define LCD_DSPL_TIMEOUT_OFF_US (30 * 1000 * 1000)

            // Display is not off/dim but it should be
            if(lcd_dspl_state.state != LCD_DSPL_STATE_OFF){
                // Should it be off?
                if((lcd_dspl_state.last_user_updt + LCD_DSPL_TIMEOUT_OFF_US) < this_proc){
                    lcd_dspl_state.state = LCD_DSPL_STATE_OFF;
                    LCD_SetBackLight(0x0000);
                }
                // Should it be dimmed?
                else if(lcd_dspl_state.state != LCD_DSPL_STATE_DIM
                && (lcd_dspl_state.last_user_updt + LCD_DSPL_TIMEOUT_DIM_US) < this_proc){
                    lcd_dspl_state.state = LCD_DSPL_STATE_DIM;
                    LCD_SetBackLight(0x7FFF);
                }
            }
        }

        last_proc = this_proc;
    }
}
