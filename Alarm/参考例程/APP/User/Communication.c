#include "includes.h"

STR_COMM Uart2Data;

//==================================================================================================
//| 函数名称 | Unpack 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 数据解包函数
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | data_in  解包前的数据结构
//|          | data_out 解包后的数据结构
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | ERROR   解包失败
//|          | SUCCESS 解包成功
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2014-08-29 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
static ErrorStatus Unpack(STR_COMM* data_in, STR_COMM* data_out)
{
	u8 crc[2];
	
	//slip解包
	if(ERROR == Slip_Unpack(data_in, data_out))
	{
		return ERROR;
	}
	//CRC校验
	GetCrc16Bit(crc, data_out->uch_buf, data_out->bytenum-2);
	if((crc[0] == data_out->uch_buf[data_out->bytenum-2]) && (crc[1]) == data_out->uch_buf[data_out->bytenum-1])
	{
		data_out->bytenum -= 2;
		return SUCCESS;
	}
	return ERROR;
}


//==================================================================================================
//| 函数名称 | Pack 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 数据打包函数
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | data_in  打包前的数据结构
//|          | data_out 打包后的数据结构
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2014-08-29 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
static void Pack(STR_COMM* data_in, STR_COMM* data_out)
{
	if((data_in->bytenum != 0x00) && ((data_in->bytenum+2) < ((BUF_MAX-2)/2)))
	{
		//CRC校验
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
//| 函数名称 | Communication 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 通信处理
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | data_in  接收到的数据结构
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2014-08-29 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
void Communication(STR_COMM* data)
{
	static STR_COMM receive, temp, send;
	u16 U16Value1, U16Value2;
	
	//解包
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
					case GET_DEVICE_INFO_CMD://读设备的版本信息
						memcpy(&temp.uch_buf[temp.bytenum], VERSION_STRING, sizeof(VERSION_STRING));
						temp.bytenum += sizeof(VERSION_STRING);
						break;
					case GET_DEVICE_ERROR_INFO_CMD://读错误信息,主要是方便主板保存日志
						memcpy(&temp.uch_buf[temp.bytenum], dev_info.err_info, strlen((char*)dev_info.err_info)+1);
						temp.bytenum += (strlen((char*)dev_info.err_info)+1);						
						break;
					case SET_DEVICE_TEST_PARAM_CMD://设置参数
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
							temp.uch_buf[temp.bytenum++] = 0x01;//数据长度不足
						}
						break;
					case GET_DEVICE_TEST_PARAM_CMD://读取参数
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
					case SET_DEVICE_SELFTEST_PARAM_CMD://设置自检参数
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
							temp.uch_buf[temp.bytenum++] = 0x01;//数据长度不足						
						}
						break;
					case GET_DEVICE_SELFTEST_PARAM_CMD://读自检参数
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
					case SET_DEVICE_START_SELFTEST_CMD://开始自检
						OSTaskCreate(SelfTestTask, (void*)0,
							&app_os_stk.SelfTest[APP_SELFTEST_TASK_STK_SIZE-1],APP_SELFTEST_TASK_PRIO);
						break;
					case GET_DEVICE_SELFTEST_RESULT_CMD://读取自检结果
						temp.uch_buf[temp.bytenum++] = (u8)dev_info.self_result;
						break;
					case SET_DEVICE_START_DETTEST_CMD://开始雷管测试
						#ifdef DEBUG
						if(DeviceSelfwaveFlag != 0)
						{//正在输出1000HZ方波
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
						{//上电后自检失败或者没有自检，不允许进行雷管测试
							temp.uch_buf[temp.bytenum-1] |= 0x80;
							temp.uch_buf[temp.bytenum++] = 0x01;//								
						}
						else if(DeviceSelfwaveFlag != 0)
						{//正在输出1000HZ方波
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
					case GET_DEVICE_DETTEST_RESULT_CMD://读取雷管测试结果
						#ifndef DEBUG
						if(ERROR == dev_info.self_result)
						{//上电后自检失败或者没有自检，不允许进行芯片测试,避免在测试芯片过程中MCU复位时导致响应的数据错误
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
					case SET_DEVICE_ADJUST_LV_PARAM_CMD://设置设备低压电流校准参数
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
								temp.uch_buf[temp.bytenum++] = 0x02;//数据保存失败								
							}
						}
						else
						{
							temp.bytenum = 2 ;
							temp.uch_buf[temp.bytenum++] = receive.uch_buf[2] | 0x80;
							temp.uch_buf[temp.bytenum++] = 0x01;//数据长度错误							
						}
						break;
					case GET_DEVICE_ADJUST_LV_PARAM_CMD://读设备低压电流校准参数
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
					case GET_DEVICE_LINAB_CURRENT_CMD://获取电流、AD测量值
						U16Value1 = GetLVCurrentValue(&U16Value2);
						temp.uch_buf[temp.bytenum++] = U16Value1>>8;  //电流值
						temp.uch_buf[temp.bytenum++] = U16Value1&0xFF;//电流值
						temp.uch_buf[temp.bytenum++] = U16Value2>>8;  //对应的ADC测量值
						temp.uch_buf[temp.bytenum++] = U16Value2&0xFF;//对应的ADC测量值						
						break;
					case GET_DEVICE_LINAB_VOLTAGE_CMD://获取电压、AD测量值
						U16Value1 = GetLinABVoltageAndAdcMeasureValue(&U16Value2);
						temp.uch_buf[temp.bytenum++] = U16Value1>>8;  //电压值
						temp.uch_buf[temp.bytenum++] = U16Value1&0xFF;//电压值
						temp.uch_buf[temp.bytenum++] = U16Value2>>8;  //对应的ADC测量值
						temp.uch_buf[temp.bytenum++] = U16Value2&0xFF;//对应的ADC测量值						
						break;
					case SET_DEVICE_LINAB_WAVE_CMD://输出1000HZ方波,停止输出时需重新上电
						DeviceSelfwaveFlag = 1;
						break;
					case SET_DEVICE_OPEN_LINAB_VOLTAGE_CMD://打开脚线电压
						if(receive.bytenum == 4)
						{							
							if(0 == receive.uch_buf[3])
							{
								CLOSE_HV_SW();//关闭高压输出开关
								SET_LIN_SHORT();	
							}
							else if(2 == receive.uch_buf[3])
							{
								OPEN_HV_SW();//高压输出
								SET_LIN_OPEN();	
							}
							else
							{//1
								CLOSE_HV_SW();//关闭高压输出开关
								SET_LIN_OPEN();
							}
						}															
						break;
					case SET_DEVICE_OPEN_BRIDGE_VOLTAGE_CMD://打开桥头电压
						if(receive.bytenum == 4)
						{
							if(0 == receive.uch_buf[3])
							{
								CLOSE_BRIDGE_POWER();
							}
							else
							{
								OPEN_BRIDGE_POWER();//打开桥头电压	
							}						
						}											
						break;
					case SET_DEVICE_ADJUST_BRIDGE_PARAM_CMD://设置设备低压电流校准参数
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
								temp.uch_buf[temp.bytenum++] = 0x02;//数据保存失败								
							}
						}
						else
						{
							temp.bytenum = 2 ;
							temp.uch_buf[temp.bytenum++] = receive.uch_buf[2] | 0x80;
							temp.uch_buf[temp.bytenum++] = 0x01;//数据长度错误							
						}
						break;
					case GET_DEVICE_ADJUST_BRIDGE_PARAM_CMD://读设备桥头电流校准参数
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
					case GET_DEVICE_BRIDGE_CURRENT_CMD://获取桥头电流、AD测量值
						U16Value1 = GetBridgeCurrentValue(&U16Value2);
						temp.uch_buf[temp.bytenum++] = U16Value1>>8;  //电流值
						temp.uch_buf[temp.bytenum++] = U16Value1&0xFF;//电流值
						temp.uch_buf[temp.bytenum++] = U16Value2>>8;  //对应的ADC测量值
						temp.uch_buf[temp.bytenum++] = U16Value2&0xFF;//对应的ADC测量值						
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
							temp.uch_buf[temp.bytenum++] = 0x01;//数据长度错误							
						}											
						break;
					default:
						temp.bytenum = 0;
						break;
				}
				if((0x00 != temp.bytenum) && (0x00 != receive.uch_buf[0]))
				{//需要回复主机
					Pack(&temp, data);
					data->status = SENDING;
					return;
				}
			}
		}
		data->bytenum = 0;
		data->status = RECIEVING;//继续接收数据		
	}
	else
	{
		data->bytenum = 1;
		data->status = RECIEVING;//继续接收数据	
	}
}
