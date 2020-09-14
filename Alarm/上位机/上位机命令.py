# -*- coding: utf-8 -*-
__author__ = 'ZH'

import PackProtool
import WinHid
import binascii
import time

class DeviceCmd(object):
    def __init__(self):
        self.MyHid = WinHid.WinHid()

    def openDevice(self):
        return  self.MyHid.openHidDevice(0x9000,0x000E)
    def closeDevice(self):
        self.MyHid.closeHidDevice()
    def sendAndNotWaitReceive(self, SendData):
        self.MyHid.sendData(SendData)
    def sendAndWaitReceive(self, SendData, WaiteTimeSec = 1.0, Retry = 1, PureFlag = False):
        for i in range(Retry):
            result = self.MyHid.sendData(SendData)
            if result:
                EndClock = time.clock()+ WaiteTimeSec
                while time.clock() < EndClock:
                    recData = self.MyHid.getData()
                    if len(recData) != 0:
                        #收到数据后尝试解包
                        result = PackProtool.UnpackData(recData)
                        if result[0] == True:
                            Data = result[1][2].decode('GBK', 'ignore')
                            if PureFlag == False:
                                Data = Data.split('\r')
                                if len(Data) <= 1 or Data[-1] != '':
                                    return False,""
                            return True, Data[0:-1]

        return False,""
    def getVersion(self, address = 0x41):
        '''读软件版本号'''
        SendData = PackProtool.PackData(0x0000,0xffff,"", address)
        return self.sendAndWaitReceive(SendData,WaiteTimeSec = 0.1)
    def getId(self, address = 0x41):
        '''读ID'''
        SendData = PackProtool.PackData(0x0001,0xffff, '', address)
        return self.sendAndWaitReceive(SendData,WaiteTimeSec = 0.1)
    def setId(self, Param, address = 0x41):
        '''设置ID'''
        SendData = PackProtool.PackData(0x0002,0xffff, Param, address)
        return self.sendAndWaitReceive(SendData)

    def getHardware(self, address = 0x41):
        '''读硬件版本号'''
        SendData = PackProtool.PackData(0x0003,0xffff, '', address)
        return self.sendAndWaitReceive(SendData,WaiteTimeSec = 0.1)
    def setHardware(self, Version, address = 0x41):
        '''设置硬件版本号'''
        SendData = PackProtool.PackData(0x0004,0xffff,Version, address)
        return self.sendAndWaitReceive(SendData)

    def setCloseVoltage(self, address = 0x41):
        '''控制设备关闭总线电压'''
        SendData = PackProtool.PackData(0x0005,0xffff, '', address)
        return self.sendAndWaitReceive(SendData)

    def setLinABOutputLV(self, address = 0x41):
        '''控制设备输出LV'''
        SendData = PackProtool.PackData(0x0006,0xffff, '', address)
        return self.sendAndWaitReceive(SendData)

    def setLinABOutput5V(self, address = 0x41):
        '''控制设备输出5V'''
        SendData = PackProtool.PackData(0x0007,0xffff, '', address)
        return self.sendAndWaitReceive(SendData)

    def getLinABCurrent(self, address = 0x41):
        '''读总线电流'''
        SendData = PackProtool.PackData(0x0008,0xffff, '', address)
        return self.sendAndWaitReceive(SendData)

    def getLinABVoltage(self, address = 0x41):
        '''读总线电压'''
        SendData = PackProtool.PackData(0x0009,0xffff, '', address)
        return self.sendAndWaitReceive(SendData)

    def getVoltageAdjustParam(self, address = 0x41):
        '''读总线电压校准参数'''
        SendData = PackProtool.PackData(0x000A,0xffff, '0\r', address)
        return self.sendAndWaitReceive(SendData)
    def getCurrentLowAdjustParam(self, address = 0x41):
        '''读总线电流(小量程)校准参数'''
        SendData = PackProtool.PackData(0x000A,0xffff, '1\r', address)
        return self.sendAndWaitReceive(SendData)
    def getCurrentHighAdjustParam(self, address = 0x41):
        '''读总线电流(大量程)校准参数'''
        SendData = PackProtool.PackData(0x000A,0xffff, '2\r', address)
        return self.sendAndWaitReceive(SendData)

    def setVoltageAdjustParam(self, Param, address = 0x41):
        '''设置总线电压校准参数'''
        SendData = PackProtool.PackData(0x000B,0xffff, '0\r'+Param, address)
        return self.sendAndWaitReceive(SendData)
    def setCurrentLowAdjustParam(self, Param, address = 0x41):
        '''设置总线电流(小量程)校准参数'''
        SendData = PackProtool.PackData(0x000B,0xffff, '1\r'+Param, address)
        return self.sendAndWaitReceive(SendData)
    def setCurrentHighAdjustParam(self, Param, address = 0x41):
        '''设置总线电流(大量程)校准参数'''
        SendData = PackProtool.PackData(0x000B,0xffff, '2\r'+Param, address)
        return self.sendAndWaitReceive(SendData)

    def deleteVoltageAdjustParam(self, address = 0x41):
        '''删除电压校准参数'''
        SendData = PackProtool.PackData(0x000C,0xffff, "0\r", address)
        return self.sendAndWaitReceive(SendData)

    def deleteCurrentLowAdjustParam(self, address = 0x41):
        '''删除电流(小量程)校准参数'''
        SendData = PackProtool.PackData(0x000C,0xffff, "1\r", address)
        return self.sendAndWaitReceive(SendData)
    def deleteCurrentHighAdjustParam(self, address = 0x41):
        '''删除电流(大量程)校准参数'''
        SendData = PackProtool.PackData(0x000C,0xffff, "2\r", address)
        return self.sendAndWaitReceive(SendData)

    def setDeviceStayInBoot(self, address = 0x41):
        '''控制设备重启后停留在bootload'''
        SendData = PackProtool.PackData(0x00F0,0xffff, '', address)
        self.sendAndNotWaitReceive(SendData)
    def getBootVersion(self, address = 0x41):
        '''读bootlaod程序软件版本号'''
        SendData = PackProtool.PackData(0x0100,0xffff,"", address)
        return self.sendAndWaitReceive(SendData)

    def setBootStartUpdate(self, address = 0x41):
        '''开始升级固件'''
        SendData = PackProtool.PackData(0x0101,0xffff, '', address)
        return self.sendAndWaitReceive(SendData,WaiteTimeSec = 10.0)

    def setBootUpdate(self, param = b'', address = 0x41):
        '''更新固件'''
        SendData = PackProtool.PackData(0x0102,0xffff, param, address)
        return self.sendAndWaitReceive(SendData)
    def setBootEndUpdate(self, address = 0x41):
        '''结束升级固件'''
        SendData = PackProtool.PackData(0x0103,0xffff, '', address)
        return self.sendAndWaitReceive(SendData)
    def setBootSoftReset(self, address = 0x41):
        '''重启设备'''
        SendData = PackProtool.PackData(0x0104,0xffff, '', address)
        self.sendAndNotWaitReceive(SendData)
if __name__ == '__main__':
    pass