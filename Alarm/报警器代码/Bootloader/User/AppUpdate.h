#ifndef _APPUPDATE_H_
#define _APPUPDATE_H_

#define FLASH_PAGE_SIZE     2048   //�������ͺţ�ÿҳFLASHΪ2K


#define APP_START_ADDRESS        0x0800C800    //Ӧ�ó�����ʼ��ַ
#define APP_END_ADDRESS			 0x0803EFFF	   //Ӧ�ó����ֹ��ַ,оƬ����256K����������2�������ڱ�������
#define APP_ADDRESS_SIZE       	 (APP_END_ADDRESS - APP_START_ADDRESS + 1)

#define BOOT_FLAG_START_ADDRESS          0x0803F000     //�����ڶ��������������л�bootloader
#define BOOT_FLAG_ADDRESS_SIZE   2048   

typedef  void (*pFunction)(void);

extern void BootToUserApp(void);
extern u8 EraseUserAppUpdate(u32 start_address, u32 length);
extern ErrorStatus UserAppUpdate(u32 start_address, u8* ptr, u32 length);

#endif
