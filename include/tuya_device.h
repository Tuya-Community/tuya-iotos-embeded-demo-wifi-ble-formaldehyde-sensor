/**
 * @File: tuya_device.h 
 * @Author: shiliu.yang@tuya.com 
 * @Last Modified time: 2021-01-05 
 * @Description: ch2o sensor demo 
 */

#ifndef __TUYA_DEVICE_H__
#define __TUYA_DEVICE_H__
    
#include "tuya_cloud_error_code.h"
    
#ifdef __cplusplus
    extern "C" {
#endif
    
#ifdef _TUYA_DEVICE_GLOBAL
    #define _TUYA_DEVICE_EXT 
#else
    #define _TUYA_DEVICE_EXT extern
#endif
    
/*************************micro define**********************/
// device information define
#define DEV_SW_VERSION USER_SW_VER
#define PRODECT_KEY "wlc9eqpmyhtl303x"

/*************************function define*******************/

/***********************************************************
*  Function: device_init 
*  Input: none
*  Output: none
*  Return: none
***********************************************************/
_TUYA_DEVICE_EXT \
OPERATE_RET device_init(VOID);

#ifdef __cplusplus
}
#endif
#endif

