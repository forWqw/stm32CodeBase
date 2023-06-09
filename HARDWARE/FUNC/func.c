//#include <pthread.h>

#include "func.h"
#include "delay.h"
#include "4G.h"
#include "string.h"
#include "log.h"

_deviceStatus g_deviceStatus[DEVICE_NUM_MAX] = {0};

void deviceStatusInit()
{
    int idx = 0;

    //printf("device init begin!\r\n");
    for (idx = 0; idx < DEVICE_NUM_MAX; idx ++)
    {
        memset(&g_deviceStatus[idx], 0, sizeof(_deviceStatus));
        snprintf(g_deviceStatus[idx].name, sizeof(g_deviceStatus[idx].name), DEVICE_NAME, idx);
        g_deviceStatus[idx].num = idx;
        g_deviceStatus[idx].work_status = STOP_STATUS;
        g_deviceStatus[idx].old_work_status = STOP_STATUS;
    }
    //printf("device init end!\r\n");
}

int func_check_name(char *name)
{
    int idx                     = 0;

    for (idx = 0; idx < DEVICE_NUM_MAX; idx++)
    {
        if (!strcmp(g_deviceStatus[idx].name, name))
        {
            return idx;
        }
    }
    return -1;
}

int set_relay(int index, int status)
{
	if(DEVICE_NUM_MAX <= index)
	{
		return -1;
	}
   
	log_printf(DEBUG_INIT, "relay %d %s!\r\n", index, (status == WORK_STATUS) ? "ON": "OFF");
	if(WORK_STATUS == status)
	{
			switch (index)
			{
				case 0:
					GPIO_ResetBits(GPIOB,GPIO_Pin_3);
				break;
				case 1:
					GPIO_ResetBits(GPIOB,GPIO_Pin_4);
					break;
				case 2:
					GPIO_ResetBits(GPIOB,GPIO_Pin_5);
					break;
				case 3:
					break;
				default:
					break;
			}
		}
	else if(STOP_STATUS == status)
	{
		switch (index)
			{
				case 0:
					GPIO_SetBits(GPIOB,GPIO_Pin_3);
				break;
				case 1:
					GPIO_SetBits(GPIOB,GPIO_Pin_4);
					break;
				case 2:
					GPIO_SetBits(GPIOB,GPIO_Pin_5);
					break;
				case 3:
					break;
				default:
					break;
			}
	}
	else
	{
		printf("workStatus is err!\r\n");
		return -1;
	}
	return 0;
}

int do_set(int deviceId, int workStatus)
{
    char strName[8] = {0};
    int intNum = 0;

    memset(strName, 0, sizeof(strName));
    snprintf(strName, sizeof(strName), "device%d", deviceId);
    log_printf(DEBUG_INIT,  "strName=%s,workStatus=%d\r\n", strName, workStatus);
    intNum = func_check_name(strName);
    if (-1 == intNum)
    {
        log_printf(DEBUG_INIT,  "check msg name fail!\r\n");
        return -1;
    }

    if (workStatus > 1 || workStatus < 0)
    {
        log_printf(DEBUG_INIT,  "check msg name fail!\r\n");
        return -1;
    }

    if (workStatus == g_deviceStatus[intNum].work_status)
    {
        log_printf(DEBUG_INIT,  "check status fail!\r\n");
        return -1;
    }

    g_deviceStatus[intNum].work_status = workStatus;

    if (set_relay(intNum, workStatus))
    {
        log_printf(DEBUG_INIT,  "set relay fail\r\n");
        return -1;
    }
    return 0;

}

int do_get()
{
    char deviceStatus[32] = {0};
    int allDevicesStatus = 0;
    int idx = 0;
    memset(deviceStatus, 0, sizeof(deviceStatus));
    for (idx = 0; idx < DEVICE_NUM_MAX; idx ++)
    {
        allDevicesStatus = g_deviceStatus[idx].work_status ? (allDevicesStatus | DEVICES_BITS(idx)) : (allDevicesStatus & ~DEVICES_BITS(idx));
    }
    snprintf(deviceStatus, sizeof(deviceStatus), "%x", allDevicesStatus);
    log_printf(DEBUG_INIT, "get device Status is %d\r\n", allDevicesStatus);
    CSTX_4G_ALYIOTSenddata((u8 *)strlen(deviceStatus), (u8 *)deviceStatus);
    delay_ms(1000);
    return 0;
}

int func_set_work_status(char *get_msg)
{
    char function[16]               = {0};
    char *ptr = NULL;
    char deviceId = -1;
    int workStatus = -1;
    if (!strlen(get_msg))
    {
        log_printf(DEBUG_INIT,  "get msg info fail!\n");
        return -1;
    }
    log_printf(DEBUG_INIT,  "msg = %s\r\n", get_msg);
    memset(function, 0, sizeof(function));
    sscanf(get_msg, "%d_%d", &deviceId, &workStatus);

    if ((-1 > workStatus) && (-1 > deviceId))
    {
        log_printf(DEBUG_INIT,  "get msg fail\n");
    }

    if (do_set(deviceId, workStatus))
    {
        log_printf(DEBUG_INIT,  "do set function fail\n");
    }

    return 0;
}

int deviceFunc(void)
{
    int idx = 0;
    char result[16] = {0};

    for (idx = 0; idx < DEVICE_NUM_MAX; idx ++)
    {
        if ((g_deviceStatus[idx].work_status != g_deviceStatus[idx].old_work_status) && (WORK_STATUS == g_deviceStatus[idx].work_status))
        {
            g_deviceStatus[idx].old_work_status = g_deviceStatus[idx].work_status;
            //memset(result, 0, sizeof(result));
            //snprintf(result, sizeof(result), "%s", "OK");
            //CSTX_4G_ALYIOTSenddata((u8 *)strlen(result), (u8 *)result);
        }
        else
        {
            //do nothing!
        }
    }

    delay_ms(1000);
    return 0;
}
