#include "includes.h"
STR_COMM HidData;


// 设备描述符
const u8 CustomHID_DeviceDescriptor[18] = { 
	0x12, /*描述符长度*/
	0x01, /*描述符类型*/
	0x10, 0x01,/*USB规范版本号*/
	0x00, /*类代码*/
	0x00, /*子类代码*/
	0x00, /*协议代码*/
	0x08, /*端点0支持的最大数据包长度*/
	0x00, 0x90,            /*VID*/
	0x0E, 0x00,            /* PID*/
	0x00, 0x01,            /*设备版本号*/
	0x01, /*供应商字符串描述符索引*/
	0x02,/*产品字符串描述符索引*/
	0x00,/*设备序列号字符串描述符索引*/
	0x01 /*所支持的配置字符串描述符索引*/
};	
 //配置描述符
const u8 CustomHID_ConfigDescriptor[41] = { 
	/*配置描述符*/
    0x09, /* 用于表示配置描述符的长度，固定为9个字节，即0x09 */
    0X02, /* 用于表示配置描述符的类型值，固定为0x02 */
    41,   /*2字节用于表示配置信息的总长度，包括配置描述符、接口描述符、端点描述符长度的总和*/
    0x00,
	
    0x01,         /* 用于表示配置所支持的接口数。一般来说，USB设备的接口至少有一个，因此其最小值为1 */
    0x01,         /* 用于表示USB设备的配置值 */
    0x00,         /* 用于指出配置字符串描述符的索引值。具体字符串的内容在后面字符串描述符中定义。如果没有配置字符串，可以置为0*/
    0x80,         /* 用于表示USB设备特性。bmAttributes是按位寻址的，第6位置1表示使用总线电源；第5位置1表示支持远程唤醒功能；该字段其他位均保留，一般来说，第0～4位置0即可，第7位置1即可 */
    0x32,         /* 用于表示USB设备运行时所需要消耗的总线电流，单位以2mA为基准。USB设备可以从USB总线上获得最大的电流为500mA，因此bMaxPower字段的最大值可以设置为250 */

    /************** 接口描述符 Descriptor of Custom HID interface ****************/
    /* 09 */
    0x09,         /* bLength: Interface Descriptor size */
    0X04,/* bDescriptorType: Interface descriptor type */
    0x00,         /* 接口号 */
    0x00,         /* 可替换设置值 */
    0x02,         /* 端点0以外的端点数 */
    0x03,         /* 类代码: HID */
    0x00,         /* 子类代码 : 1=BOOT, 0=no boot */
    0x00,         /* 协议代码 : 0=none, 1=keyboard, 2=mouse */
    0,            /* 字符串描述符的索引值 */
    /******************** Descriptor of Custom HID HID ********************/
    /* 18 */
    0x09,         /* HID描述符长度 */
    USB_HID_DESCR_TYPE, /* HID描述符类型，值为0x21 */
    0x10,         /* HID设备所遵循的HID版本号，为4位16进制的BCD码数据。1.0即0x0100，1.1即0x0101，2.0即0x0200。*/
    0x01,
    0x00,         /* HID设备国家/地区代码 */
    0x01,         /* HID设备支持的其他设备描述符的数量。由于HID设备至少需要包括一个报告描述符，故其值至小为0x01 */
    USB_REPORT_DESCR_TYPE,         /* HID描述符附属的类别描述符长度 bDescriptorType ,下级描述符的类型:报告描述符编号为0x22*/
    38,          /* 可选字段，用于表示HID描述符附属的类别描述符类型及长度 */
    0x00,
    /********************端点描述符  Descriptor of Custom HID endpoints ******************/
    /* 27 */
    0x07,          /*用于表示端点描述符的长度，固定为7字节，即0x07 */
    0x05, /* 用于表示接口描述符的类型值，固定为0x05 */

    0x82,          /* 用于表示端点的端点号以及端点的数据传输方向。第七位表示端点的数据传输方向，0表示OUT数据传输，1表示IN数据传输；第0～位表示端点号，例如001B表示端点1、010B表示端点2；其余位均保留，必须置0。 */
    0x03,          /* 用于表示端点的特性。其中第0位和第1位表示端点的数据传输类型，00B表示控制传输、01B表示同步传输、10B表示块传 输、11B表示中断传输；如果是同步传输，第2位和第3位表示同步类型，00B表示非同步、01B表示异步、10B表示自适应、11B表示同步；第4、5 位表示端点的用法类型，00B表示数据端点、01B表示显示反馈端点、10B表示隐匿反馈端点、11B保留。其余位保留。 */
    0x40,          /* 用于表示端点所支持最大数据包的长度。其中第0～10位表示数据包的长度，第11位和12位指出每小帧最多传输的事务数，其余位均保留，必须置0。*/
    0x00,
    0x18,          /* 用于指定端点数据传输的访问间隔。低速中断端点，取值范围为10～255，对应的访问间隔为10～255ms；对于全速中断端点，取值范围为1～255，对应的访问间隔为1～255ms；对于其他端点，可以参阅USB相关协议。 */
    /* 34 */
    	
    0x07,	/* bLength: Endpoint Descriptor size */
    0X05,	/* bDescriptorType: */
			/*	Endpoint descriptor type */
    0x02,	/* bEndpointAddress: */
			/*	Endpoint Address (OUT) */
    0x03,	/* bmAttributes: Interrupt endpoint */
    0x40,	/* wMaxPacketSize: 64 Bytes max  */
    0x00,
    0x01,	/* bInterval: Polling Interval (1 ms) */
    /* 41 */
};
/*报告描述符*/
const uint8_t CustomHID_ReportDescriptor[38] =
{                    	  
	0x06,0x00,0xFF,             //USAGE_PAGE (Vendor Defined Page 1)       
    0x09,0x01,              //USAGE (Vendor Usage 1)       
    0xA1,0x01,              //COLLECTION (Application)            
    0x19,0x01,              //(Vendor Usage 1)       
    0x29,0x40,              //(Vendor Usage 1)       
    0x15,0x00,              //LOGICAL_MINIMUM (0)       
    0x26,0xFF,0x00,             //LOGICAL_MAXIMUM (255)       
    0x75,0x08,              //REPORT_SIZE (8)       
    0x95,0x40,              //REPORT_COUNT (64)       
    0x81,0x02,              //INPUT (Data,Var,Abs)           
    0x19,0x01,              //(Vendor Usage 1)       
    0x29,0x40,              //(Vendor Usage 1) 
    0x15,0x00,              //LOGICAL_MINIMUM (0)       
    0x26,0xFF,0x00,             //LOGICAL_MAXIMUM (255)       
    0x75,0x08,              //REPORT_SIZE (8)       
    0x95,0x40,              //REPORT_COUNT (64)	
    0x91,0x02,              //OUTPUT (Data,Var,Abs)            
    0xC0                    // END_COLLECTION
};


volatile u8 DeviceHidSendFinish;

ErrorStatus DeviceHidSendData(u8* Data, u16 Bytes)
{
	u8 StartIndex=0,Count = 0;
	u16 TimeCount;
	
	while(1)
	{
		DeviceHidSendFinish = 0;
		if(StartIndex >= Bytes)
		{
			return SUCCESS;
		}
		if ((StartIndex+64) <= Bytes)
		{
			Count = 64;
		}
		else
		{
			Count = Bytes%64;
			CH374WriteBlockZero(RAM_ENDP2_TRAN, 64);
		}
		Write374Block( RAM_ENDP2_TRAN, Count, Data+StartIndex);
		Write374Byte( REG_USB_LENGTH, 64);
		Write374Byte( REG_USB_ENDP2, M_SET_EP2_TRAN_ACK( Read374Byte( REG_USB_ENDP2 ) ) ^ BIT_EP2_RECV_TOG );	
		
		StartIndex += Count;
		
		TimeCount = 0;
        while(DeviceHidSendFinish == 1)
        {
            return SUCCESS;
        }
		while(DeviceHidSendFinish == 0)
		{
			TimeCount++;
			if(TimeCount > 30000)
			{
				return ERROR;
			}
		}
	}	
}

void DeviceHidOutCallBack(void)
{
	u8 length;
	
	length = Read374Byte( REG_USB_LENGTH );
	if((HidData.bytenum + length) > BUF_MAX)
	{
		HidData.bytenum = 0;
	}
	Read374Block( RAM_ENDP2_RECV, length, &HidData.uch_buf[HidData.bytenum] );
	HidData.bytenum += length;
	if(SlipSimpleCheck(&HidData) == SUCCESS)
	{
		if(OS_ERR_NONE != OSSemPost(MsgSem.usb_rcv))
		{
			HidData.bytenum = 0;
			HidData.status = RECIEVING;
		}	
	}

		
//	//回环测试
//	CH374WriteBlock( RAM_ENDP2_TRAN, length, &HidData.uch_buf[HidData.bytenum-length]);
//	Write374Byte( REG_USB_LENGTH, length);
//	Write374Byte( REG_USB_ENDP2, M_SET_EP2_TRAN_ACK( Read374Byte( REG_USB_ENDP2 ) ) ^ BIT_EP2_RECV_TOG );	
}

void DeviceHidSendFinishCallBack(void)
{
	DeviceHidSendFinish = 1;
}

void CH374InterruptCallBack(void)
{
	while(GET_CH374_INT_PIN() == 0)
	{
		CH374DeviceInterrupt();
	}
}

