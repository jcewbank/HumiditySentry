// sht40.cpp

#include "sht40.h"
#include "../i2c_util.h"

void sht40_init(sht40_trh_data_t *data){
    memset((void *)data, 0, sizeof(sht40_trh_data_t));
}

float convert_raw_temp_c(uint8_t *);
float convert_raw_temp_f(uint8_t *);
float convert_raw_rh(uint8_t *);

// Section 4.6 Conversion of Signal Output
float convert_raw_temp_c(uint8_t buf[2]){
    uint16_t adj_t = buf[0] * 256 + buf[1];
    float temp_c = -45.0f + (175.0f * (adj_t / 65535.0f));
    return temp_c;
}

float convert_raw_temp_f(uint8_t buf[2]){
    uint16_t adj_t = buf[0] * 256 + buf[1];
    float temp_f = -49.0f + (315 * (adj_t / 65535.0f));
    return temp_f;
}

float convert_raw_rh(uint8_t buf[2]){
        uint16_t adj_rh = buf[0] * 256 + buf[1];
        float rh = -6.0f + (125.0f * (adj_rh / 65535.0f));
        return rh;
}

// Do a blocking I2C write and read to retrieve data
sht40_err_t8 sht40_sample(sht40_trh_data_t *sample_data){
    // Data buffer
    uint8_t buf[SHT40_I2C_BUF_SZ];
    sht40_raw_trh_data_t *raw_data;

    // Send the command, wait necessary time, receive sample
    buf[0] = SHT40_CMD_MEAS_TRH_HI;
    i2c_write_blocking(I2C_PORT, SHT40_I2C_ADDR, buf, 1, true);
    
    // TODO JCE might need to busywait for 1ms here
    //sleep_ms(10);
    busy_wait_us_32(2000);

    memset(buf, 0, sizeof(buf));
    i2c_read_blocking(I2C_PORT, SHT40_I2C_ADDR, buf, 6, false);
    
    // Check the CRCs
    uint8_t crc;
    raw_data = (sht40_raw_trh_data_t *)&buf[0]; // Point to the temp offset
    crc = calculateCRC8((const uint8_t *)&raw_data->data, 2);
    if(crc != raw_data->crc){
        return(SHT40_ERR_CRC_TEMP);
    }
    sample_data->temp = convert_raw_temp_c((uint8_t *)&raw_data->data);

    raw_data = (sht40_raw_trh_data_t *)&buf[3]; // Point to the RH offset
    crc = calculateCRC8((const uint8_t *)&raw_data->data, 2);
    if(crc != raw_data->crc){
        return(SHT40_ERR_CRC_RH);
    }
    sample_data->rh = convert_raw_rh((uint8_t *)&raw_data->data);
    return(SHT40_ERR_OK);
}
