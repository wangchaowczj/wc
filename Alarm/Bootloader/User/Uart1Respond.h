#ifndef _UART1RESPOND_H_
#define _UART1RESPOND_H_

extern STR_COMM  Uart1Data;
extern STR_COMM  Uart1TempBuffer;

extern void Uart1Respond(STR_COMM *buffer_in, STR_COMM *buffer_out);

#endif

