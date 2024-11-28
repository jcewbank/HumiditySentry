#include "i2c_main.h"
#include "i2c_util.h"
#include "seesaw/seesaw.h"
#include "sht40/sht40.h"

static seesaw_dev_t     seesaw_soil_dev;

volatile seesaw_soil_data_t
                        soil_data;
volatile sht40_trh_data_t
                        trh_data;
volatile sht40_err_t8   sht40_err;

void i2c_prj_init(){
    memset(&soil_data, 0, sizeof(seesaw_soil_data_t));
    sht40_err = SHT40_ERR_OK;

    sht40_init(&trh_data);
    seesaw_init(&seesaw_soil_dev, SEESAW_SOIL_I2C_ADDR, true);
}

void i2c_prj_probe_sensors(){
    // SHT40 temp and humidity
    sht40_err = sht40_sample(&trh_data);

    // Adafruit seesaw soil moisture sensor
    if(seesaw_soil_dev.present){
        soil_data.temp = seesaw_get_temp(&seesaw_soil_dev);
        soil_data.moisture = seesaw_touch_read(&seesaw_soil_dev, 0);
        soil_data.valid = true;
    }
    else{
        soil_data.valid = false;
    }
}