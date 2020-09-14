#ifndef _LCD_H_
#define _LCD_H_

extern u8 LcdWriteData(u8 Data);
extern u8 LcdWriteDataBytes(u8* Data, u16 bytes);
extern void LcdSetPos(unsigned char x, unsigned char y) ;
extern ErrorStatus LcdInit(void);

#endif

