#ifndef __BC26_H
#define __BC26_H	
#include "usart.h"
#include <stm32f10x.h>
#include "delay.h"
void Clear_Buffer(void);//清空缓存	
void CSTX_4G_Init(void);
void CSTX_4G_PDPACT(void);
void CSTX_4G_ConTCP(void);
void CSTX_4G_CreateTCPSokcet(void);
void CSTX_4G_Senddata(int len,uint8_t *data);
void CSTX_4G_CreateSokcet(void);
void Clear_Buffer(void);
void CSTX_4G_ChecekConStatus(void);
void CSTX_4G_RECTCPData(void);
void CSTX_4G_RegALIYUNIOT(void);//平台注册
void CSTX_4G_ALYIOTSenddata(u8 *len,u8 *data);//上发数据，上发的数据跟对应的插件有关系，用户需要注意插件然后对应数据即可
void CSTX_4G_ALYIOTSenddata1(u8 *len,u8 *data);//上发数据，上发的数据跟对应的插件有关系，用户需要注意插件然后对应数据即可
char* Get_4GIMEI_NUM(void); //获取IMEI号
typedef struct
{
    uint8_t CSQ;    
    uint8_t Socketnum;   //编号
    uint8_t reclen;   //获取到数据的长度
    uint8_t res;      
    uint8_t recdatalen[10];
    uint8_t recdata[100];
    uint8_t netstatus;//网络指示灯
} CSTX_4G;

#endif







