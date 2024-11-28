/*****************************************************************************
* | File      	:	LCD_Driver.c
* | Author      :   Waveshare team
* | Function    :	LCD Drive function
* | Info        :
*   Image scanning
*      Please use progressive scanning to generate images or fonts
*----------------
* |	This version:   V1.0
* | Date        :   2018-01-11
* | Info        :   Basic version
*
******************************************************************************/

/**************************Intermediate driver layer**************************/
#include "LCD2_Driver.h"
// #include <stdlib.h>
// #include <stdalign.h> // For alignas

// #include "../../pi_platform/settings.h"
#include "hardware/pwm.h"

// scratch framebuffer

// #define BUFFER_SIZE 128
// static alignas(4) uint8_t fb[BUFFER_SIZE];

LCD_DIS sLCD_DIS;
uint8_t id;
/*******************************************************************************
function:
	Hardware reset
*******************************************************************************/
static void LCD_Reset(void)
{
    DEV_Digital_Write(LCD_RST_PIN,1);
    Driver_Delay_ms(500);
    DEV_Digital_Write(LCD_RST_PIN,0);
    Driver_Delay_ms(500);
    DEV_Digital_Write(LCD_RST_PIN,1);
    Driver_Delay_ms(500);
}

static const uint16_t BACKLIGHT_LEVELS[] = {
	0x000f,
	0x00fe,
	0x01fd,
	0x02fc,
	0x03fb,
	0x04fa,
	0x05f9,
	0x06f8,
	0x07f7,
	0x08f6,
	0x09f5,
	0x0af4,
	0x0bf3,
	0x0cf2,
	0x0df1,
	0x0ef0,
	0x0fef,
	0x10ee,
	0x11ed,
	0x12ec,
	0x13eb,
	0x14ea,
	// No perceptible difference beyond this point
	// 0x15e9,
	// 0x16e8,
	// 0x17e7,
	// 0x18e6,
	// 0x19e5,
	// 0x1ae4,
	};

// // Gamma correction function for smooth perceived brightness
static uint16_t gamma_correct(uint16_t brightness) {
    // Adjust the exponent between 2.0 and 3.0 for desired curve
    const float gamma = 3.0f;
    
    // Normalize input to 0-1 range
 	// By observation, screen is brightest around 0x17e7
 	// so scale input to that range as well.
 	// #define LCD_MAX_BRIGHT_SCALE (float)6119 / (float)65535
 	// brightness = (float)brightness * LCD_MAX_BRIGHT_SCALE;

 	// Normalize the input to 0-1
     float normalized = (float)brightness / 65535.0f;
    
     // Apply power law
     float corrected = powf(normalized, gamma);
    
     // Convert back to 16-bit range
     return (uint16_t)(corrected * 65535.0f);
 }

static uint16_t s_last_bl_value = 0;
void LCD_SetBackLight(uint16_t value)
{
	// Scale 0-255 into the known valid range of [0x0000, 0x16e8]
	if(value == s_last_bl_value){
		return;
	}
	s_last_bl_value = value;
    uint slice_num = pwm_gpio_to_slice_num(13);
	uint16_t correct = gamma_correct(value / 2);
	pwm_set_chan_level(slice_num, PWM_CHAN_B, correct);
}

/*******************************************************************************
function:
		Write register address and data
*******************************************************************************/
void LCD_WriteReg(uint8_t Reg)
{
    DEV_Digital_Write(LCD_DC_PIN,0);
    DEV_Digital_Write(LCD_CS_PIN,0);
    SPI4W_Write_Byte(Reg);
	DEV_Digital_Write(LCD_CS_PIN,1);
}

void LCD_WriteData(uint16_t Data)
{
	if(LCD_2_8 == id){
		DEV_Digital_Write(LCD_DC_PIN,1);
		DEV_Digital_Write(LCD_CS_PIN,0);
		SPI4W_Write_Byte((uint8_t)Data);
		DEV_Digital_Write(LCD_CS_PIN,1);
	}else{
		DEV_Digital_Write(LCD_DC_PIN,1);
		DEV_Digital_Write(LCD_CS_PIN,0);
		SPI4W_Write_Byte(Data >> 8);
		SPI4W_Write_Byte(Data & 0XFF);
		DEV_Digital_Write(LCD_CS_PIN,1);
	}
    
}

/*******************************************************************************
function:
		Write register data
*******************************************************************************/
static void LCD_Write_AllData(uint16_t Data, uint32_t DataLen)
{
    uint32_t i;
    DEV_Digital_Write(LCD_DC_PIN,1);
    DEV_Digital_Write(LCD_CS_PIN,0);
    for(i = 0; i < DataLen; i++) {
        SPI4W_Write_Byte(Data >> 8);
        SPI4W_Write_Byte(Data & 0XFF);
    }
	DEV_Digital_Write(LCD_CS_PIN,1);
}

/*******************************************************************************
function:
		Common register initialization
*******************************************************************************/
static void LCD_InitReg(void)
{
	id = LCD_Read_Id();
	if(LCD_2_8 == id){
		// Turn off sleep mode
		LCD_WriteReg(0x11);
		Driver_Delay_ms(100);

		// Memory data access mode
		LCD_WriteReg(0x36);
			// Page address order:
			//		Top to bottom
			// Column address order:
			//		Left to right
			// Page/Column order:
			//		Normal mode
			// Line address order:
			//		LCD refresh top to bottom
			// RGB/BGR order:
			//		RGB
			// Display data latch data order:
			//		LCD refresh left to right	
			LCD_WriteData(0x00);

		// Interface pixel format
		LCD_WriteReg(0x3a);
			// b'0101_0101
			// 65K color _ 16bit/px
			LCD_WriteData(0x55);

		// Porch setting
		LCD_WriteReg(0xb2);
			// BPA[6:0] back porch setting
			// b'0000_1100
			// 12
			// max 0x7F, min 0x01
			//(default) LCD_WriteData(0x0c);
			LCD_WriteData(0x5f);
			// FPA[6:0] front porch setting
			// 12
			// (default) LCD_WriteData(0x0c);
			LCD_WriteData(0x5f);
			// Seperate porch control enable (disabled)
			LCD_WriteData(0x00);
			// BPB[3:0] and FPB[3:0] idle mode back and front porch setting
			// b'0011_0011
			LCD_WriteData(0x33);
			// BPC[3:0] and FPC[3:0] partial mode back and front porch setting
			// b'0011_0011
			LCD_WriteData(0x33);
		// Gate control
		// Gate/scanning driver timing and voltage in HIGH and LOW power modes
		LCD_WriteReg(0xb7);
			// VGHS[2:0] 3: 13.26
			// VGLS[2:0] 5: -10.43
			LCD_WriteData(0x35);
		// VCOM setting
		// Common voltage settings. Specific to this panel
		LCD_WriteReg(0xbb);
			LCD_WriteData(0x28);
		// LCM control
		// Liquid Crystal Monitor control. Specific to this panel
		LCD_WriteReg(0xc0);
			LCD_WriteData(0x3c);
		// Analog power voltage levels for the display
		LCD_WriteReg(0xc2);
			LCD_WriteData(0x01);
			// VRH set
			LCD_WriteReg(0xc3);
				LCD_WriteData(0x0b);
			// VDV set
			LCD_WriteReg(0xc4);
				LCD_WriteData(0x20);
		// Frame rate control in normal mode	
		LCD_WriteReg(0xc6);
			// NLA [2:0] inversion selection in normal mode
			// RTNA [4:0] framerate in normal mode
			// b'0000_1111
			// dot inversion; 60hz framerate
			// Frame rate=10MHz/(320+FPA[6:0]+BPA[6:0])*(250+RTNA[4:0]*16
			// FPA|BPA|FR 0x0c|0x0c|0x0f is 59.33hz
			// FPA|BPA|FR 0x5f|0x5f|0x05 is 59.42hz
			// FPA|BPA|FR 0x7f|0x7f|0x00 is 69.69hz
			// (default) LCD_WriteData(0x0f);
			LCD_WriteData(0x05);
		LCD_WriteReg(0xD0);
			LCD_WriteData(0xa4);
			LCD_WriteData(0xa1);
		LCD_WriteReg(0xe0);
			LCD_WriteData(0xd0);
			LCD_WriteData(0x01);
			LCD_WriteData(0x08);
			LCD_WriteData(0x0f);
			LCD_WriteData(0x11);
			LCD_WriteData(0x2a);
			LCD_WriteData(0x36);
			LCD_WriteData(0x55);
			LCD_WriteData(0x44);
			LCD_WriteData(0x3a);
			LCD_WriteData(0x0b);
			LCD_WriteData(0x06);
			LCD_WriteData(0x11);
			LCD_WriteData(0x20);
		LCD_WriteReg(0xe1);
			LCD_WriteData(0xd0);
			LCD_WriteData(0x02);
			LCD_WriteData(0x07);
			LCD_WriteData(0x0a);
			LCD_WriteData(0x0b);
			LCD_WriteData(0x18);
			LCD_WriteData(0x34);
			LCD_WriteData(0x43);
			LCD_WriteData(0x4a);
			LCD_WriteData(0x2b);
			LCD_WriteData(0x1b);
			LCD_WriteData(0x1c);
			LCD_WriteData(0x22);
			LCD_WriteData(0x1f);
		LCD_WriteReg(0x55);
			LCD_WriteData(0xB0);
		LCD_WriteReg(0x29);
	}else{
		LCD_WriteReg(0x21);
		LCD_WriteReg(0xC2);	//Normal mode, increase can change the display quality, while increasing power consumption
		LCD_WriteData(0x33);
		LCD_WriteReg(0XC5);
		LCD_WriteData(0x00);
		LCD_WriteData(0x1e);//VCM_REG[7:0]. <=0X80.
		LCD_WriteData(0x80);
		LCD_WriteReg(0xB1);//Sets the frame frequency of full color normal mode
		LCD_WriteData(0xB0);//0XB0 =70HZ, <=0XB0.0xA0=62HZ
		LCD_WriteReg(0x36);
		LCD_WriteData(0x28); //2 DOT FRAME MODE,F<=70HZ.
		LCD_WriteReg(0XE0);
		LCD_WriteData(0x0);
		LCD_WriteData(0x13);
		LCD_WriteData(0x18);
		LCD_WriteData(0x04);
		LCD_WriteData(0x0F);
		LCD_WriteData(0x06);
		LCD_WriteData(0x3a);
		LCD_WriteData(0x56);
		LCD_WriteData(0x4d);
		LCD_WriteData(0x03);
		LCD_WriteData(0x0a);
		LCD_WriteData(0x06);
		LCD_WriteData(0x30);
		LCD_WriteData(0x3e);
		LCD_WriteData(0x0f);		
		LCD_WriteReg(0XE1);
		LCD_WriteData(0x0);
		LCD_WriteData(0x13);
		LCD_WriteData(0x18);
		LCD_WriteData(0x01);
		LCD_WriteData(0x11);
		LCD_WriteData(0x06);
		LCD_WriteData(0x38);
		LCD_WriteData(0x34);
		LCD_WriteData(0x4d);
		LCD_WriteData(0x06);
		LCD_WriteData(0x0d);
		LCD_WriteData(0x0b);
		LCD_WriteData(0x31);
		LCD_WriteData(0x37);
		LCD_WriteData(0x0f);
		LCD_WriteReg(0X3A);	//Set Interface Pixel Format
		LCD_WriteData(0x55);
		LCD_WriteReg(0x11);//sleep out
		Driver_Delay_ms(120);
		LCD_WriteReg(0x29);//Turn on the LCD display
	}
}

/********************************************************************************
function:	Set the display scan and color transfer modes
parameter:
		Scan_dir   :   Scan direction
		Colorchose :   RGB or GBR color format
********************************************************************************/
void LCD_SetGramScanWay(LCD_SCAN_DIR Scan_dir)
{
    uint16_t MemoryAccessReg_Data = 0; //addr:0x36
    uint16_t DisFunReg_Data = 0; //addr:0xB6

	if(LCD_2_8 == id){
		/*		it will support later		*/
		//Pico-ResTouch-LCD-2.8
		switch(Scan_dir){
			case L2R_U2D:
				/* Memory access control: MY = 0, MX = 0, MV = 0, ML = 0 RGB = 0 MH = 0 NN = 0 NN = 0*/
				MemoryAccessReg_Data = 0x00;
				break;	
			case D2U_L2R: 
				/* Memory access control: MY = 1, MX = 0, MV = 1, ML = 0 RGB = 0 MH = 0 NN = 0 NN = 0*/
				MemoryAccessReg_Data = 0xA0;
				break;
			case R2L_D2U: 				
				/* Memory access control: MY = 1, MX = 1, MV = 0, ML = 0 RGB = 0 MH = 0 NN = 0 NN = 0*/
				MemoryAccessReg_Data = 0xc0;
				break;
			case U2D_R2L: 
				/* Memory access control: MY = 0, MX = 1, MV = 1, ML = 0 RGB = 0 MH = 0 NN = 0 NN = 0*/
				MemoryAccessReg_Data = 0x60;				
				break;
			
		}
		sLCD_DIS.LCD_Scan_Dir = Scan_dir;
		//Get GRAM and LCD width and height
		//240*320,vertical default
		if(Scan_dir == L2R_U2D || Scan_dir == R2L_D2U) {
			sLCD_DIS.LCD_Dis_Column	= LCD_2_8_WIDTH ;
			sLCD_DIS.LCD_Dis_Page =  LCD_2_8_HEIGHT;
		} else {
			sLCD_DIS.LCD_Dis_Column	=  LCD_2_8_HEIGHT;
			sLCD_DIS.LCD_Dis_Page = LCD_2_8_WIDTH ;
		}

		
		LCD_WriteReg(0x36);
		LCD_WriteData(MemoryAccessReg_Data);
	}else{
		//Pico-ResTouch-LCD-3.5
		// Gets the scan direction of GRAM
		switch (Scan_dir) {
		case L2R_U2D:
			/* Memory access control: MY = 0, MX = 0, MV = 0, ML = 0 */
			/* Display Function control: NN = 0, GS = 0, SS = 1, SM = 0	*/
			MemoryAccessReg_Data = 0x08;
			DisFunReg_Data = 0x22;
			break;
		case R2L_D2U: 
			/* Memory access control: MY = 0, MX = 0, MV = 0, ML = 0 */
			/* Display Function control: NN = 0, GS = 1, SS = 0, SM = 0	*/
			MemoryAccessReg_Data = 0x08;
			DisFunReg_Data = 0x42;
			break;
		case U2D_R2L: //0X6
			/* Memory access control: MY = 0, MX = 0, MV = 1, ML = 0 	X-Y Exchange*/
			/* Display Function control: NN = 0, GS = 0, SS = 0, SM = 0	*/
			MemoryAccessReg_Data = 0x28;
			DisFunReg_Data = 0x02;
			break;
		case D2U_L2R: //0XA
			/* Memory access control: MY = 0, MX = 0, MV = 1, ML = 0 	X-Y Exchange*/
			/* Display Function control: NN = 0, GS = 1, SS = 1, SM = 0	*/
			MemoryAccessReg_Data = 0x28;
			DisFunReg_Data = 0x62;
			break;
		}

		//Get the screen scan direction
		sLCD_DIS.LCD_Scan_Dir = Scan_dir;

		//Get GRAM and LCD width and height
		//480*320,horizontal default
		if(Scan_dir == L2R_U2D || Scan_dir == R2L_D2U) {
			sLCD_DIS.LCD_Dis_Column	= LCD_3_5_HEIGHT ;
			sLCD_DIS.LCD_Dis_Page = LCD_3_5_WIDTH ;
		} else {
			sLCD_DIS.LCD_Dis_Column	= LCD_3_5_WIDTH ;
			sLCD_DIS.LCD_Dis_Page = LCD_3_5_HEIGHT ;
		}

		// Set the read / write scan direction of the frame memory
		LCD_WriteReg(0xB6);
		LCD_WriteData(0X00);
		LCD_WriteData(DisFunReg_Data);

		LCD_WriteReg(0x36);
		LCD_WriteData(MemoryAccessReg_Data);
	}
}

/********************************************************************************
function:	Set the display scan and color transfer modes
parameter:
		Scan_dir   :   Scan direction
		Colorchose :   RGB or GBR color format
********************************************************************************/
void BMP_SetGramScanWay(LCD_SCAN_DIR Scan_dir)
{
    uint16_t MemoryAccessReg_Data = 0; //addr:0x36
    uint16_t DisFunReg_Data = 0; //addr:0xB6

	if(LCD_2_8 == id){
		/*		it will support later		*/
		//Pico-ResTouch-LCD-2.8
		switch(Scan_dir){
			case L2R_U2D:
				/* Memory access control: MY = 0, MX = 1, MV = 0, ML = 0 RGB = 0 MH = 0 NN = 0 NN = 0*/
				MemoryAccessReg_Data = 0x40;
				break;	
			case D2U_L2R: 
				/* Memory access control: MY = 0, MX = 0, MV = 1, ML = 0 RGB = 0 MH = 0 NN = 0 NN = 0*/
				MemoryAccessReg_Data = 0x20;
				break;
			case R2L_D2U: 				
				/* Memory access control: MY = 1, MX = 0, MV = 0, ML = 1 RGB = 0 MH = 0 NN = 0 NN = 0*/
				MemoryAccessReg_Data = 0x80; //
				break;
			case U2D_R2L: 
				/* Memory access control: MY = 1, MX = 1, MV = 1, ML = 0 RGB = 0 MH = 0 NN = 0 NN = 0*/
				MemoryAccessReg_Data = 0xE0;				
				break;
			
		}
		sLCD_DIS.LCD_Scan_Dir = Scan_dir;
		//Get GRAM and LCD width and height
		//240*320,vertical default
		if(Scan_dir == L2R_U2D || Scan_dir == R2L_D2U) {
			sLCD_DIS.LCD_Dis_Column	= LCD_2_8_WIDTH ;
			sLCD_DIS.LCD_Dis_Page =  LCD_2_8_HEIGHT;
		} else {
			sLCD_DIS.LCD_Dis_Column	=  LCD_2_8_HEIGHT;
			sLCD_DIS.LCD_Dis_Page = LCD_2_8_WIDTH ;
		}

		
		LCD_WriteReg(0x36);
		LCD_WriteData(MemoryAccessReg_Data);
	}else{
		//Pico-ResTouch-LCD-3.5
		// Gets the scan direction of GRAM
		switch (Scan_dir) {
		case L2R_U2D:
			/* Memory access control: MY = 0, MX = 1, MV = 0, ML = 0 */
			/* Display Function control: NN = 0, GS = 0, SS = 1, SM = 0	*/
			MemoryAccessReg_Data = 0x48;
			DisFunReg_Data = 0x22;
			break;
		case R2L_D2U: 
			/* Memory access control: MY = 0, MX = 1, MV = 0, ML = 0 */
			/* Display Function control: NN = 0, GS = 1, SS = 0, SM = 0	*/
			MemoryAccessReg_Data = 0x48;
			DisFunReg_Data = 0x42;
			break;
		case U2D_R2L: 
			/* Memory access control: MY = 1, MX = 0, MV = 1, ML = 0 	X-Y Exchange*/
			/* Display Function control: NN = 0, GS = 0, SS = 0, SM = 0	*/
			MemoryAccessReg_Data = 0xA8;
			DisFunReg_Data = 0x02;
			break;
		case D2U_L2R: 
			/* Memory access control: MY = 1, MX = 0, MV = 1, ML = 0 	X-Y Exchange*/
			/* Display Function control: NN = 0, GS = 1, SS = 1, SM = 0	*/
			MemoryAccessReg_Data = 0xA8;
			DisFunReg_Data = 0x62;
			break;
		}

		//Get the screen scan direction
		sLCD_DIS.LCD_Scan_Dir = Scan_dir;

		//Get GRAM and LCD width and height
		//480*320,horizontal default
		if(Scan_dir == L2R_U2D || Scan_dir == R2L_D2U) {
			sLCD_DIS.LCD_Dis_Column	= LCD_3_5_HEIGHT ;
			sLCD_DIS.LCD_Dis_Page = LCD_3_5_WIDTH ;
		} else {
			sLCD_DIS.LCD_Dis_Column	= LCD_3_5_WIDTH ;
			sLCD_DIS.LCD_Dis_Page = LCD_3_5_HEIGHT ;
		}

		// Set the read / write scan direction of the frame memory
		LCD_WriteReg(0xB6);
		LCD_WriteData(0X00);
		LCD_WriteData(DisFunReg_Data);

		LCD_WriteReg(0x36);
		LCD_WriteData(MemoryAccessReg_Data);
	}
}

/********************************************************************************
function:
	initialization
********************************************************************************/
void LCD_Init(LCD_SCAN_DIR LCD_ScanDir, uint8_t LCD_BLval)
{
    
    LCD_Reset();//Hardware reset

    LCD_InitReg();//Set the initialization register
	
	if(LCD_BLval > 255)
		LCD_BLval = 255;
	LCD_SetBackLight(LCD_BLval);
	
	LCD_SetGramScanWay(LCD_ScanDir);//Set the display scan and color transfer modes
	Driver_Delay_ms(200);
}

/********************************************************************************
function:	Sets the start position and size of the display area
parameter:
	Xstart 	:   X direction Start coordinates
	Ystart  :   Y direction Start coordinates
	Xend    :   X direction end coordinates
	Yend    :   Y direction end coordinates
********************************************************************************/
void LCD_SetWindow(POINT Xstart, POINT Ystart,	POINT Xend, POINT Yend)
{	

	//set the X coordinates
	LCD_WriteReg(0x2A);
	LCD_WriteData(Xstart >> 8);	 		//Set the horizontal starting point to the high octet
	LCD_WriteData(Xstart & 0xff);	 	//Set the horizontal starting point to the low octet
	LCD_WriteData((Xend - 1) >> 8);		//Set the horizontal end to the high octet
	LCD_WriteData((Xend - 1) & 0xff);	//Set the horizontal end to the low octet

	//set the Y coordinates
	LCD_WriteReg(0x2B);
	LCD_WriteData(Ystart >> 8);
	LCD_WriteData(Ystart & 0xff );
	LCD_WriteData((Yend - 1) >> 8);
	LCD_WriteData((Yend - 1) & 0xff);

    LCD_WriteReg(0x2C);
}

/********************************************************************************
function:	Set the display point (Xpoint, Ypoint)
parameter:
	xStart :   X direction Start coordinates
	xEnd   :   X direction end coordinates
********************************************************************************/
void LCD_SetCursor(POINT Xpoint, POINT Ypoint)
{
	LCD_SetWindow(Xpoint, Ypoint, Xpoint, Ypoint);
}

/********************************************************************************
function:	Set show color
parameter:
		Color  :   Set show color,16-bit depth
********************************************************************************/
void LCD_SetColor(COLOR Color , POINT Xpoint, POINT Ypoint)
{
    LCD_Write_AllData(Color , (uint32_t)Xpoint * (uint32_t)Ypoint);
}

/********************************************************************************
function:	Point (Xpoint, Ypoint) Fill the color
parameter:
	Xpoint :   The x coordinate of the point
	Ypoint :   The y coordinate of the point
	Color  :   Set the color
********************************************************************************/
void LCD_SetPointlColor( POINT Xpoint, POINT Ypoint, COLOR Color)
{
    if ((Xpoint <= sLCD_DIS.LCD_Dis_Column) && (Ypoint <= sLCD_DIS.LCD_Dis_Page)) {
        LCD_SetCursor (Xpoint, Ypoint);
        LCD_SetColor(Color, 1, 1);
    }
}

/********************************************************************************
function:	Fill the area with the color
parameter:
	Xstart :   Start point x coordinate
	Ystart :   Start point y coordinate
	Xend   :   End point coordinates
	Yend   :   End point coordinates
	Color  :   Set the color
********************************************************************************/
void LCD_SetArealColor(POINT Xstart, POINT Ystart, POINT Xend, POINT Yend,	COLOR Color)
{
    if((Xend > Xstart) && (Yend > Ystart)) {
        LCD_SetWindow(Xstart , Ystart , Xend , Yend  );
        LCD_SetColor ( Color , Xend - Xstart, Yend - Ystart);
    }
}

/********************************************************************************
function:
			Clear screen
********************************************************************************/
void LCD_Clear(COLOR  Color)
{
    LCD_SetArealColor(0, 0, sLCD_DIS.LCD_Dis_Column , sLCD_DIS.LCD_Dis_Page , Color);
}

uint8_t LCD_Read_Id(void)
{
	uint8_t reg = 0xDC;
	uint8_t tx_val = 0x00;
	uint8_t rx_val;
    DEV_Digital_Write(LCD_CS_PIN, 0);
    DEV_Digital_Write(LCD_DC_PIN, 0);
	SPI4W_Write_Byte(reg);
	spi_write_read_blocking(spi1,&tx_val,&rx_val,1);
    DEV_Digital_Write(LCD_CS_PIN, 1);
	return rx_val;
}