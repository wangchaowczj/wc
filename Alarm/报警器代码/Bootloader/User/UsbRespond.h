#ifndef _USBRESPOND_H_
#define _USBRESPOND_H_

extern STR_COMM  GusetSendBuffer;
extern STR_COMM  GusetTempBuffer;

extern void UsbRespond(STR_COMM *buffer_in, STR_COMM *buffer_out);

#endif 

