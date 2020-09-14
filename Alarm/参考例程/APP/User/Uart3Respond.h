#ifndef _UART3RESPOND_H_
#define _UART3RESPOND_H_

extern STR_COMM  Uart3Data;
extern STR_COMM  Uart3TempBuffer;

extern void Uart3Respond(STR_COMM *buffer_in, STR_COMM *buffer_out);

#endif
