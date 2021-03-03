/**
 * @File: tuya_device.c 
 * @Author: shiliu.yang@tuya.com 
 * @Last Modified time: 2021-01-05 
 * @Description: ch2o sensor demo 
 *
 */
#define _TUYA_DEVICE_GLOBAL

#include "tuya_device.h"
#include "tuya_iot_wifi_api.h"
#include "tuya_hal_system.h"
#include "tuya_iot_com_api.h"
#include "tuya_cloud_com_defs.h"
#include "tuya_uart.h"
#include "BkDriverUart.h"
#include "gw_intf.h"
#include "mem_pool.h"
#include "uni_log.h"
#include "gpio_test.h"

#include "tuya_dp_process.h"
#include "tuya_ch2o.h"


/*************************variable define*******************/
//#define ENABLE_UART_1_PRINT 

/*************************function define*******************/

STATIC VOID get_free_heap(VOID)
{
    INT_T free_size = tuya_hal_system_getheapsize();
    PR_NOTICE("device_init is OK!free_size_heap:%d",free_size);
}
/***********************************************************
*   Function:  gpio_test
*   Input:     none
*   Output:    none
*   Return:    none
*   Notice:    gpio测试
***********************************************************/
BOOL_T gpio_test(IN CONST CHAR_T *in, OUT CHAR_T *out)
{
    return gpio_test_all(in, out);
}

/***********************************************************
*   Function:  mf_user_callback
*   Input:     none
*   Output:    none
*   Return:    none
*   Notice:    产测授权回调函数
***********************************************************/
VOID mf_user_callback(VOID)
{
    hw_reset_flash_data();
    return;
}

VOID prod_test(BOOL_T flag,CHAR_T rssi)
{
    OPERATE_RET op_ret;
    PR_NOTICE("have scaned the ssid, and enter product test.rssi value:%d",rssi);
    if(rssi < -60) {
        PR_ERR("no authorization, product test err!");
        return;
    }
    if(FALSE == flag) {
        PR_ERR("current rssi value is less than -60 dB, product test err!");
        return;
    }
}

#ifdef ENABLE_UART_1_PRINT

//切换日志打印串口到 uart1 
#define TY_UART TY_UART1
OPERATE_RET ty_uart_init_mk(IN CONST TY_UART_PORT_E port,IN CONST TY_UART_BAUD_E badu,IN CONST TY_DATA_BIT_E bits, \
                            IN CONST TY_PARITY_E parity,IN CONST TY_STOPBITS_E stop)
{
    bk_uart_config_t config;

    config.baud_rate = badu;
    config.data_width = bits;
    config.parity = parity;    //0:no parity,1:odd,2:even
    config.stop_bits = stop;   //0:1bit,1:2bit
    config.flow_control = 0;   //FLOW_CTRL_DISABLED
    config.flags = 0;

    bk_uart_initialize(port, &config, NULL);
    //bk_uart_set_rx_callback(port, ty_read_uart_data_to_buffer, NULL);
}

#endif

VOID app_init(VOID)
{
    OPERATE_RET op_ret = OPRT_OK;
    
#ifdef ENABLE_UART_1_PRINT
    /* 设置 uart1 为日志打印口*/
    ty_set_log_port(LOG_PORT1);
    ty_uart_init_mk(TY_UART, TYU_RATE_115200, TYWL_8B, TYP_NONE, TYS_STOPBIT1);
#endif

    app_cfg_set(GWCM_LOW_POWER,prod_test);
    PR_DEBUG("wifi_mode:%d",GWCM_LOW_POWER);

}

/***********************************************************
*   Function:  pre_device_init
*   Input:     none
*   Output:    none
*   Return:    none
*   Notice:    打印信息等级和固件基本信息
***********************************************************/
VOID pre_device_init(VOID)
{
    PR_DEBUG("%s",tuya_iot_get_sdk_info());
    PR_DEBUG("%s:%s",APP_BIN_NAME,DEV_SW_VERSION);
    PR_NOTICE("firmware compiled at %s %s", __DATE__, __TIME__);
    SetLogManageAttr(TY_LOG_LEVEL_INFO);
}

/***********************************************************
*   Function: status_changed_cb
*   Input:    status : wifi acticed state
*   Output:   none
*   Return:   none
*   Notice: wifi acticed state changed call back
***********************************************************/
VOID status_changed_cb(IN CONST GW_STATUS_E status)
{
    PR_NOTICE("status_changed_cb is status:%d",status);

    if(GW_NORMAL == status) {
        updata_dp_all();
    }else if(GW_RESET == status) {
        PR_NOTICE("status is GW_RESET");
    }
}

/***********************************************************
*   Function: upgrade_notify_cb
*   Input: 
*   Output: 
*   Return: 
***********************************************************/
VOID upgrade_notify_cb(IN CONST FW_UG_S *fw, IN CONST INT_T download_result, IN PVOID_T pri_data)
{
    PR_DEBUG("download  Finish");
    PR_DEBUG("download_result:%d", download_result);
}

/***********************************************************
*  Function: get_file_data_cb
*  Input: 
*  Output: 
*  Return: 
***********************************************************/
OPERATE_RET get_file_data_cb(IN CONST FW_UG_S *fw, IN CONST UINT_T total_len, IN CONST UINT_T offset,
                                     IN CONST BYTE_T *data, IN CONST UINT_T len, OUT UINT_T *remain_len, IN PVOID_T pri_data)
{
    PR_DEBUG("Rev File Data");
    PR_DEBUG("Total_len:%d ", total_len);
    PR_DEBUG("Offset:%d Len:%d", offset, len);

    return OPRT_OK;
}

/***********************************************************
*   Function: gw_ug_inform_cb
*   Input: 
*   Output: 
*   Return: 
***********************************************************/
VOID gw_ug_inform_cb(IN CONST FW_UG_S *fw)
{
    PR_DEBUG("Rev GW Upgrade Info");
    PR_DEBUG("fw->fw_url:%s", fw->fw_url);
   // PR_DEBUG("fw->fw_md5:%s", fw->fw_md5);
    PR_DEBUG("fw->sw_ver:%s", fw->sw_ver);
    PR_DEBUG("fw->file_size:%d", fw->file_size);

    tuya_iot_upgrade_gw(fw, get_file_data_cb, upgrade_notify_cb, NULL);
}

/***********************************************************
*   Function:  hw_reset_flash_data
*   Input:     none
*   Output:    none
*   Return:    OPERATE_RET
*   Notice:    GW_REMOTE_RESET_FACTORY by app panel control
***********************************************************/
VOID hw_reset_flash_data(VOID)
{
    //app 上移除设备后需要进行处理的 
}

/***********************************************************
*   Function:  gw_reset_cb
*   Input:     none
*   Output:    none
*   Return:    OPERATE_RET
*   Notice:    GW_REMOTE_RESET_FACTORY by app panel control
***********************************************************/
VOID gw_reset_cb(IN CONST GW_RESET_TYPE_E type)
{
    PR_DEBUG("gw_reset_cb type:%d",type);
    if(GW_REMOTE_RESET_FACTORY != type) {
        PR_DEBUG("type is GW_REMOTE_RESET_FACTORY");
        return;
    }

    hw_reset_flash_data();
    return;
}

/***********************************************************
*   Function:  dev_obj_dp_cb
*   Input:     *dp :the data need to analyse point
*   Output:    return frame
*   Return:    none
*   Notice:    dp data analysis for object data, send return frame
***********************************************************/
VOID dev_obj_dp_cb(IN CONST TY_RECV_OBJ_DP_S *dp)
{
    PR_DEBUG("dp->cid:%s dp->dps_cnt:%d",dp->cid,dp->dps_cnt);
    UCHAR_T i = 0;

    for(i = 0;i < dp->dps_cnt;i++) {
        dp_process(&(dp->dps[i]));
    }
}

/***********************************************************
*   Function:  dev_raw_dp_cb
*   Input:     *dp :the data need to analyse point
*   Output:    return frame
*   Return:    none
*   Notice:    dp data analysis for raw data, send return frame
***********************************************************/
VOID dev_raw_dp_cb(IN CONST TY_RECV_RAW_DP_S *dp)
{
    PR_DEBUG("raw data dpid:%d",dp->dpid);

    PR_DEBUG("recv len:%d",dp->len);
#if 0
    INT_T i = 0;
    
    for(i = 0;i < dp->len;i++) {
        PR_DEBUG_RAW("%02X ",dp->data[i]);
    }
#endif
    PR_DEBUG_RAW("\n");
    PR_DEBUG("end");
}

/***********************************************************
*   Function: dev_dp_query_cb
*   Input:    dp_qry: received query cmd
*   Output:   none
*   Return:   none
*   Notice:   device query callback
***********************************************************/
STATIC VOID dev_dp_query_cb(IN CONST TY_DP_QUERY_S *dp_qry)
{
    PR_NOTICE("Recv DP Query Cmd");
    updata_dp_all();
}

/***********************************************************
*   Function: wf_nw_status_cb
*   Input:    GW_WIFI_NW_STAT_E: wifi状态
*   Output:   none
*   Return:   none
*   Notice:   WiFi状态回调函数
***********************************************************/
VOID wf_nw_status_cb(IN CONST GW_WIFI_NW_STAT_E stat)
{
    PR_NOTICE("wf_nw_status_cb,wifi_status:%d",stat);
    if(stat == STAT_AP_STA_CONN || stat >= STAT_STA_CONN) {
        updata_dp_all();
    }
}

OPERATE_RET device_init(VOID)
{
    OPERATE_RET op_ret = OPRT_OK;

    TY_IOT_CBS_S wf_cbs = {
        status_changed_cb,\
        gw_ug_inform_cb,\
        gw_reset_cb,\
        dev_obj_dp_cb,\
        dev_raw_dp_cb,\
        dev_dp_query_cb,\
        NULL,
    };

    //进入配网模式
    op_ret = tuya_iot_wf_soc_dev_init_param(GWCM_OLD, WF_START_SMART_FIRST,&wf_cbs,NULL,PRODECT_KEY,DEV_SW_VERSION);
    if(OPRT_OK != op_ret) {
        PR_ERR("tuya_iot_wf_soc_dev_init_param error,err_num:%d",op_ret);
        return op_ret;
    }

    op_ret = tuya_iot_reg_get_wf_nw_stat_cb(wf_nw_status_cb);
    if(OPRT_OK != op_ret) {
        PR_ERR("tuya_iot_reg_get_wf_nw_stat_cb is error,err_num:%d",op_ret);
        return op_ret;
    }

    CH2O_sensor_init();
    return op_ret;
}
