#ifndef _COMMUNICATION_H_
#define _COMMUNICATION_H_

//主/从命令标志
#define HOST_CMD_FLAG            '@'
#define GUEST_CMD_FLAG           '#'

//命令定义如下：
#define GET_DEVICE_INFO_CMD                   			0x01     /**<获取设备信息命令*/
#define GET_DEVICE_ERROR_INFO_CMD             			0x02     /**<获取错误信息*/
#define SET_DEVICE_TEST_PARAM_CMD             			0x03     /**<设置设备配置参数命令*/
#define GET_DEVICE_TEST_PARAM_CMD             			0x04	 /**<获取设备配置参数命令*/
#define SET_DEVICE_SELFTEST_PARAM_CMD         			0x05	 /**<设置设备自检参数命令*/
#define GET_DEVICE_SELFTEST_PARAM_CMD         			0x06	 /**<获取设备自检参数命令*/
#define SET_DEVICE_START_SELFTEST_CMD         			0x07     /**<设置设备开始自检命令*/
#define GET_DEVICE_SELFTEST_RESULT_CMD        			0x08     /**<获取设备自检结果命令*/
#define SET_DEVICE_START_DETTEST_CMD          			0x09     /**<设置设备开始测试雷管命令*/
#define GET_DEVICE_DETTEST_RESULT_CMD         			0x0A     /**<获取设备测试雷管结果*/
#define SET_DEVICE_ERASE_APP_CMD              			0x0B     /**<控制擦除应用程序*/
#define GET_DEVICE_ERASE_APP_RESULT_CMD       			0x0C     /**<获取擦除应用程序结果*/
#define SET_DEVICE_PROGRAM_DATA_CMD           			0x0D     /**<更新应用程序数据*/
#define SET_DEVICE_ADJUST_LV_PARAM_CMD        			0x0E     /**<设置设备低压电流校准参数命令*/
#define GET_DEVICE_ADJUST_LV_PARAM_CMD        			0x0F	 /**<获取设备低压电流校准参数命令*/
#define GET_DEVICE_LINAB_CURRENT_CMD          			0x10	 /**<获取设备电流值命令*/
#define GET_DEVICE_LINAB_VOLTAGE_CMD          			0x11	 /**<获取设备电压值命令*/
#define SET_DEVICE_LINAB_WAVE_CMD             			0x12	 /**<设置设备输出500HZ方波命令*/
#define SET_DEVICE_OPEN_LINAB_VOLTAGE_CMD     			0x13	 /**<设置设备打开/关闭脚线电压命令*/
#define SET_DEVICE_OPEN_BRIDGE_VOLTAGE_CMD    			0x14	 /**<设置设备打开/关闭桥头电压命令*/
#define SET_DEVICE_ADJUST_BRIDGE_PARAM_CMD        		0x15     /**<设置设备桥头电流校准参数命令*/
#define GET_DEVICE_ADJUST_BRIDGE_PARAM_CMD        		0x16	 /**<获取设备桥头电流校准参数命令*/
#define GET_DEVICE_BRIDGE_CURRENT_CMD          			0x17	 /**<获取设备桥头电流值命令*/
#define SET_DEVICE_DET_NEW_CODE_CMD           			0x18     /**<设置雷管待写入的内码命令*/

extern STR_COMM Uart2Data;
extern void Communication(STR_COMM* data_in);
#endif
