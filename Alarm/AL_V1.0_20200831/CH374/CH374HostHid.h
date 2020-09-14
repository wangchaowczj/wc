#ifndef _CH374HOSTMOUSE_H_
#define _CH374HOSTMOUSE_H_
typedef enum 
{ 
	SCAN_KEYBOARD_IDLE,
	SCAN_KEYBOARD_DOING,
	SCAN_KEYBOARD_END
}SCAN_KEYBOARD_STAUTS;

#define SCAN_KEYBOARD_DATA_LEN  129
typedef struct
{
	u16 Bytes;
	SCAN_KEYBOARD_STAUTS Status;
	u8 Data[SCAN_KEYBOARD_DATA_LEN];
}SCAN_KEYBOARD;

extern SCAN_KEYBOARD ScanKeyBoard;
extern STR_COMM  HidData ;

extern void OpenScanKeyBoard(void);
extern void CloseScanKeyBoard(void);
extern u8 ScanKeyBoardIsClose(void);
extern void ClearScanKeyBoard(void);
extern u8  CH374HostMouseProcess(u8* MouseTog);
extern u8 CH374HostScanKeyBoardProcess(u8* MouseTog);

#endif
