#include "seesaw.h"

// #define ADAFRUIT_DELAY_US_DFLT (8)   // this isn't enough
#define ADAFRUIT_DELAY_US_DFLT (1000)
#define ADAFRUIT_TOUCH_RETRY_CNT (5)

static int32_t _write8(i2c_inst_t *i2c_port, uint8_t addr, uint8_t data, bool nostop){
    // Data buffer
    busy_wait_ms(1);
    return i2c_write_blocking(i2c_port, addr, &data, 1, nostop);
}

static int32_t _read8(i2c_inst_t *i2c_port, uint8_t addr, bool nostop){
    uint8_t data;
    busy_wait_ms(1);
    return(i2c_read_blocking(i2c_port, addr, &data, 1, nostop));
}

static int32_t _write(
    seesaw_dev_t*   dev,
    uint8_t         reg_base,
    uint8_t         reg,
    uint8_t*        buf,
    uint8_t         buf_write_ofst, // position to write pair into buffer
    uint8_t         buf_sz,
    bool            nostop){
    // Adafruit python library writes a byte pair here
    uint8_t range = reg - reg_base;
    assert(buf_sz >= (buf_write_ofst + 2));
    buf[buf_write_ofst] = reg_base;
    buf[buf_write_ofst + 1] = reg;
    busy_wait_ms(1);
    return i2c_write_blocking(
        dev->i2c_port, 
        dev->addr, 
        buf, 
        buf_write_ofst + 2, 
        nostop
        );
}

static int32_t _read_w_delay(
    seesaw_dev_t*   dev,
    uint8_t         reg_base,
    uint8_t         reg,
    uint8_t*        buf,
    uint16_t        buf_sz, // bytes to read
    bool            nostop,
    uint16_t        delay){
    int32_t err = PICO_ERROR_GENERIC;

    // Write the register pair
    err = _write(dev, reg_base, reg, buf, 0, 2, nostop);
    if(err < 0){
        return err;
    }

    // 8us is the Adafruit default
    // busy_wait_us(delay);
    busy_wait_us(delay);
    err = i2c_read_blocking(dev->i2c_port,
    dev->addr,
    buf,
    buf_sz,
    nostop
    );

    return err;
}

static int32_t _read(
    seesaw_dev_t*   dev,
    uint8_t         reg_base,
    uint8_t         reg,
    uint8_t*        buf,
    uint16_t        buf_sz, // bytes to read
    bool            nostop){
    return _read_w_delay(dev, reg_base, reg, buf, buf_sz, nostop, ADAFRUIT_DELAY_US_DFLT);
}

/*
*/

static bool _probe_for_device(seesaw_dev_t* dev){
    int32_t ret;
    uint8_t data = 0;
    busy_wait_ms(1);
    ret = i2c_write_blocking(dev->i2c_port, dev->addr, &data, 1, false);
    if(ret != PICO_OK){
        return false;
    }
    busy_wait_ms(1);
    ret = i2c_read_blocking(dev->i2c_port, dev->addr, &data, 1, false);
    return(ret >= 0);
    /*
    while not self.i2c.try_lock():
            time.sleep(0)
        try:
            self.i2c.writeto(self.device_address, b"")
        except OSError:
            # some OS's dont like writing an empty bytesting...
            # Retry by reading a byte
            try:
                result = bytearray(1)
                self.i2c.readfrom_into(self.device_address, result)
            except OSError:
                # pylint: disable=raise-missing-from
                raise ValueError("No I2C device at address: 0x%x" % self.device_address)
                # pylint: enable=raise-missing-from
        finally:
            self.i2c.unlock()
    */
}

static int32_t _sw_reset(seesaw_dev_t* dev){
    busy_wait_ms(1);
    return _write8(dev->i2c_port, SEESAW_STATUS_BASE, SEESAW_STATUS_SWRST, 0xFF);
}

static bool _reset(seesaw_dev_t* dev, uint8_t addr, float post_reset_delay){
    bool found = false;
    memset(dev, 0, sizeof(seesaw_dev_t));
    dev->i2c_port = I2C_PORT;
    dev->addr = addr;
    _sw_reset(dev);
    for (int retries = 0; retries < 10; retries++) {
      if (_probe_for_device(dev)) {
        found = true;
        break;
      }
      sleep_ms(10);
    }
    return found;
}

/*
    def sw_reset(self, post_reset_delay=0.5):
        """Trigger a software reset of the SeeSaw chip"""
        self.write8(_STATUS_BASE, _STATUS_SWRST, 0xFF)
        time.sleep(post_reset_delay)
*/

float seesaw_get_temp(seesaw_dev_t* dev){
    uint8_t buf[4] = {0};
    busy_wait_ms(1);
    _read(
        dev,
        SEESAW_STATUS_BASE,
        SEESAW_STATUS_TEMP,
        buf,
        4,
        false);
    int32_t ret = ((uint32_t)buf[0] << 24) | ((uint32_t)buf[1] << 16) |
                    ((uint32_t)buf[2] << 8) | (uint32_t)buf[3];
    return (1.0 / (1UL << 16)) * ret;
    /*
        def get_temp(self):
        """Read the temperature"""
        buf = bytearray(4)
        self.read(_STATUS_BASE, _STATUS_TEMP, buf, 0.005)
        buf[0] = buf[0] & 0x3F
        ret = struct.unpack(">I", buf)[0]
        return 0.00001525878 * ret
    */
}

uint16_t seesaw_touch_read(seesaw_dev_t* dev, uint8_t pin){
  uint16_t buf;
  uint8_t p = pin;
  uint16_t ret = 65535;
  bool success;

  for (uint8_t retry = 0; retry < ADAFRUIT_TOUCH_RETRY_CNT; retry++) {
    success = _read_w_delay(
        dev,
        SEESAW_TOUCH_BASE,
        SEESAW_TOUCH_CHANNEL_OFFSET + p,
        (uint8_t *)&buf,
        2,
        false,
        3000 + (retry * 1000)
        ) > 0;
    if(success && buf != 0xFFFF) {
    //   ret = ((uint16_t)buf[0] << 8) | buf[1];
        ret = buf;
        break;
    }
  }
  return ret;
}

static uint32_t _get_options(seesaw_dev_t* dev){
    _read(
        dev,
        SEESAW_STATUS_BASE,
        SEESAW_STATUS_OPTIONS,
        (uint8_t *)&dev->options,
        4,
        false
        );
}

static void _get_version_prod_date_code(seesaw_dev_t* dev){
    uint8_t buf[4] = {0};
    _read(
        dev,
        SEESAW_STATUS_BASE,
        SEESAW_STATUS_VERSION,
        buf,
        4,
        false
        );
    dev->_prod_date_code = *(uint32_t *)buf;
}

void seesaw_get_prod_date_code(seesaw_dev_t* dev){
    _get_version_prod_date_code(dev);

    dev->pid = dev->_prod_date_code >> 16;
    dev->date_year = dev->_prod_date_code & 0x3F;
    dev->date_month = (dev->_prod_date_code >> 7) & 0xF;
    dev->date_day = (dev->_prod_date_code >> 11) & 0x1F;
}

bool seesaw_init(seesaw_dev_t* dev, uint8_t addr, bool reset){
    uint32_t buf = {0};
    if(reset){
        if(!_reset(dev, addr, 0.5)){
            return(false);
        }
    }

    dev->present = true;
    _read(
        dev,
        SEESAW_STATUS_BASE,
        SEESAW_STATUS_HW_ID,
        (uint8_t *)&dev->chip_id,
        2,
        false
        );
    
    _get_options(dev);
    seesaw_get_prod_date_code(dev);

    // TODO JCE set the pinmap
}