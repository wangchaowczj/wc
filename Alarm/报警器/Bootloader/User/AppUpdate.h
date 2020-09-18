#ifndef _APPUPDATE_H_
#define _APPUPDATE_H_

#define FLASH_PAGE_SIZE     2048   //大容量型号，每页FLASH为2K


#define APP_START_ADDRESS        0x0800C800    //应用程序起始地址
#define APP_END_ADDRESS			 0x0803EFFF	   //应用程序截止地址,芯片内置256K容量，最后的2扇区用于保存数据
#define APP_ADDRESS_SIZE       	 (APP_END_ADDRESS - APP_START_ADDRESS + 1)

#define BOOT_FLAG_START_ADDRESS          0x0803F000     //倒数第二扇区用于设置切换bootloader
#define BOOT_FLAG_ADDRESS_SIZE   2048   

typedef  void (*pFunction)(void);

extern void BootToUserApp(void);
extern u8 EraseUserAppUpdate(u32 start_address, u32 length);
extern ErrorStatus UserAppUpdate(u32 start_address, u8* ptr, u32 length);

#endif
