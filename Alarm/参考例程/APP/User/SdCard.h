/**
  *************************************************************************************
  * @file    SdCard.h
  * @author  ZH
  * @version V1.0.0
  * @date    2014年9月10日
  * @brief   SD卡模块头文件
  *************************************************************************************
  */
#ifndef _SDCARD_H_
#define _SDCARD_H_

/**
 *@ingroup SdCard
 *@{
 */
/**
 *@defgroup SdCardType
 *@{
 */
/* MMC card type flags (MMC_GET_TYPE) */
#define SDCARD_TYPE_MMC		0x01		/**< MMC ver 3 */
#define SDCARD_TYPE_SD1		0x02		/**< SD ver 1 */
#define SDCARD_TYPE_SD2		0x04		/**< SD ver 2 */
#define SDCARD_TYPE_SDC		(SDCARD_TYPE_SD1 | SDCARD_TYPE_SD2)	/**< SD */
#define SDCARD_TYPE_BLOCK	0x08		/**< Block addressing */

/**
 *@} SdCardType
 */
 
/**
 *@defgroup SdCardCommand
 *@{
 */
/* Definitions for MMC/SDC command */
#define SDCARD_CMD0	    (0x40+0)	/**< GO_IDLE_STATE */
#define SDCARD_CMD1	    (0x40+1)	/**< SEND_OP_COND (MMC) */
#define SDCARD_ACMD41	(0xC0+41)	/**< SEND_OP_COND (SDC) */
#define SDCARD_CMD8	    (0x40+8)	/**< SEND_IF_COND */
#define SDCARD_CMD9	    (0x40+9)	/**< SEND_CSD */
#define SDCARD_CMD10	(0x40+10)	/**< SEND_CID */
#define SDCARD_CMD12	(0x40+12)	/**< STOP_TRANSMISSION */
#define SDCARD_ACMD13	(0xC0+13)	/**< SD_STATUS (SDC) */
#define SDCARD_CMD16	(0x40+16)	/**< SET_BLOCKLEN */
#define SDCARD_CMD17	(0x40+17)	/**< READ_SINGLE_BLOCK */
#define SDCARD_CMD18	(0x40+18)	/**< READ_MULTIPLE_BLOCK */
#define SDCARD_CMD23	(0x40+23)	/**< SET_BLOCK_COUNT (MMC) */
#define SDCARD_ACMD23	(0xC0+23)	/**< SET_WR_BLK_ERASE_COUNT (SDC) */
#define SDCARD_CMD24	(0x40+24)	/**< WRITE_BLOCK */
#define SDCARD_CMD25	(0x40+25)	/**< WRITE_MULTIPLE_BLOCK */
#define SDCARD_CMD55	(0x40+55)	/**< APP_CMD */
#define SDCARD_CMD58	(0x40+58)	/**< READ_OCR */
#define SDCARD_CMD59	(0x40+59)	/**< CRC校验开关 */

#define SDCARD_CMD17_TOKEN       0xFE       /**< CMD17/18/24指令令牌*/
#define SDCARD_CMD25_TOKEN       0xFC       /**< CMD25指令令牌*/
#define SDCARD_CMD25_ST_TOKEN    0xFD       /**< CMD25指令令牌，停止传输*/

#define SDCARD_DATA_RESP_ACCEPTED       0x05                  /**< 数据写入成功*/
#define SDCARD_DATA_RESP_CRCERR         0x0B                  /**< CRC校验错误*/
#define SDCARD_DATA_RESP_WRERR          0x0D                  /**< 数据写错误*/

#define SDCARD_R1_PARAERR_BIT           0x40                  /**< R1参数错误状态*/
#define SDCARD_R1_ADDRERR_BIT           0x20                  /**< R1地址错误*/
#define SDCARD_R1_ESERR_BIT             0x10                  /**< R1擦除序列错误*/
#define SDCARD_R1_CMDCRCERR_BIT         0x08                  /**< R1指令CRC校验错误*/
#define SDCARD_R1_ILLCMD_BIT            0x04                  /**< R1非法指令*/
#define SDCARD_R1_ER_BIT                0x02                  /**< R1擦除复位*/
#define SDCARD_R1_IDLE_BIT              0x01                  /**< R1空闲*/
#define SDCARD_R1_NOERR                 0x00                  /**< R1无错误*/

/**
 *@} SdCardCommand
 */
/**
 *@} SdCard
 */

extern ErrorStatus SdCardSendDataBlock(const uint8_t *buff, uint8_t token);
extern ErrorStatus SdCardIoCtrl(uint8_t cmd, uint8_t *buff);
extern ErrorStatus SdCardReadBlock(uint8_t *buff, uint32_t sector, uint32_t count);
extern ErrorStatus SdCardWriteBlock(const uint8_t *buff, uint32_t sector, uint32_t count);
extern ErrorStatus SdCardPowerOn(void);
#endif
