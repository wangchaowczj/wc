#include "includes.h"

STR_COMM Uart2Data;

//==================================================================================================
//| �������� | Unpack 
//|----------|--------------------------------------------------------------------------------------
//| �������� | ���ݽ������
//|----------|--------------------------------------------------------------------------------------
//| ������� | data_in  ���ǰ�����ݽṹ
//|          | data_out ���������ݽṹ
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | ERROR   ���ʧ��
//|          | SUCCESS ����ɹ�
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�֣��    ʱ�䣺2014-08-29 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | 
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
static ErrorStatus Unpack(STR_COMM* data_in, STR_COMM* data_out)
{
	u8 crc[2];
	
	//slip���
	if(ERROR == Slip_Unpack(data_in, data_out))
	{
		return ERROR;
	}
	//CRCУ��
	GetCrc16Bit(crc, data_out->uch_buf, data_out->bytenum-2);
	if((crc[0] == data_out->uch_buf[data_out->bytenum-2]) && (crc[1]) == data_out->uch_buf[data_out->bytenum-1])
	{
		data_out->bytenum -= 2;
		return SUCCESS;
	}
	return ERROR;
}


//==================================================================================================
//| �������� | Pack 
//|----------|--------------------------------------------------------------------------------------
//| �������� | ���ݴ������
//|----------|--------------------------------------------------------------------------------------
//| ������� | data_in  ���ǰ�����ݽṹ
//|          | data_out ���������ݽṹ
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | ��
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�֣��    ʱ�䣺2014-08-29 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | 
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
static void Pack(STR_COMM* data_in, STR_COMM* data_out)
{
	if((data_in->bytenum != 0x00) && ((data_in->bytenum+2) < ((BUF_MAX-2)/2)))
	{
		//CRCУ��
		GetCrc16Bit(&data_in->uch_buf[data_in->bytenum], data_in->uch_buf, data_in->bytenum);
		
		data_in->bytenum += 2;
		
		//slip
		Slip_Pack(data_in, data_out);	
	}
	else
	{
		data_out->bytenum = 0x00;
	}
}



//==================================================================================================
//| �������� | Communication 
//|----------|--------------------------------------------------------------------------------------
//| �������� | ͨ�Ŵ���
//|----------|--------------------------------------------------------------------------------------
//| ������� | data_in  ���յ������ݽṹ
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | ��
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�֣��    ʱ�䣺2014-08-29 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | 
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
void Communication(STR_COMM* data)
{
	static STR_COMM receive, temp, send;
	u16 U16Value1, U16Value2;
	
	//���
	if(ERROR != Unpack(data, &receive))
	{
		if((receive.bytenum >= 3) && ((dev_info.dev_addr == receive.uch_buf[0]) || (0x00 == receive.uch_buf[0])))
		{			
			send.bytenum = 0;
			temp.bytenum = 0;
			temp.uch_buf[temp.bytenum++] = dev_info.dev_addr;
			temp.uch_buf[temp.bytenum++] = GUEST_CMD_FLAG;
			temp.uch_buf[temp.bytenum++] = receive.uch_buf[2];
			if(HOST_CMD_FLAG == receive.uch_buf[1])
			{
				switch(receive.uch_buf[2])
				{
					case GET_DEVICE_INFO_CMD://���豸�İ汾��Ϣ
						memcpy(&temp.uch_buf[temp.bytenum], VERSION_STRING, sizeof(VERSION_STRING));
						temp.bytenum += sizeof(VERSION_STRING);
						break;
					case GET_DEVICE_ERROR_INFO_CMD://��������Ϣ,��Ҫ�Ƿ������屣����־
						memcpy(&temp.uch_buf[temp.bytenum], dev_info.err_info, strlen((char*)dev_info.err_info)+1);
						temp.bytenum += (strlen((char*)dev_info.err_info)+1);						
						break;
					case SET_DEVICE_TEST_PARAM_CMD://���ò���
						if(34 == receive.bytenum)
						{
							test_param.det_lv_current_min = (receive.uch_buf[3]<<8) + receive.uch_buf[4];
							test_param.det_lv_current_max = (receive.uch_buf[5]<<8) + receive.uch_buf[6];
							test_param.det_lv_current_time_out = (receive.uch_buf[7]<<8) + receive.uch_buf[8];
							test_param.det_hv_current_min = (receive.uch_buf[9]<<8) + receive.uch_buf[10];
							test_param.det_hv_current_max = (receive.uch_buf[11]<<8) + receive.uch_buf[12];	
							test_param.det_hv_current_time_out = (receive.uch_buf[13]<<8) + receive.uch_buf[14];							
							test_param.det_freq_min = (receive.uch_buf[15]<<24) + (receive.uch_buf[16]<<16) +(receive.uch_buf[17]<<8) + receive.uch_buf[18];
							test_param.det_freq_max = (receive.uch_buf[19]<<24) + (receive.uch_buf[20]<<16) +(receive.uch_buf[21]<<8) + receive.uch_buf[22];
							test_param.test_ctrl = (receive.uch_buf[23]<<24) + (receive.uch_buf[24]<<16) +(receive.uch_buf[25]<<8) + receive.uch_buf[26];
							test_param.det_type = receive.uch_buf[27];
							test_param.res = receive.uch_buf[28];
							test_param.blast_cap_leak_current_max = (receive.uch_buf[29]<<8) + receive.uch_buf[30]; 
							test_param.blast_cap_leak_current_time_out = (receive.uch_buf[31]<<8) + receive.uch_buf[32]; 
							test_param.detCodeWriteType = receive.uch_buf[33];
						}
						else
						{
							temp.uch_buf[temp.bytenum-1] |= 0x80;
							temp.uch_buf[temp.bytenum++] = 0x01;//���ݳ��Ȳ���
						}
						break;
					case GET_DEVICE_TEST_PARAM_CMD://��ȡ����
						temp.uch_buf[temp.bytenum++] = (u8)(test_param.det_lv_current_min>>8);
						temp.uch_buf[temp.bytenum++] = (u8)(test_param.det_lv_current_min&0xFF);
					
						temp.uch_buf[temp.bytenum++] = (u8)(test_param.det_lv_current_max>>8);
						temp.uch_buf[temp.bytenum++] = (u8)(test_param.det_lv_current_max&0xFF);

						temp.uch_buf[temp.bytenum++] = (u8)(test_param.det_lv_current_time_out>>8);
						temp.uch_buf[temp.bytenum++] = (u8)(test_param.det_lv_current_time_out&0xFF);
					
						temp.uch_buf[temp.bytenum++] = (u8)(test_param.det_hv_current_min>>8);
						temp.uch_buf[temp.bytenum++] = (u8)(test_param.det_hv_current_min&0xFF);
					
						temp.uch_buf[temp.bytenum++] = (u8)(test_param.det_hv_current_max>>8);
						temp.uch_buf[temp.bytenum++] = (u8)(test_param.det_hv_current_max&0xFF);
					
						temp.uch_buf[temp.bytenum++] = (u8)(test_param.det_hv_current_time_out>>8);
						temp.uch_buf[temp.bytenum++] = (u8)(test_param.det_hv_current_time_out&0xFF);	
					
						temp.uch_buf[temp.bytenum++] = (u8)(test_param.det_freq_min>>24);
						temp.uch_buf[temp.bytenum++] = (u8)(test_param.det_freq_min>>16);
						temp.uch_buf[temp.bytenum++] = (u8)(test_param.det_freq_min>>8);
						temp.uch_buf[temp.bytenum++] = (u8)(test_param.det_freq_min&0xFF);
					
						temp.uch_buf[temp.bytenum++] = (u8)(test_param.det_freq_max>>24);
						temp.uch_buf[temp.bytenum++] = (u8)(test_param.det_freq_max>>16);
						temp.uch_buf[temp.bytenum++] = (u8)(test_param.det_freq_max>>8);
						temp.uch_buf[temp.bytenum++] = (u8)(test_param.det_freq_max&0xFF);
						
						temp.uch_buf[temp.bytenum++] = (u8)(test_param.test_ctrl>>24);
						temp.uch_buf[temp.bytenum++] = (u8)(test_param.test_ctrl>>16);
						temp.uch_buf[temp.bytenum++] = (u8)(test_param.test_ctrl>>8);
						temp.uch_buf[temp.bytenum++] = (u8)(test_param.test_ctrl&0xFF);	

						temp.uch_buf[temp.bytenum++] = test_param.det_type;
						temp.uch_buf[temp.bytenum++] = test_param.res;

						temp.uch_buf[temp.bytenum++] = (u8)(test_param.blast_cap_leak_current_max>>8);
						temp.uch_buf[temp.bytenum++] = (u8)(test_param.blast_cap_leak_current_max&0xFF);
					
						temp.uch_buf[temp.bytenum++] = (u8)(test_param.blast_cap_leak_current_time_out>>8);
						temp.uch_buf[temp.bytenum++] = (u8)(test_param.blast_cap_leak_current_time_out&0xFF);

						temp.uch_buf[temp.bytenum++] = test_param.detCodeWriteType;
						break;
					case SET_DEVICE_SELFTEST_PARAM_CMD://�����Լ����
						if(31 == receive.bytenum)
						{
							self_param.ref_min = (receive.uch_buf[3]<<8) + receive.uch_buf[4];
							self_param.ref_max = (receive.uch_buf[5]<<8) + receive.uch_buf[6];
						
							self_param.dual_min = (receive.uch_buf[7]<<8) + receive.uch_buf[8];
							self_param.dual_max = (receive.uch_buf[9]<<8) + receive.uch_buf[10];
						
							self_param.linab_min = (receive.uch_buf[11]<<8) + receive.uch_buf[12];
							self_param.linab_max = (receive.uch_buf[13]<<8) + receive.uch_buf[14];
						
							self_param.power_5v_min = (receive.uch_buf[15]<<8) + receive.uch_buf[16];
							self_param.power_5v_max = (receive.uch_buf[17]<<8) + receive.uch_buf[18];

							self_param.hv_min = (receive.uch_buf[19]<<8) + receive.uch_buf[20];
							self_param.hv_max = (receive.uch_buf[21]<<8) + receive.uch_buf[22];
						
							self_param.lv_min = (receive.uch_buf[23]<<8) + receive.uch_buf[24];
							self_param.lv_max = (receive.uch_buf[25]<<8) + receive.uch_buf[26];
						
							self_param.ref_plus_min = (receive.uch_buf[27]<<8) + receive.uch_buf[28];
							self_param.ref_plus_max = (receive.uch_buf[29]<<8) + receive.uch_buf[30];						
						}
						else
						{
							temp.uch_buf[temp.bytenum-1] |= 0x80;
							temp.uch_buf[temp.bytenum++] = 0x01;//���ݳ��Ȳ���						
						}
						break;
					case GET_DEVICE_SELFTEST_PARAM_CMD://���Լ����
						temp.uch_buf[temp.bytenum++] = (u8)(self_param.ref_min>>8);
						temp.uch_buf[temp.bytenum++] = (u8)(self_param.ref_max&0xFF);

						temp.uch_buf[temp.bytenum++] = (u8)(self_param.dual_min>>8);
						temp.uch_buf[temp.bytenum++] = (u8)(self_param.dual_max&0xFF);

						temp.uch_buf[temp.bytenum++] = (u8)(self_param.linab_min>>8);
						temp.uch_buf[temp.bytenum++] = (u8)(self_param.linab_max&0xFF);

						temp.uch_buf[temp.bytenum++] = (u8)(self_param.power_5v_min>>8);
						temp.uch_buf[temp.bytenum++] = (u8)(self_param.power_5v_max&0xFF);

						temp.uch_buf[temp.bytenum++] = (u8)(self_param.hv_min>>8);
						temp.uch_buf[temp.bytenum++] = (u8)(self_param.hv_max&0xFF);

						temp.uch_buf[temp.bytenum++] = (u8)(self_param.lv_min>>8);
						temp.uch_buf[temp.bytenum++] = (u8)(self_param.lv_max&0xFF);	

						temp.uch_buf[temp.bytenum++] = (u8)(self_param.ref_plus_min>>8);
						temp.uch_buf[temp.bytenum++] = (u8)(self_param.ref_plus_max&0xFF);					
						break;
					case SET_DEVICE_START_SELFTEST_CMD://��ʼ�Լ�
						OSTaskCreate(SelfTestTask, (void*)0,
							&app_os_stk.SelfTest[APP_SELFTEST_TASK_STK_SIZE-1],APP_SELFTEST_TASK_PRIO);
						break;
					case GET_DEVICE_SELFTEST_RESULT_CMD://��ȡ�Լ���
						temp.uch_buf[temp.bytenum++] = (u8)dev_info.self_result;
						break;
					case SET_DEVICE_START_DETTEST_CMD://��ʼ�׹ܲ���
						#ifdef DEBUG
						if(DeviceSelfwaveFlag != 0)
						{//�������1000HZ����
							temp.uch_buf[temp.bytenum-1] |= 0x80;
							temp.uch_buf[temp.bytenum++] = 0x02;//								
						}
						else
						{
							det_info.state = DET_TESTING;
							OSTaskCreate(DetTestTask, (void*)0,
								&app_os_stk.DetTest[APP_DETTEST_TASK_STK_SIZE-1],APP_DETTEST_TASK_PRIO);
						}						
						#else
						if(ERROR == dev_info.self_result)
						{//�ϵ���Լ�ʧ�ܻ���û���Լ죬����������׹ܲ���
							temp.uch_buf[temp.bytenum-1] |= 0x80;
							temp.uch_buf[temp.bytenum++] = 0x01;//								
						}
						else if(DeviceSelfwaveFlag != 0)
						{//�������1000HZ����
							temp.uch_buf[temp.bytenum-1] |= 0x80;
							temp.uch_buf[temp.bytenum++] = 0x02;//								
						}
						else
						{
							det_info.state = DET_TESTING;
							OSTaskCreate(DetTestTask, (void*)0,
								&app_os_stk.DetTest[APP_DETTEST_TASK_STK_SIZE-1],APP_DETTEST_TASK_PRIO);
						}
						#endif
						break;
					case GET_DEVICE_DETTEST_RESULT_CMD://��ȡ�׹ܲ��Խ��
						#ifndef DEBUG
						if(ERROR == dev_info.self_result)
						{//�ϵ���Լ�ʧ�ܻ���û���Լ죬���������оƬ����,�����ڲ���оƬ������MCU��λʱ������Ӧ�����ݴ���
							temp.uch_buf[temp.bytenum-1] |= 0x80;
							temp.uch_buf[temp.bytenum++] = 0x01;								
						}
						else
						#endif
						{
							OSSchedLock();
							temp.uch_buf[temp.bytenum++] = (u8)det_info.state;
							temp.uch_buf[temp.bytenum++] = (u8)det_info.ser1;
							temp.uch_buf[temp.bytenum++] = (u8)det_info.ser2;
							temp.uch_buf[temp.bytenum++] = (u8)det_info.ser3;
							temp.uch_buf[temp.bytenum++] = (u8)det_info.fac;
							temp.uch_buf[temp.bytenum++] = (u8)det_info.bat;
							temp.uch_buf[temp.bytenum++] = (u8)det_info.year;
							temp.uch_buf[temp.bytenum++] = (u8)det_info.Pwd1;
							temp.uch_buf[temp.bytenum++] = (u8)det_info.Pwd2;
							temp.uch_buf[temp.bytenum++] = (u8)(det_info.freq>>24);
							temp.uch_buf[temp.bytenum++] = (u8)(det_info.freq>>16);
							temp.uch_buf[temp.bytenum++] = (u8)(det_info.freq>>8);
							temp.uch_buf[temp.bytenum++] = (u8)(det_info.freq&0xFF);					
							temp.uch_buf[temp.bytenum++] = (u8)det_info.type;
							temp.uch_buf[temp.bytenum++] = (u8)(det_info.lv_current>>8);
							temp.uch_buf[temp.bytenum++] = (u8)(det_info.lv_current&0xFF);	
							temp.uch_buf[temp.bytenum++] = (u8)(det_info.lv_reverse_current>>8);
							temp.uch_buf[temp.bytenum++] = (u8)(det_info.lv_reverse_current&0xFF);
							temp.uch_buf[temp.bytenum++] = (u8)(det_info.hv_current>>8);
							temp.uch_buf[temp.bytenum++] = (u8)(det_info.hv_current&0xFF);	
							temp.uch_buf[temp.bytenum++] = (u8)(det_info.hv_reverse_current>>8);
							temp.uch_buf[temp.bytenum++] = (u8)(det_info.hv_reverse_current&0xFF);
							temp.uch_buf[temp.bytenum++] = (u8)(det_info.blast_cap_leak_current>>8);
							temp.uch_buf[temp.bytenum++] = (u8)(det_info.blast_cap_leak_current&0xFF);	

							memcpy((char*)&temp.uch_buf[temp.bytenum], (char*)&DetTestItemStatus, sizeof(DET_TEST_ITEM_STATUS));
							temp.bytenum += sizeof(DET_TEST_ITEM_STATUS);
							
							OSSchedUnlock();
						}											
						break;	
					case SET_DEVICE_ADJUST_LV_PARAM_CMD://�����豸��ѹ����У׼����
						if((receive.bytenum-3) == 10)
						{
							U16Value1 = (((u16)receive.uch_buf[3])<<8) + receive.uch_buf[4];
							U16Value2 = (((u16)receive.uch_buf[5])<<8) + receive.uch_buf[6];
							adc_adjust.lv_current.k = U16Value1/1000.0;
							adc_adjust.lv_current.b = U16Value2;
							adc_adjust.lv_current.year = receive.uch_buf[7];
							adc_adjust.lv_current.month = receive.uch_buf[8];
							adc_adjust.lv_current.day = receive.uch_buf[9];
							adc_adjust.lv_current.hour = receive.uch_buf[10];
							adc_adjust.lv_current.min = receive.uch_buf[11];
							adc_adjust.lv_current.sec = receive.uch_buf[12];
							
							adc_adjust.hv_current = adc_adjust.lv_current;
							
							if(ERROR == WriteFactoryConfigParam())
							{
								temp.bytenum = 2 ;
								temp.uch_buf[temp.bytenum++] = receive.uch_buf[2] | 0x80;
								temp.uch_buf[temp.bytenum++] = 0x02;//���ݱ���ʧ��								
							}
						}
						else
						{
							temp.bytenum = 2 ;
							temp.uch_buf[temp.bytenum++] = receive.uch_buf[2] | 0x80;
							temp.uch_buf[temp.bytenum++] = 0x01;//���ݳ��ȴ���							
						}
						break;
					case GET_DEVICE_ADJUST_LV_PARAM_CMD://���豸��ѹ����У׼����
						U16Value1 = (u16)(adc_adjust.lv_current.k*1000);
	
						temp.uch_buf[temp.bytenum++] = U16Value1>>8;
						temp.uch_buf[temp.bytenum++] = U16Value1&0xFF;
						temp.uch_buf[temp.bytenum++] = adc_adjust.lv_current.b>>8;
						temp.uch_buf[temp.bytenum++] = adc_adjust.lv_current.b&0xFF;
						temp.uch_buf[temp.bytenum++] = adc_adjust.lv_current.year;
						temp.uch_buf[temp.bytenum++] = adc_adjust.lv_current.month;
						temp.uch_buf[temp.bytenum++] = adc_adjust.lv_current.day;
						temp.uch_buf[temp.bytenum++] = adc_adjust.lv_current.hour;
						temp.uch_buf[temp.bytenum++] = adc_adjust.lv_current.min;
						temp.uch_buf[temp.bytenum++] = adc_adjust.lv_current.sec;
						break;
					case GET_DEVICE_LINAB_CURRENT_CMD://��ȡ������AD����ֵ
						U16Value1 = GetLVCurrentValue(&U16Value2);
						temp.uch_buf[temp.bytenum++] = U16Value1>>8;  //����ֵ
						temp.uch_buf[temp.bytenum++] = U16Value1&0xFF;//����ֵ
						temp.uch_buf[temp.bytenum++] = U16Value2>>8;  //��Ӧ��ADC����ֵ
						temp.uch_buf[temp.bytenum++] = U16Value2&0xFF;//��Ӧ��ADC����ֵ						
						break;
					case GET_DEVICE_LINAB_VOLTAGE_CMD://��ȡ��ѹ��AD����ֵ
						U16Value1 = GetLinABVoltageAndAdcMeasureValue(&U16Value2);
						temp.uch_buf[temp.bytenum++] = U16Value1>>8;  //��ѹֵ
						temp.uch_buf[temp.bytenum++] = U16Value1&0xFF;//��ѹֵ
						temp.uch_buf[temp.bytenum++] = U16Value2>>8;  //��Ӧ��ADC����ֵ
						temp.uch_buf[temp.bytenum++] = U16Value2&0xFF;//��Ӧ��ADC����ֵ						
						break;
					case SET_DEVICE_LINAB_WAVE_CMD://���1000HZ����,ֹͣ���ʱ�������ϵ�
						DeviceSelfwaveFlag = 1;
						break;
					case SET_DEVICE_OPEN_LINAB_VOLTAGE_CMD://�򿪽��ߵ�ѹ
						if(receive.bytenum == 4)
						{							
							if(0 == receive.uch_buf[3])
							{
								CLOSE_HV_SW();//�رո�ѹ�������
								SET_LIN_SHORT();	
							}
							else if(2 == receive.uch_buf[3])
							{
								OPEN_HV_SW();//��ѹ���
								SET_LIN_OPEN();	
							}
							else
							{//1
								CLOSE_HV_SW();//�رո�ѹ�������
								SET_LIN_OPEN();
							}
						}															
						break;
					case SET_DEVICE_OPEN_BRIDGE_VOLTAGE_CMD://����ͷ��ѹ
						if(receive.bytenum == 4)
						{
							if(0 == receive.uch_buf[3])
							{
								CLOSE_BRIDGE_POWER();
							}
							else
							{
								OPEN_BRIDGE_POWER();//����ͷ��ѹ	
							}						
						}											
						break;
					case SET_DEVICE_ADJUST_BRIDGE_PARAM_CMD://�����豸��ѹ����У׼����
						if((receive.bytenum-3) == 10)
						{
							U16Value1 = (((u16)receive.uch_buf[3])<<8) + receive.uch_buf[4];
							U16Value2 = (((u16)receive.uch_buf[5])<<8) + receive.uch_buf[6];
							adc_adjust.bridge_current.k = U16Value1/1000.0;
							adc_adjust.bridge_current.b = U16Value2;
							adc_adjust.bridge_current.year = receive.uch_buf[7];
							adc_adjust.bridge_current.month = receive.uch_buf[8];
							adc_adjust.bridge_current.day = receive.uch_buf[9];
							adc_adjust.bridge_current.hour = receive.uch_buf[10];
							adc_adjust.bridge_current.min = receive.uch_buf[11];
							adc_adjust.bridge_current.sec = receive.uch_buf[12];
							
							if(ERROR == WriteFactoryConfigParam())
							{
								temp.bytenum = 2 ;
								temp.uch_buf[temp.bytenum++] = receive.uch_buf[2] | 0x80;
								temp.uch_buf[temp.bytenum++] = 0x02;//���ݱ���ʧ��								
							}
						}
						else
						{
							temp.bytenum = 2 ;
							temp.uch_buf[temp.bytenum++] = receive.uch_buf[2] | 0x80;
							temp.uch_buf[temp.bytenum++] = 0x01;//���ݳ��ȴ���							
						}
						break;
					case GET_DEVICE_ADJUST_BRIDGE_PARAM_CMD://���豸��ͷ����У׼����
						U16Value1 = (u16)(adc_adjust.bridge_current.k*1000);
	
						temp.uch_buf[temp.bytenum++] = U16Value1>>8;
						temp.uch_buf[temp.bytenum++] = U16Value1&0xFF;
						temp.uch_buf[temp.bytenum++] = adc_adjust.bridge_current.b>>8;
						temp.uch_buf[temp.bytenum++] = adc_adjust.bridge_current.b&0xFF;
						temp.uch_buf[temp.bytenum++] = adc_adjust.bridge_current.year;
						temp.uch_buf[temp.bytenum++] = adc_adjust.bridge_current.month;
						temp.uch_buf[temp.bytenum++] = adc_adjust.bridge_current.day;
						temp.uch_buf[temp.bytenum++] = adc_adjust.bridge_current.hour;
						temp.uch_buf[temp.bytenum++] = adc_adjust.bridge_current.min;
						temp.uch_buf[temp.bytenum++] = adc_adjust.bridge_current.sec;
						break;
					case GET_DEVICE_BRIDGE_CURRENT_CMD://��ȡ��ͷ������AD����ֵ
						U16Value1 = GetBridgeCurrentValue(&U16Value2);
						temp.uch_buf[temp.bytenum++] = U16Value1>>8;  //����ֵ
						temp.uch_buf[temp.bytenum++] = U16Value1&0xFF;//����ֵ
						temp.uch_buf[temp.bytenum++] = U16Value2>>8;  //��Ӧ��ADC����ֵ
						temp.uch_buf[temp.bytenum++] = U16Value2&0xFF;//��Ӧ��ADC����ֵ						
						break;
					case SET_DEVICE_DET_NEW_CODE_CMD:
						if((receive.bytenum-3) == 6)
						{
							memcpy(DetCodeWrite, &receive.uch_buf[3], 6);
						}
						else
						{
							temp.bytenum = 2 ;
							temp.uch_buf[temp.bytenum++] = receive.uch_buf[2] | 0x80;
							temp.uch_buf[temp.bytenum++] = 0x01;//���ݳ��ȴ���							
						}											
						break;
					default:
						temp.bytenum = 0;
						break;
				}
				if((0x00 != temp.bytenum) && (0x00 != receive.uch_buf[0]))
				{//��Ҫ�ظ�����
					Pack(&temp, data);
					data->status = SENDING;
					return;
				}
			}
		}
		data->bytenum = 0;
		data->status = RECIEVING;//������������		
	}
	else
	{
		data->bytenum = 1;
		data->status = RECIEVING;//������������	
	}
}
