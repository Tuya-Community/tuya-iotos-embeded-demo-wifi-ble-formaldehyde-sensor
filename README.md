# Tuya IoTOS Embeded Demo WiFi & BLE Smart Formaldehyde Sensor

[English](./README.md) | [中文](./README_zh.md)


## Introduction 

This Demo implements a smart formaldehyde detector through the Tuya Smart Cloud Platform, Tuya Smart APP, IoTOS Embeded WiFi &Ble SDK, using the Tuya WiFi/WiFi+BLE series modules to achieve remote data monitoring and 1s automatic upload of formaldehyde concentration in the current environment.

Implemented functions include.

+ Real-time monitoring of formaldehyde concentration




## Quick start 

### Compile & Burn 
+ Download [Tuya IoTOS Embeded WiFi & BLE sdk](https://github.com/tuya/tuya-iotos-embeded-sdk-wifi-ble-bk7231t) 

+ Download the demo to the apps directory of the SDK directory 

  ```bash
  $ cd apps
  $ git clone https://github.com/Tuya-Community/tuya-iotos-embeded-demo-wifi-ble-formaldehyde-sensor
  ```
  
+ Start the compilation by executing the following command in the SDK root directory.

  ```bash
  sh build_app.sh apps/tuya-iotos-embeded-demo-wifi-ble-formaldehyde-sensor tuya-iotos-embeded-demo-wifi-ble-formaldehyde-sensor 1.0.0 
  ```

+ Firmware burn-in license information please refer to: [Wi-Fi + BLE series module burn-in license](https://developer.tuya.com/cn/docs/iot/device-development/burn-and-authorization/burn-and- authorize-wifi-ble-modules/burn-and-authorize-wb-series-modules?id=Ka78f4pttsytd) 

 

### File introduction 

```
├── include
│   ├── tuya_ch2o.h
│   ├── tuya_device.h
│   └── tuya_dp_process.h
└── src
    ├── tuya_ch2o.c			//Formaldehyde sensor data processing file
    ├── tuya_device.c		//Application entry file
    └── tuya_dp_process.c	//DataPoint processing file
```



### Demo information 

|      Product ID      |                       wlc9eqpmyhtl303x                       |
| :------------------: | :----------------------------------------------------------: |
|    chip platform     |                           BK7231S                            |
| Compiler Environment | ty_iot_wf_bt_sdk_bk_bk7231t_1.0.2 (Tuya IOT Platform SDK 1.0.2) |
|     Sensor Model     |                          ZE08-CH2O                           |

 

### Demo entry

Entry file: tuya_device.c

Important functions: device_init()

+ Call tuya_iot_wf_soc_dev_init_param() interface to initialize the SDK, configure the operating mode, the mating mode, and register various callback functions and store the firmware key and PID.

+ Calling the tuya_iot_reg_get_wf_nw_stat_cb() interface to register the device network status callback functions.

+ Call the application layer initialization function air_box_device_init()



### DataPoint related

+ Report dp point interface: dev_report_dp_json_async()

| function name | OPERATE_RET dev_report_dp_json_async(IN CONST CHAR_T *dev_id,IN CONST TY_OBJ_DP_S *dp_data,IN CONST UINT_T cnt) |
| ------------- | ------------------------------------------------------------ |
| devid         | device id (if gateway, MCU, SOC class device then devid = NULL; if sub-device, then devid = sub-device_id) |
| dp_data       | dp structure array name                                      |
| cnt           | Number of elements of the dp structure array                 |
| Return        | OPRT_OK: Success Other: Failure                              |

 

### Connection Description 

| CH2O sensor pin | Corresponding wb3s pin |
| :-------------: | :--------------------: |
|       VCC       |          3.3V          |
|       GND       |          GND           |
|       TX        |      RXD1/GPIO_10      |
|       RX        |      TXD1/GPIO_11      |

 

### Sensor usage instructions 

Sensor and SOC serial communication settings.

|  baud rate  |  9600  |
| :---------: | :----: |
|  Data bits  | 8 bits |
|  Stop bit   | 1 bit  |
| check digit |  none  |

 Communication is divided into active upload type (1s send once) and question and answer type.

Switch to active upload.

|   Byte0   |  Byte1   |     Byte2      |     Byte3     |  Byte4   |  Byte5   |  Byte6   |    Byte7    |  Byte8   |
| :-------: | :------: | :------------: | :-----------: | :------: | :------: | :------: | :---------: | :------: |
| Start Bit | Reserved | Toggle Command | Active Upload | Reserved | Reserved | Reserved | Check Digit | Checksum |
|   0xFF    |   0x01   |      0x78      |     0x40      |   0x00   |   0x00   |   0x00   |    0x00     |   0x00   |

 Active upload data display format.

| Byte0 | Byte1 | Byte2 | Byte3 | Byte4 | Byte5 | Byte6 | Byte7 | Byte8 |
| :----: | :------: | :---------: | :---------: | :-----------: | :-----------: | :---------: | :---------: | :---------: |
| Start Bit | Gas Name | Unit (ppb) | Fractional Digits-None | Gas Concentration-High | Gas Concentration-Low | Full Range-High | Full Range-Low | Checksum |
| 0xFF | 0x17 | 0x04 | 0x00 | 0x00 | 0x25 | 0x13 | 0x88 | 0x25 |

The gas concentration value (PPB) = ( gas concentration high bit * 256 + gas concentration low bit ). When switching to PPM: PPM= PPB/1000. 1PPM×1.25 = 1.25mg/m3.   

 

Switching to Q&A mode: 

| Byte0 |  Byte1   |     Byte2      | Byte3 |  Byte4   |  Byte5   |  Byte6   |  Byte7   |  Byte8   |
| :---: | :------: | :------------: | :---: | :------: | :------: | :------: | :------: | :------: |
| Start | Reserved | Toggle Command | Q & A | Reserved | Reserved | Reserved | Reserved | Checksum |
| 0xFF  |   0x01   |      0x78      | 0x41  |   0x00   |   0x00   |   0x00   |   0x00   |   0x46   |

Read concentration command format: 

|   Byte0   |  Byte1   |  Byte2  |  Byte3   |  Byte4   |  Byte5   |  Byte6   |  Byte7   |  Byte8   |
| :-------: | :------: | :-----: | :------: | :------: | :------: | :------: | :------: | :------: |
| Start Bit | Reserved | Command | Reserved | Reserved | Reserved | Reserved | Reserved | Checksum |
|   0xFF    |   0x01   |  0x86   |   0x00   |   0x00   |   0x00   |   0x00   |   0x00   |   0x79   |

The sensor returns the concentration value in the following format: 

 

| Byte0 |  Byte1  |             Byte2             |            Byte3             |  Byte4   |  Byte5   |            Byte6            |           Byte7            |  Byte8   |
| :---: | :-----: | :---------------------------: | :--------------------------: | :------: | :------: | :-------------------------: | :------------------------: | :------: |
| Start | Command | Gas concentration-high(ug/m3) | Gas concentration-low(ug/m3) | Reserved | Reserved | Gas concentration-high(ppb) | Gas concentration-low(ppb) | Checksum |
| 0xFF  |  0x86   |             0x00              |             0x2A             |   0x00   |   0x00   |            0x00             |            0x00            |   0x20   |



Checksum = (Inverse (Byte1+Byte2+......+Byte7)) + 1.

 

## Related Documents

Tuya Demo Center: https://developer.tuya.com/cn/demo/



## Technical Support

You can get support for Tuya by using the following methods:

- Developer Center: https://developer.tuya.com/cn/
- Help Center: https://support.tuya.com/en/help
- Technical Support Work Order Center: [https://service.console.tuya.com](https://service.console.tuya.com/) 