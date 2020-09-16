#ifndef _RECOEDER_H_
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

//�ػ���־λ
#define FACTORY_CFG_POWER_ADDR      (FACTORY_CFG_ADJUST_ADDR + FACTORY_CFG_ADJUST_SIZE + 2 )
#define FACTORY_CFG_POWER_SIZE      2

extern void ReadFactoryConfigParam(void);
extern ErrorStatus WriteFactoryConfigParam(void);
extern ErrorStatus SetStayInBoot(void);
extern void SoftReset(void);
							   
#endif
