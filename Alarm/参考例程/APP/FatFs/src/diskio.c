/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2013        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/

//#include "diskio.h"		/* FatFs lower layer API */
//#include "usbdisk.h"	/* Example: USB drive control */
//#include "atadrive.h"	/* Example: ATA drive control */
//#include "sdcard.h"		/* Example: MMC/SDC contorl */
#include "includes.h"

/* Definitions of physical drive number for each media */
//#define ATA		0
//#define MMC		1
//#define USB		2

static DSTATUS SdState = STA_NOINIT;

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber (0..) */
)
{
	DSTATUS stat = STA_NOINIT;

	switch (pdrv) 
	{
		case 0 :
			if(SdCardPowerOn() != ERROR)
			{
				stat &= ~STA_NOINIT;
			}
			SdState = stat;
			break;
		default:
			break;		
	}
	return stat;
}



/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber (0..) */
)
{
	switch (pdrv) 
	{
		case 0 :
			return SdState;
		default:
			break;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	UINT count		/* Number of sectors to read (1..128) */
)
{
	switch (pdrv) 
	{
		case 0 :
			if(SdCardReadBlock(buff, sector, count) != ERROR)
			{
				return RES_OK;
			}
			break;
		default:
			break;
	}
	return RES_ERROR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	UINT count			/* Number of sectors to write (1..128) */
)
{
	switch (pdrv) 
	{
		case 0 :
			if(SdCardWriteBlock(buff, sector, count) != ERROR)
			{
				return RES_OK;
			}
			break;			
		default:
			break;
	}
	return RES_ERROR;
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	switch (pdrv) 
	{
		case 0 :
			if(SdCardIoCtrl(cmd, buff) != ERROR)
			{
				return RES_OK;
			}
			break;			
		default:
			break;
	}
	return RES_PARERR;
}
#endif

/* User defined function to give a current time to fatfs module          */
/* 31-25: Year(0-127 org.1980), 24-21: Month(1-12), 20-16: Day(1-31) */                                                                                                                                                                                                                                          
/* 15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2) */ 
DWORD get_fattime (void)
{
//    TIME time;
//    DWORD date;
//    RTC_GetTime(&time);
//	if(time.year>1980)//年份改为1980年起,0-11月改为1-12月,将秒数改为0-29
//	{
//		time.year -= 1980;
//		time.sec /=2;
//		date = (time.year << 25)|(time.month<<21)|(time.day<<16)|\
//				(time.hour<<11)|(time.min<<5)|(time.sec);

//		return date;		
//	}
//	else
	{
		return 0;
	}


}
