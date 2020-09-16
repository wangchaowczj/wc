#ifndef _RECOEDER_H_
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

//关机标志位
#define FACTORY_CFG_POWER_ADDR      (FACTORY_CFG_ADJUST_ADDR + FACTORY_CFG_ADJUST_SIZE + 2 )
#define FACTORY_CFG_POWER_SIZE      2

extern void ReadFactoryConfigParam(void);
extern ErrorStatus WriteFactoryConfigParam(void);
extern ErrorStatus SetStayInBoot(void);
extern void SoftReset(void);
							   
#endif
