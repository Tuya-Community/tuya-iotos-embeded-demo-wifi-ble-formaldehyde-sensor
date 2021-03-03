# Tuya IoTOS Embeded Demo WiFi & BLE Smart  Formaldehyde Sensor 

[English](./README.md) | [中文](./README_zh.md)

## 简介 

本Demo通过涂鸦智能云平台、涂鸦智能APP、IoTOS Embeded WiFi &Ble SDK实现一款智能甲醛检测器，使用涂鸦WiFi/WiFi+BLE系列模组，实现远程数据监测，1s自动上传一次当前环境下的甲醛浓度。

已实现功能包括：

+ 实时监测甲醛浓度




## 快速上手 

### 编译与烧录 
+ 下载[Tuya IoTOS Embeded WiFi & BLE sdk](https://github.com/tuya/tuya-iotos-embeded-sdk-wifi-ble-bk7231t) 

+ 下载Demo至SDK目录的apps目录下 

  ```bash
  $ cd apps
  $ git clone https://github.com/Tuya-Community/tuya-iotos-embeded-demo-wifi-ble-formaldehyde-sensor
  ```
  
+ 在SDK根目录下执行以下命令开始编译：

  ```bash
  sh build_app.sh apps/tuya-iotos-embeded-demo-wifi-ble-formaldehyde-sensor tuya-iotos-embeded-demo-wifi-ble-formaldehyde-sensor 1.0.0 
  ```

+ 固件烧录授权相关信息请参考：[Wi-Fi + BLE 系列模组烧录授权](https://developer.tuya.com/cn/docs/iot/device-development/burn-and-authorization/burn-and-authorize-wifi-ble-modules/burn-and-authorize-wb-series-modules?id=Ka78f4pttsytd) 

 

### 文件介绍 

```
├── include
│   ├── tuya_ch2o.h
│   ├── tuya_device.h
│   └── tuya_dp_process.h
└── src
    ├── tuya_ch2o.c			//甲醛传感器数据处理文件
    ├── tuya_device.c		//应用层入口文件
    └── tuya_dp_process.c	//dp点处理文件
```



### Demo信息 

|   产品ID   |                      wlc9eqpmyhtl303x                      |
| :--------: | :--------------------------------------------------------: |
|  芯片平台  |                          BK7231S                           |
|  编译环境  | ty_iot_wf_bt_sdk_bk_bk7231t_1.0.2（涂鸦IOT平台 SDK 1.0.2） |
| 传感器型号 |                         ZE08-CH2O                          |

 

### Demo入口

入口文件：tuya_device.c

重要函数：device_init()

+ 调用 tuya_iot_wf_soc_dev_init_param() 接口进行SDK初始化，配置了工作模式、配网模式，同时注册了各种回调函数并存入了固件key和PID。

+ 调用 tuya_iot_reg_get_wf_nw_stat_cb() 接口注册设备网络状态回调函数。

+ 调用应用层初始化函数 air_box_device_init()



### DP点相关

+ 上报dp点接口: dev_report_dp_json_async()

| 函数名  | OPERATE_RET dev_report_dp_json_async(IN CONST CHAR_T *dev_id,IN CONST TY_OBJ_DP_S *dp_data,IN CONST UINT_T cnt) |
| ------- | ------------------------------------------------------------ |
| devid   | 设备id（若为网关、MCU、SOC类设备则devid = NULL;若为子设备，则devid = sub-device_id) |
| dp_data | dp结构体数组名                                               |
| cnt     | dp结构体数组的元素个数                                       |
| Return  | OPRT_OK: 成功  Other: 失败                                   |

 

### 连线说明 

| 甲醛传感器引脚 | 对应连接的wb3s引脚 |
| :------------: | :----------------: |
|      VCC       |        3.3V        |
|      GND       |        GND         |
|       TX       |    RXD1/GPIO_10    |
|       RX       |    TXD1/GPIO_11    |

 

### 传感器使用说明 

传感器和 SOC 串口通信设置：

| 波特率 | 9600 |
| ------ | ---- |
| 数据位 | 8位  |
| 停止位 | 1位  |
| 校验位 | 无   |

通信分为主动上传式（1s 发送一次）和问答式 ：

切换为 主动上传：

| Byte0  | Byte1 |  Byte2   |  Byte3   | Byte4 | Byte5 | Byte6 | Byte7 | Byte8  |
| :----: | :---: | :------: | :------: | :---: | :---: | :---: | :---: | :----: |
| 起始位 | 保留  | 切换命令 | 主动上传 | 保留  | 保留  | 保留  | 保留  | 校验值 |
|  0xFF  | 0x01  |   0x78   |   0x40   | 0x00  | 0x00  | 0x00  | 0x00  |  0x47  |

 主动上传数据显示格式 ：

| Byte0  |  Byte1   |    Byte2    |    Byte3    |     Byte4     |     Byte5     |    Byte6    |    Byte7    | Byte8  |
| :----: | :------: | :---------: | :---------: | :-----------: | :-----------: | :---------: | :---------: | :----: |
| 起始位 | 气体名称 | 单位（ppb） | 小数位数-无 | 气体浓度-高位 | 气体浓度-低位 | 满量程-高位 | 满量程-低位 | 校验值 |
|  0xFF  |   0x17   |    0x04     |    0x00     |     0x00      |     0x25      |    0x13     |    0x88     |  0x25  |

气 体 浓 度 值 (PPB)=( 气 体 浓 度 高 位 *256+ 气 体 浓 度 低 位 ). 当 转 换 为 PPM 时 ：   PPM= PPB/1000.    1PPM× 1.25 = 1.25mg/m3.   

 

切换为问答模式：

| Byte0  | Byte1 |  Byte2   | Byte3 | Byte4 | Byte5 | Byte6 | Byte7 | Byte8  |
| :----: | :---: | :------: | :---: | :---: | :---: | :---: | :---: | :----: |
| 起始位 | 保留  | 切换命令 | 问答  | 保留  | 保留  | 保留  | 保留  | 校验值 |
|  0xFF  | 0x01  |   0x78   | 0x41  | 0x00  | 0x00  | 0x00  | 0x00  |  0x46  |

读取浓度命令格式： 

| Byte0  | Byte1 | Byte2 | Byte3 | Byte4 | Byte5 | Byte6 | Byte7 | Byte8  |
| :----: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :----: |
| 起始位 | 保留  | 命令  | 保留  | 保留  | 保留  | 保留  | 保留  | 校验值 |
|  0xFF  | 0x01  | 0x86  | 0x00  | 0x00  | 0x00  | 0x00  | 0x00  |  0x79  |

传感器返回浓度值显示格式：

| Byte0  | Byte1 |        Byte2         |        Byte3         | Byte4 | Byte5 |       Byte6        |       Byte7        | Byte8  |
| :----: | :---: | :------------------: | :------------------: | :---: | :---: | :----------------: | :----------------: | :----: |
| 起始位 | 命令  | 气体浓度高位（ug/m3) | 气体浓度低位（ug/m3) | 保留  | 保留  | 气体浓度-高位(ppb) | 气体浓度-低位(ppb) | 校验值 |
|  0xFF  | 0x86  |         0x00         |         0x2A         | 0x00  | 0x00  |        0x00        |        0x20        |  0x30  |

 

校验和 = （取反（Byte1+Byte2+……+Byte7）） + 1.  

 

## 相关文档

涂鸦Demo中心：https://developer.tuya.com/cn/demo/



## 技术支持

您可以通过以下方法获得涂鸦的支持:

- 开发者中心：https://developer.tuya.com/cn/
- 帮助中心: https://support.tuya.com/en/help
- 技术支持工单中心: [https://service.console.tuya.com](https://service.console.tuya.com/)