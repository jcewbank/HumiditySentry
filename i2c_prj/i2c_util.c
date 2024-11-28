#include "pico/stdlib.h"
#include "i2c_util.h"

// Section 4.4 Checksum Calculation
uint8_t calculateCRC8(const uint8_t *data, size_t length) {
    // CRC-8 with polynomial 0x31 (x^8 + x^5 + x^4 + 1)
    // Initial value: 0xFF
    // Final XOR: 0x00
    
    uint8_t crc = 0xFF; // Initial seed
    
    for (size_t i = 0; i < length; i++) {
        crc ^= data[i];
        
        for (uint8_t bit = 0; bit < 8; bit++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ 0x31;
            } else {
                crc = crc << 1;
            }
        }
    }
    
    return crc; // No final XOR needed since it's 0x00
}