/**
 * @File: tuya_ch2o.c 
 * @Author: shiliu.yang@tuya.com 
 * @Last Modified time: 2021-01-05 
 * @Description: ch2o sensor demo 
 */
#include "tuya_ch2o.h"
#include "tuya_dp_process.h"
#include "tuya_iot_com_api.h"
#include "uni_thread.h"
#include "tuya_uart.h"
#include "FreeRTOS.h"
#include "uni_log.h"
#include "task.h"

#define BUFSZ   10

#define CH2O_ALARM  62

static UCHAR_T func_check_sum (UCHAR_T *data, UCHAR_T len)
{
    UCHAR_T i, tempq = 0;
    data += 1; //指向data[1]

    for(i=0; i<(len-2); i++)
    {
        tempq += *data;
        data++;
    }

    tempq = (~tempq) + 1;

    return (tempq);
}

VOID CH2O_get_value_task(VOID)
{
    UCHAR_T buff_rev[BUFSZ];
    UINT_T  ret_cnt;

    UCHAR_T data_high, data_low;
    FLOAT_T CH2O_value;
    UCHAR_T check_sums;
    
    while(1) {
    
        tuya_hal_system_sleep(10000);
    
        memset(buff_rev, 0, sizeof(buff_rev));
        check_sums = 0x00;
        
        //ty_read_uart_data_to_buffer(TY_UART1, buff_rev);
        ret_cnt = ty_uart_read_data(TY_UART1, buff_rev, BUFSZ);
        
        check_sums = func_check_sum(buff_rev, 9);

        if(buff_rev[8] != check_sums) {
            PR_ERR("CH2O data check sums error");
            continue;
        }

        if((buff_rev[0] != 0xff) || (buff_rev[1] != 0x17) || (buff_rev[2] != 0x04)) {
            PR_ERR("buff_rev[0]: %02x, buff_rev[1]: %02x, buff_rev[2]: %02x", buff_rev[0], buff_rev[1], buff_rev[2]);
            PR_ERR("receive CH2O data error");
            continue;
        }

        data_high = buff_rev[4];
        data_low = buff_rev[5];

        CH2O_value = (data_high * 256 + data_low);

        PR_NOTICE("CH2O_value is %lf", CH2O_value);

        if (CH2O_value > CH2O_ALARM) {
            CH2O_sensor_status.CH2O_sta = alarm;
        }
        CH2O_sensor_status.CH2O_value = CH2O_value;
        updata_dp_all();
    }
}

VOID CH2O_sensor_init(VOID)
{
    ty_uart_init(TY_UART1, TYU_RATE_9600, TYWL_8B, TYP_NONE, TYS_STOPBIT1, (BUFSZ * SIZEOF(UCHAR_T)), TRUE);
    
    tuya_hal_thread_create(NULL, "CH2O_task", 512, TRD_PRIO_5, CH2O_get_value_task, NULL);
}


