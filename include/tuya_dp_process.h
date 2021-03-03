/**
 * @File: tuya_dp_process.h 
 * @Author: shiliu.yang@tuya.com 
 * @Last Modified time: 2021-01-05 
 * @Description: ch2o sensor demo 
 */
#ifndef __TUYA_DP_PROCESS_H_
#define __TUYA_DP_PROCESS_H_

#include "tuya_cloud_com_defs.h"
#include "tuya_cloud_types.h"

typedef enum {
    alarm = 0,
    normal
}CH2O_STA_E;

//设备状态结构体
typedef struct {
    CH2O_STA_E CH2O_sta; 
    UINT_T CH2O_value;
}CH2O_SENSOR_S;


extern CH2O_SENSOR_S  CH2O_sensor_status;

VOID updata_dp_all(VOID);
VOID dp_process(IN CONST TY_OBJ_DP_S *root);


#endif

