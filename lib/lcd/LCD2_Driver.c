#include "LCD2_Driver.h"
#include <stdint.h>
#include "hardware/dma.h"

static void dma_fill_16(uint16_t *buffer, uint16_t value, size_t size) {
    // Configure DMA channel
    int dma_channel = dma_claim_unused_channel(true);
    bool dma_channel_claimed = dma_channel_is_claimed(dma_channel);
    if(dma_channel_claimed){
        dma_channel_config config = dma_channel_get_default_config(dma_channel);
        channel_config_set_transfer_data_size(&config, DMA_SIZE_16);
        channel_config_set_read_increment(&config, false);  // No increment on source
        channel_config_set_write_increment(&config, true);  // Increment on destination

        // Start transfer
        dma_channel_configure(
            dma_channel,
            &config,
            buffer,          // Destination address
            &value,          // Source address (single value)
            size,            // Number of 16-bit transfers
            true             // Start immediately
        );

        // Wait for completion
        dma_channel_wait_for_finish_blocking(dma_channel);

        // Release the channel
        dma_channel_unclaim(dma_channel);
    }
}

void LCD2_SetColor(
    COLOR       Color,
    uint16_t*   fb,
    uint32_t    fb_sz
    ){
    dma_fill_16(
        fb,
        Color,
        fb_sz
    );
}

void LCD2_SetColor_Rect(
    COLOR       Color,
    uint16_t    x,
    uint16_t    y,
    uint16_t    w,
    uint16_t    h,
    uint16_t    cnvs_width,
    uint16_t*   fb,
    uint32_t    fb_sz
    )
    {
    for(uint16_t i = 0; i < h; i++){
        LCD2_SetColor_HorizontalLine(
            Color,
            x,
            y + i,
            w,
            cnvs_width,
            fb,
            fb_sz
        );
    }
}

void LCD2_SetColor_HorizontalLine(
    COLOR       Color,
    uint16_t    x,
    uint16_t    y,
    uint16_t    w,
    uint16_t    cnvs_width,
    uint16_t*   fb,
    uint32_t    fb_sz
    )
    {
    uint32_t fb_ofst = y * cnvs_width;
    assert((x + w) < cnvs_width);
    assert((fb_ofst + x + w) < (fb_sz / BYTES_PER_PIXEL));
    // Use DMA if big enough line
    if(w > 128){
        dma_fill_16(
            &fb[fb_ofst + x],
            Color,
            w
        );
    }
    else{
        for(uint16_t i = 0; i < w; i++){
            fb[fb_ofst + x + i] = Color;
        }
    }
}

void LCD2_SetColor_VerticalLine(
    COLOR       Color,
    uint16_t    x,
    uint16_t    y,
    uint16_t    h,
    uint16_t    cnvs_width,
    uint16_t*   fb,
    uint32_t    fb_sz
    ){
    uint32_t fb_ofst = y * cnvs_width;
    assert((y + h) < ((fb_sz / BYTES_PER_PIXEL) / cnvs_width));
    assert((fb_ofst + x) < (fb_sz / BYTES_PER_PIXEL));
    // Use DMA if big enough line
    for(uint16_t i = 0; i < h; i++){
        fb[fb_ofst + x + (i * cnvs_width)] = Color;
    }
}

void LCD2_Flush(uint16_t* fb, uint32_t fb_sz){
	// Set the LCD SRAM to receive (?)
    DEV_Digital_Write(LCD_DC_PIN,1);
    DEV_Digital_Write(LCD_CS_PIN,0);
    
    spi_write_blocking(spi1, (uint8_t *)fb, fb_sz);

	// Close up shop
	DEV_Digital_Write(LCD_CS_PIN,1);
}
