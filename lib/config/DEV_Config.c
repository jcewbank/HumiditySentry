/*****************************************************************************
* | File      	:	DEV_Config.c
* | Author      :   Waveshare team
* | Function    :	Show SDcard BMP picto LCD 
* | Info        :
*   Provide the hardware underlying interface	 
*----------------
* |	This version:   V1.0
* | Date        :   2018-01-11
* | Info        :   Basic version
*
******************************************************************************/
#include "DEV_Config.h"
#include "hardware/pwm.h"

void DEV_Digital_Write(UWORD Pin, UBYTE Value)
{
	gpio_put(Pin, Value);
}

UBYTE DEV_Digital_Read(UWORD Pin)
{
	return gpio_get(Pin);
}

/**
 * GPIO Mode
**/
void DEV_GPIO_Mode(UWORD Pin, UWORD Mode)
{
    gpio_init(Pin);
    if(Mode == 0 || Mode == GPIO_IN) {
        gpio_set_dir(Pin, GPIO_IN);
    } else {
        gpio_set_dir(Pin, GPIO_OUT);
    }
}

void DEV_GPIO_Init(void)
{
	DEV_GPIO_Mode(LCD_RST_PIN,GPIO_OUT);
    DEV_GPIO_Mode(LCD_DC_PIN, GPIO_OUT);
    DEV_GPIO_Mode(LCD_CS_PIN, GPIO_OUT);
    DEV_GPIO_Mode(TP_CS_PIN,GPIO_OUT);
    DEV_GPIO_Mode(TP_IRQ_PIN,GPIO_IN);
    DEV_GPIO_Mode(SD_CS_PIN,GPIO_OUT);
	gpio_set_pulls(TP_IRQ_PIN,true,false);

    DEV_Digital_Write(TP_CS_PIN, 1);
    DEV_Digital_Write(LCD_CS_PIN, 1);
    DEV_Digital_Write(SD_CS_PIN, 1);

	gpio_init(LCD_BKL_PIN);
	gpio_set_function(LCD_BKL_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(13);

    // Set PWM frequency and duty cycle
    pwm_set_clkdiv(slice_num, 62.5f);   // (125MHz / 62.5) = 2MHz base clock
    pwm_set_wrap(slice_num, 7999);      // 2MHz / (7999 + 1) ≈ 250 Hz

    // Set initial duty cycle (0-255 for 8-bit)
    pwm_set_chan_level(slice_num, PWM_CHAN_B, 65535);  // max brightness
    
    // Enable PWM
    pwm_set_enabled(slice_num, true);

}


/********************************************************************************
function:	System Init
note:
	Initialize the communication method
********************************************************************************/
uint8_t System_Init(void)
{
    DEV_GPIO_Init();
	// Pi Pico theoretical max SPI speed is 125MHz (the default system clock)
	// Controller max is ~60MHz
	spi_init(SPI_PORT, 4000000);
	gpio_set_function(LCD_CLK_PIN,GPIO_FUNC_SPI);
	gpio_set_function(LCD_MOSI_PIN,GPIO_FUNC_SPI);
	gpio_set_function(LCD_MISO_PIN,GPIO_FUNC_SPI);

    return 0;
}

void System_Exit(void)
{

}

/*********************************************
function:	Hardware interface
note:
	SPI4W_Write_Byte(value) : 
		Register hardware SPI
*********************************************/	
uint8_t SPI4W_Write_Byte(uint8_t value)                                    
{   
	uint8_t rxDat;
	spi_write_read_blocking(spi1,&value,&rxDat,1);
    return rxDat;
}

uint8_t SPI4W_Read_Byte(uint8_t value)                                    
{
	return SPI4W_Write_Byte(value);
}

/********************************************************************************
function:	Delay function
note:
	Driver_Delay_ms(xms) : Delay x ms
	Driver_Delay_us(xus) : Delay x us
********************************************************************************/
void Driver_Delay_ms(uint32_t xms)
{
	sleep_ms(xms);
}

void Driver_Delay_us(uint32_t xus)
{
	int j;
    for(j=xus; j > 0; j--);
}
