#ifndef _RECORDER_H_
#define _RECORDER_H_

#define BOOT_CFG_START_ADDR                0x0803F000UL
#define BOOT_CFG_END_ADDR                  0x0803F7FFUL

#define FACTORY_CFG_START_ADDR             0x0803F800UL
#define FACTORY_CFG_END_ADDR               0x0803FFFFUL

//�豸ID
#define FACTORY_CFG_DEV_ID_ADDR            (FACTORY_CFG_START_ADDR+0)
#define FACTORY_CFG_DEV_ID_SIZE            32//��С������2�ı���,����CRC16

//Ӳ���汾
#define FACTORY_CFG_HW_VERSION_ADDR        (FACTORY_CFG_DEV_ID_ADDR + FACTORY_CFG_DEV_ID_SIZE + 2)
#define FACTORY_CFG_HW_VERSION_SIZE        32//��С������2�ı���,����CRC16

//У׼����
#define FACTORY_CFG_ADJUST_ADDR    (FACTORY_CFG_HW_VERSION_ADDR + FACTORY_CFG_HW_VERSION_SIZE + 2)
#define FACTORY_CFG_ADJUST_SIZE    100//��С������2�ı���,����CRC16,����Kռ2�ֽڣ�B2���ֽڣ�У׼ʱ��6�ֽ�(������ʱ����)

//���Բ���
#define FACTORY_CFG_TEST_PARAM_ADDR        (FACTORY_CFG_ADJUST_ADDR + FACTORY_CFG_ADJUST_SIZE + 2)
#define FACTORY_CFG_TEST_PARAM_SIZE        100//��С������2�ı���,����CRC16

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
