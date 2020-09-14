#ifndef _RECORDER_H_
#define _RECORDER_H_

#define BOOT_CFG_START_ADDR                0x0803F000UL
#define BOOT_CFG_END_ADDR                  0x0803F7FFUL

#define FACTORY_CFG_START_ADDR             0x0803F800UL
#define FACTORY_CFG_END_ADDR               0x0803FFFFUL

//设备ID
#define FACTORY_CFG_DEV_ID_ADDR            (FACTORY_CFG_START_ADDR+0)
#define FACTORY_CFG_DEV_ID_SIZE            32//大小必需是2的倍数,不含CRC16

//硬件版本
#define FACTORY_CFG_HW_VERSION_ADDR        (FACTORY_CFG_DEV_ID_ADDR + FACTORY_CFG_DEV_ID_SIZE + 2)
#define FACTORY_CFG_HW_VERSION_SIZE        32//大小必需是2的倍数,不含CRC16

//校准参数
#define FACTORY_CFG_ADJUST_ADDR    (FACTORY_CFG_HW_VERSION_ADDR + FACTORY_CFG_HW_VERSION_SIZE + 2)
#define FACTORY_CFG_ADJUST_SIZE    100//大小必需是2的倍数,不含CRC16,参数K占2字节，B2个字节，校准时间6字节(年月日时分秒)

//测试参数
#define FACTORY_CFG_TEST_PARAM_ADDR        (FACTORY_CFG_ADJUST_ADDR + FACTORY_CFG_ADJUST_SIZE + 2)
#define FACTORY_CFG_TEST_PARAM_SIZE        100//大小必需是2的倍数,不含CRC16

typedef enum
{
	NEW_STR_INFO = 0,
	STR_INFO,
	HEX_ARRAY,
	U16_DATA,
	DEV_START,
	H32_ARRAY,
	HEX_ARRAY_NO_TITLE,
    LOG_DET_INFO
}DATE_WRITE_TYPE;
#define LOG_PATH    "0:LOG"
#define LOG_FILE_MAX    104857600UL  //100M

extern void ReadFactoryConfigParam(void);
extern ErrorStatus WriteFactoryConfigParam(void);
extern ErrorStatus SetStayInBoot(void);

extern u8 LogInit(void);
extern u32 GetLogFileSize(u8 Index);
extern ErrorStatus DeleteLogFile(u8 Index);
extern ErrorStatus WriteLog(void* data, u16 len, DATE_WRITE_TYPE op_type);

#endif
