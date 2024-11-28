#include "pico/stdlib.h"
#include "humidifier_main.h"
#include "../i2c_prj/seesaw/seesaw.h"

#define HUMIDIFIER_GPIO_PIN (14)
#define HUMIDITY_THRESHOLD (60000)

extern volatile seesaw_soil_data_t soil_data;

static humidifier_state_t state;

void humidifier_init(){
    memset(&state, 0, sizeof(humidifier_state_t));
    gpio_init(14);
    gpio_set_dir(14, GPIO_OUT);
}

void humidifier_main(){
    if(soil_data.valid
    && soil_data.moisture >= HUMIDITY_THRESHOLD 
    && !state.on){
        state.on = true;
        gpio_put(14, 1);
    }
    else if(state.on
    && soil_data.moisture < HUMIDITY_THRESHOLD){
        state.on = false;
        gpio_put(14, 0);
    }
}