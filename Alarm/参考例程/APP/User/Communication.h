#ifndef _COMMUNICATION_H_
#define _COMMUNICATION_H_

//��/�������־
#define HOST_CMD_FLAG            '@'
#define GUEST_CMD_FLAG           '#'

//��������£�
#define GET_DEVICE_INFO_CMD                   			0x01     /**<��ȡ�豸��Ϣ����*/
#define GET_DEVICE_ERROR_INFO_CMD             			0x02     /**<��ȡ������Ϣ*/
#define SET_DEVICE_TEST_PARAM_CMD             			0x03     /**<�����豸���ò�������*/
#define GET_DEVICE_TEST_PARAM_CMD             			0x04	 /**<��ȡ�豸���ò�������*/
#define SET_DEVICE_SELFTEST_PARAM_CMD         			0x05	 /**<�����豸�Լ��������*/
#define GET_DEVICE_SELFTEST_PARAM_CMD         			0x06	 /**<��ȡ�豸�Լ��������*/
#define SET_DEVICE_START_SELFTEST_CMD         			0x07     /**<�����豸��ʼ�Լ�����*/
#define GET_DEVICE_SELFTEST_RESULT_CMD        			0x08     /**<��ȡ�豸�Լ�������*/
#define SET_DEVICE_START_DETTEST_CMD          			0x09     /**<�����豸��ʼ�����׹�����*/
#define GET_DEVICE_DETTEST_RESULT_CMD         			0x0A     /**<��ȡ�豸�����׹ܽ��*/
#define SET_DEVICE_ERASE_APP_CMD              			0x0B     /**<���Ʋ���Ӧ�ó���*/
#define GET_DEVICE_ERASE_APP_RESULT_CMD       			0x0C     /**<��ȡ����Ӧ�ó�����*/
#define SET_DEVICE_PROGRAM_DATA_CMD           			0x0D     /**<����Ӧ�ó�������*/
#define SET_DEVICE_ADJUST_LV_PARAM_CMD        			0x0E     /**<�����豸��ѹ����У׼��������*/
#define GET_DEVICE_ADJUST_LV_PARAM_CMD        			0x0F	 /**<��ȡ�豸��ѹ����У׼��������*/
#define GET_DEVICE_LINAB_CURRENT_CMD          			0x10	 /**<��ȡ�豸����ֵ����*/
#define GET_DEVICE_LINAB_VOLTAGE_CMD          			0x11	 /**<��ȡ�豸��ѹֵ����*/
#define SET_DEVICE_LINAB_WAVE_CMD             			0x12	 /**<�����豸���500HZ��������*/
#define SET_DEVICE_OPEN_LINAB_VOLTAGE_CMD     			0x13	 /**<�����豸��/�رս��ߵ�ѹ����*/
#define SET_DEVICE_OPEN_BRIDGE_VOLTAGE_CMD    			0x14	 /**<�����豸��/�ر���ͷ��ѹ����*/
#define SET_DEVICE_ADJUST_BRIDGE_PARAM_CMD        		0x15     /**<�����豸��ͷ����У׼��������*/
#define GET_DEVICE_ADJUST_BRIDGE_PARAM_CMD        		0x16	 /**<��ȡ�豸��ͷ����У׼��������*/
#define GET_DEVICE_BRIDGE_CURRENT_CMD          			0x17	 /**<��ȡ�豸��ͷ����ֵ����*/
#define SET_DEVICE_DET_NEW_CODE_CMD           			0x18     /**<�����׹ܴ�д�����������*/

extern STR_COMM Uart2Data;
extern void Communication(STR_COMM* data_in);
#endif
