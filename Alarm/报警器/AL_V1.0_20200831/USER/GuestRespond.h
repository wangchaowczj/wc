#ifndef _GUESTRESPOND_H_
#define _GUESTRESPOND_H_

extern STR_COMM  GusetSendBuffer;
extern STR_COMM  GusetTempBuffer;

extern void GetGeneralRespondData(STR_COMM* buffer, u16 Cmd, u16 PackIndex);
extern u16 GetBufferStringLenght(u8* Buffer, u16 MaxLenght);
extern u32 GetDecValueFromBuffer(u8* Buffer, u16 MaxLenght);
extern ErrorStatus GetDataFromSeparator(u8 Separator, u8 SeparatorNumber, u8* Buffer, u16 Bytes, u16* StartIndex, u16* DataBytes);
extern ErrorStatus GetDecValueFromSeparator(u8 Separator, u8 SeparatorNumber, u8* Buffer, u16 Bytes, u32* Value);

extern void  GuestRespond(STR_COMM *buffer_in, STR_COMM *buffer_out);

#endif

