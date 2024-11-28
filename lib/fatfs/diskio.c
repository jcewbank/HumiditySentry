/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2007        */
/*-----------------------------------------------------------------------*/
/* This is a stub disk I/O module that acts as front end of the existing */
/* disk I/O modules and attach it to FatFs module with common interface. */
/*-----------------------------------------------------------------------*/
#include "DEV_Config.h"
#include "MMC_SD.h"	
#include "ff.h"
#include "diskio.h"

/*-----------------------------------------------------------------------*/
/* Initialize a Drive                                                    */

#define SD_CARD	 0  // SD card, labeled as 0

#define FLASH_SECTOR_SIZE 	512			  

// Initialize the disk
DSTATUS disk_initialize (
	BYTE drv				/* Physical drive number (0..) */
)
{	
	uint8_t res = 0;	    
	switch(drv)
	{
		case SD_CARD: // SD card
			res = SD_Initialize(); // Initialize SD card
		 	if(res) // Bug in STM32 SPI: if the SD card operation fails, not executing the following code might cause SPI read/write issues
			{
				SD_SPI_SpeedLow();
				SD_SPI_ReadWriteByte(0xff); // Provide an additional 8 clock cycles
				SD_SPI_SpeedHigh();
			}
  			break;
		default:
			res = 1; 
	}		 
	if(res) return STA_NOINIT; // Return "not initialized" status if there is an error
	else return 0; // Initialization successful
}   

// Get disk status
DSTATUS disk_status (
	BYTE drv		/* Physical drive number (0..) */
)
{		   
    return 0;
}

// Read sector
// drv: Disk number 0~9
// *buff: Address of data reception buffer
// sector: Sector address
// count: Number of sectors to read
DRESULT disk_read (
	BYTE drv,		/* Physical drive number (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	BYTE count		/* Number of sectors to read (1..255) */
)
{
	uint8_t res = 0; 
    if (!count) return RES_PARERR; // Count cannot be 0; otherwise, return parameter error		 	 
	switch(drv)
	{
		case SD_CARD: // SD card
			res = SD_ReadDisk(buff, sector, count);	 
		 	if(res) // Bug in STM32 SPI: if the SD card operation fails, not executing the following code might cause SPI read/write issues
			{
				SD_SPI_SpeedLow();
				SD_SPI_ReadWriteByte(0xff); // Provide an additional 8 clock cycles
				SD_SPI_SpeedHigh();
			}
			break;
		default:
			res = 1; 
	}
    // Process return value, converting SPI_SD_driver.c's return value to ff.c's return value
    if(res == 0x00) return RES_OK;	 
    else return RES_ERROR;	   
}  

// Write sector
// drv: Disk number 0~9
// *buff: Address of the data to send
// sector: Sector address
// count: Number of sectors to write	    
#if _READONLY == 0
DRESULT disk_write (
	BYTE drv,			/* Physical drive number (0..) */
	const BYTE *buff,	        /* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	BYTE count			/* Number of sectors to write (1..255) */
)
{
	uint8_t res = 0;  
    if (!count) return RES_PARERR; // Count cannot be 0; otherwise, return parameter error		 	 
	switch(drv)
	{
		case SD_CARD: // SD card
			res = SD_WriteDisk((uint8_t*)buff, sector, count);
			break;
		default:
			res = 1; 
	}
    // Process return value, converting SPI_SD_driver.c's return value to ff.c's return value
    if(res == 0x00) return RES_OK;	 
    else return RES_ERROR;		 
}
#endif /* _READONLY */

// Obtain other parameters
// drv: Disk number 0~9
// ctrl: Control code
// *buff: Pointer to send/receive buffer
DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive number (0..) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{	
	DRESULT res;						  			     
	if(drv == SD_CARD) // SD card
	{
	    switch(ctrl)
	    {
		    case CTRL_SYNC:
				DEV_Digital_Write(SD_CS_PIN, 0);
		        if(SD_WaitReady() == 0) res = RES_OK; 
		        else res = RES_ERROR;	  
				DEV_Digital_Write(SD_CS_PIN, 1);
		        break;	 
		    case GET_SECTOR_SIZE:
		        *(WORD*)buff = 512; // Sector size is 512 bytes
		        res = RES_OK;
		        break;	 
		    case GET_BLOCK_SIZE:
		        *(WORD*)buff = 8; // Block size is 8 sectors
		        res = RES_OK;
		        break;	 
		    case GET_SECTOR_COUNT:
		        *(DWORD*)buff = SD_GetSectorCount(); // Get total sector count
		        res = RES_OK;
		        break;
		    default:
		        res = RES_PARERR;
		        break;
	    }
	}
    else res = RES_ERROR; // Other drives are not supported
    return res;
} 

/*-----------------------------------------------------------------------*/
/* User-defined function to provide the current time to the FatFs module*/
/* 31-25: Year (0-127 origin:1980), 24-21: Month (1-12), 20-16: Day (1-31) */
/* 15-11: Hour (0-23), 10-5: Minute (0-59), 4-0: Second (0-29 * 2) */                                                                                                                                                                                                                                                

DWORD get_fattime (void)
{
    return 0;
}
