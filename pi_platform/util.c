#include "settings.h"
#include "util.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h"

int PlatformInit(){
    stdio_init_all();

    // Initialise the Wi-Fi chip            
    if (cyw43_arch_init()) {    
        printf("Wi-Fi init failed\n");
        return -1;
    }

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

    printf("System Clock Frequency is %d Hz\n", clock_get_hz(clk_sys));
    printf("USB Clock Frequency is %d Hz\n", clock_get_hz(clk_usb));
    // For more examples of clocks use see https://github.com/raspberrypi/pico-examples/tree/master/clocks

    // Example to turn on the Pico W LED
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

    // Set up our UART
    uart_init(UART_ID, BAUD_RATE);
    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    // gpio_set_function(BACKLIGHT_GPIO_PIN, GPIO_FUNC_PWM);
    // uint32_t slice_num = pwm_gpio_to_slice_num(BACKLIGHT_GPIO_PIN);
    // pwm_set_wrap(slice_num, 255 * 255);
    // pwm_set_gpio_level(BACKLIGHT_GPIO_PIN, 0);
    // pwm_set_enabled(slice_num, true);
}

uint16_t PlatformCrc8(uint16_t seed){
    // TODO
    return 0;
}

// int64_t alarm_callback(alarm_id_t id, void *user_data) {
//     // Put your timeout handler code in here
//     return 0;
// }