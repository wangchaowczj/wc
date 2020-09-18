#include "includes.h"
STR_COMM HidData;


// �豸������
const u8 CustomHID_DeviceDescriptor[18] = { 
	0x12, /*����������*/
	0x01, /*����������*/
	0x10, 0x01,/*USB�淶�汾��*/
	0x00, /*�����*/
	0x00, /*�������*/
	0x00, /*Э�����*/
	0x08, /*�˵�0֧�ֵ�������ݰ�����*/
	0x00, 0x90,            /*VID*/
	0x0E, 0x00,            /* PID*/
	0x00, 0x01,            /*�豸�汾��*/
	0x01, /*��Ӧ���ַ�������������*/
	0x02,/*��Ʒ�ַ�������������*/
	0x00,/*�豸���к��ַ�������������*/
	0x01 /*��֧�ֵ������ַ�������������*/
};	
 //����������
const u8 CustomHID_ConfigDescriptor[41] = { 
	/*����������*/
    0x09, /* ���ڱ�ʾ�����������ĳ��ȣ��̶�Ϊ9���ֽڣ���0x09 */
    0X02, /* ���ڱ�ʾ����������������ֵ���̶�Ϊ0x02 */
    41,   /*2�ֽ����ڱ�ʾ������Ϣ���ܳ��ȣ������������������ӿ����������˵����������ȵ��ܺ�*/
    0x00,
	
    0x01,         /* ���ڱ�ʾ������֧�ֵĽӿ�����һ����˵��USB�豸�Ľӿ�������һ�����������СֵΪ1 */
    0x01,         /* ���ڱ�ʾUSB�豸������ֵ */
    0x00,         /* ����ָ�������ַ���������������ֵ�������ַ����������ں����ַ����������ж��塣���û�������ַ�����������Ϊ0*/
    0x80,         /* ���ڱ�ʾUSB�豸���ԡ�bmAttributes�ǰ�λѰַ�ģ���6λ��1��ʾʹ�����ߵ�Դ����5λ��1��ʾ֧��Զ�̻��ѹ��ܣ����ֶ�����λ��������һ����˵����0��4λ��0���ɣ���7λ��1���� */
    0x32,         /* ���ڱ�ʾUSB�豸����ʱ����Ҫ���ĵ����ߵ�������λ��2mAΪ��׼��USB�豸���Դ�USB�����ϻ�����ĵ���Ϊ500mA�����bMaxPower�ֶε����ֵ��������Ϊ250 */

    /************** �ӿ������� Descriptor of Custom HID interface ****************/
    /* 09 */
    0x09,         /* bLength: Interface Descriptor size */
    0X04,/* bDescriptorType: Interface descriptor type */
    0x00,         /* �ӿں� */
    0x00,         /* ���滻����ֵ */
    0x02,         /* �˵�0����Ķ˵��� */
    0x03,         /* �����: HID */
    0x00,         /* ������� : 1=BOOT, 0=no boot */
    0x00,         /* Э����� : 0=none, 1=keyboard, 2=mouse */
    0,            /* �ַ���������������ֵ */
    /******************** Descriptor of Custom HID HID ********************/
    /* 18 */
    0x09,         /* HID���������� */
    USB_HID_DESCR_TYPE, /* HID���������ͣ�ֵΪ0x21 */
    0x10,         /* HID�豸����ѭ��HID�汾�ţ�Ϊ4λ16���Ƶ�BCD�����ݡ�1.0��0x0100��1.1��0x0101��2.0��0x0200��*/
    0x01,
    0x00,         /* HID�豸����/�������� */
    0x01,         /* HID�豸֧�ֵ������豸������������������HID�豸������Ҫ����һ������������������ֵ��СΪ0x01 */
    USB_REPORT_DESCR_TYPE,         /* HID������������������������� bDescriptorType ,�¼�������������:�������������Ϊ0x22*/
    38,          /* ��ѡ�ֶΣ����ڱ�ʾHID������������������������ͼ����� */
    0x00,
    /********************�˵�������  Descriptor of Custom HID endpoints ******************/
    /* 27 */
    0x07,          /*���ڱ�ʾ�˵��������ĳ��ȣ��̶�Ϊ7�ֽڣ���0x07 */
    0x05, /* ���ڱ�ʾ�ӿ�������������ֵ���̶�Ϊ0x05 */

    0x82,          /* ���ڱ�ʾ�˵�Ķ˵���Լ��˵�����ݴ��䷽�򡣵���λ��ʾ�˵�����ݴ��䷽��0��ʾOUT���ݴ��䣬1��ʾIN���ݴ��䣻��0��λ��ʾ�˵�ţ�����001B��ʾ�˵�1��010B��ʾ�˵�2������λ��������������0�� */
    0x03,          /* ���ڱ�ʾ�˵�����ԡ����е�0λ�͵�1λ��ʾ�˵�����ݴ������ͣ�00B��ʾ���ƴ��䡢01B��ʾͬ�����䡢10B��ʾ�鴫 �䡢11B��ʾ�жϴ��䣻�����ͬ�����䣬��2λ�͵�3λ��ʾͬ�����ͣ�00B��ʾ��ͬ����01B��ʾ�첽��10B��ʾ����Ӧ��11B��ʾͬ������4��5 λ��ʾ�˵���÷����ͣ�00B��ʾ���ݶ˵㡢01B��ʾ��ʾ�����˵㡢10B��ʾ���䷴���˵㡢11B����������λ������ */
    0x40,          /* ���ڱ�ʾ�˵���֧��������ݰ��ĳ��ȡ����е�0��10λ��ʾ���ݰ��ĳ��ȣ���11λ��12λָ��ÿС֡��ഫ���������������λ��������������0��*/
    0x00,
    0x18,          /* ����ָ���˵����ݴ���ķ��ʼ���������ж϶˵㣬ȡֵ��ΧΪ10��255����Ӧ�ķ��ʼ��Ϊ10��255ms������ȫ���ж϶˵㣬ȡֵ��ΧΪ1��255����Ӧ�ķ��ʼ��Ϊ1��255ms�����������˵㣬���Բ���USB���Э�顣 */
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
/*����������*/
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

		
//	//�ػ�����
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

