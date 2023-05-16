#include "4G.h"
#include "string.h"
#include "usart.h"
#include "wdg.h"
#include "led.h"
///////////////下面是液晶屏头文件/////////////////////
#include "Lcd_Driver.h"
#include "GUI.h"
#include "delay.h"
//#include "Picture.h"
#include "QDTFT_demo.h"
#include "func.h"
/////////////////////////////////////////////////////
char *strx,*extstrx;

CSTX_4G CSTX_4G_Status;	//模块的状态信息
int  errcount=0;	//发送命令失败次数 防止死循环
int  errCountData=0;
char ATSTR[BUFLEN];	//组建AT命令的函数
char IMEINUMBER[BUFLEN];//+CGSN: "869523052178994"
////////////////////////只要修改三要素/////////////////////////////////
/*
#define PRODUCEKEY "a1RTeaNN74s"        //修改产品秘钥
#define DEVICENAME "CAT1GNSS"           //修改设备名称
#define DEVICESECRET "ec8f743ccdcd4edb1f2090918e7165a7" //修改设备秘钥
*/
///////////////////////////////////////////////////////////////////////
/*
#define PRODUCEKEY "ict71NcvRb1"        //修改产品秘钥
#define DEVICENAME "test"           //修改设备名称
#define DEVICESECRET "3011abb39a869cdf7d29d9fbaac39754" //修改设备秘钥
*/

#define PRODUCEKEY 		"ict71NcvRb1"        //修改产品秘钥
#define DEVICENAME 		"test"           //修改设备名称
#define DEVICESECRET 	"3011abb39a869cdf7d29d9fbaac39754" //修改设备秘钥
#define MQTTHOSTURL 	"iot-06z00daa2jr8gpj.mqtt.iothub.aliyuncs.com"
#define MQTTHOSTPORT 	1883


/*****************************************************
清空模块反馈的信息
*****************************************************/
void Clear_Buffer(void)//清空缓存
{
		printf(buf_uart2.buf);
	
		strx=strstr((const char*)buf_uart2.buf,(const char*)"+QMTRECV:");//返回+QIURC:，表明接收到TCP服务器发回的数据
    if(strx)
    {
				Gui_DrawFont_GBK16(16,10,RED,WHITE, "RECEIVE DATA");      
    }
		
    delay_ms(300);
    buf_uart2.index=0;
    memset(buf_uart2.buf,0,BUFLEN);
		IWDG_Feed();//喂狗
	
}


/*****************************************************
初始化模块 和单片机连接，获取卡号和信号质量
*****************************************************/
void CSTX_4G_Init(void)
{
		//打印初始化信息
		printf("start init EC200X\r\n");
		//发第一个命令ATE1
    Uart2_SendStr("ATE1\r\n"); 
    delay_ms(300);
		printf(buf_uart2.buf);      //打印串口收到的信息
    strx=strstr((const char*)buf_uart2.buf,(const char*)"OK");//返回OK
    Clear_Buffer();	
    while(strx==NULL)
    {
				//Gui_DrawFont_GBK16(16,70,RED,WHITE, "全程技术支持");
				Gui_DrawFont_GBK16(16,10,RED,WHITE, "CONNECT 4G..");
				printf("单片机正在连接模块......\r\n");
        Clear_Buffer();	
        Uart2_SendStr("ATE1\r\n"); 
        delay_ms(300);
        strx=strstr((const char*)buf_uart2.buf,(const char*)"OK");//返回OK
    }
	  Gui_DrawFont_GBK16(16,10,RED,WHITE, "CONNECT [OK]");
		printf("****单片机和模块连接成功*****\r\n");
		Uart2_SendStr("ATI\r\n");//获取模块的版本
		delay_ms(300);
		Clear_Buffer();	
		
    Uart2_SendStr("AT+CIMI\r\n");//获取卡号，类似是否存在卡的意思，比较重要。
    delay_ms(300);
    strx=strstr((const char*)buf_uart2.buf,(const char*)"460");//返460，表明识别到卡了
    while(strx==NULL)
    {
        Clear_Buffer();	
        Uart2_SendStr("AT+CIMI\r\n");//获取卡号，类似是否存在卡的意思，比较重要。
        delay_ms(300);
        strx=strstr((const char*)buf_uart2.buf,(const char*)"460");//返回OK,说明卡是存在的
    } 
		printf("我的卡号是 : %s \r\n",buf_uart2.buf+8);
		Clear_Buffer();	
		
	  Gui_DrawFont_GBK16(16,10,RED,WHITE, "SIMCARD [OK]");
		
		Uart2_SendStr("AT+CGATT?\r\n");//查询激活状态
		delay_ms(300);
		strx=strstr((const char*)buf_uart2.buf,(const char*)"+CGATT: 1");//返1
		Clear_Buffer();	
		while(strx==NULL)
		{
				Clear_Buffer();	
				Uart2_SendStr("AT+CGATT?\r\n");//获取激活状态
				delay_ms(300);
				strx=strstr((const char*)buf_uart2.buf,(const char*)"+CGATT: 1");//返回1,表明注网成功
		}
		
	  Gui_DrawFont_GBK16(16,10,RED,WHITE, "REGISTER[OK]");
		
		
		Clear_Buffer();	
		Uart2_SendStr("AT+CSQ\r\n");//查看获取CSQ值
		delay_ms(300);
    strx=strstr((const char*)buf_uart2.buf,(const char*)"+CSQ:");//返回CSQ
		if(strx)
		{
				
				printf("信号质量是:%s 注意：信号最大值是31 \r\n",buf_uart2.buf+14);      
		}
		IWDG_Feed();//喂狗
}



/*****************************************************
关闭之前存在的和服务器的链接 可能反馈失败 
*****************************************************/
void CSTX_4G_ConTCP(void)
{		
		//关闭之前建立的链接
		Uart2_SendStr("AT+QICLOSE=0\r\n");//关闭socekt连接
		delay_ms(100);
		Uart2_SendStr("AT+QICLOSE=1\r\n");//关闭socekt连接
		delay_ms(100);
		Uart2_SendStr("AT+QICLOSE=2\r\n");//关闭socekt连接
		delay_ms(100);
	
    Clear_Buffer();
    IWDG_Feed();//喂狗
}

/*****************************************************
下面就是需要修改的地方，修改服务器的IP地址和端口号
*****************************************************/
#define SERVERIP "101.200.212.234"
#define SERVERPORT "1001"
/*****************************************************
建立TCP链接 
*****************************************************/
void CSTX_4G_CreateTCPSokcet(void)//创建sokcet
{
		Gui_DrawFont_GBK16(0,30,BLUE,WHITE,SERVERIP);
		Gui_DrawFont_GBK16(0,50,BLUE,WHITE,"SERVER PORT");
		Gui_DrawFont_GBK16(88,50,BLUE,WHITE,SERVERPORT);	//显示IP地址和端口号 中文是16个像素 英文是8个像素
		//Gui_DrawFont_GBK16(16,10,RED,WHITE, "SERVER  [OK]");
		Gui_DrawFont_GBK16(16,10,RED,WHITE, "SERVER  [..]");
		memset(ATSTR,0,BUFLEN);
		sprintf(ATSTR,"AT+QIOPEN=1,0,\"TCP\",\"%s\",%s,0,1\r\n",SERVERIP,SERVERPORT);
    Uart2_SendStr(ATSTR);//创建连接TCP,输入IP以及服务器端口号码 
    delay_ms(300);
	
		strx=strstr((const char*)buf_uart2.buf,(const char*)"+QIOPEN: 0,566");//检查是否登陆成功
		if(strx)
		{
			 Gui_DrawFont_GBK16(16,10,RED,WHITE, "SERVER  [NO]");	//链接服务器失败
			 return ;	//如果连接服务器失败就反馈 后面不需要判断是否成功了
		}
	
    strx=strstr((const char*)buf_uart2.buf,(const char*)"+QIOPEN: 0,0");//检查是否登陆成功
	  errcount=0;
		while(strx==NULL)
		{
				errcount++;
				strx=strstr((const char*)buf_uart2.buf,(const char*)"+QIOPEN: 0,0");//检查是否登陆成功
				delay_ms(100);
				if(errcount>100)     //超时退出死循环 表示服务器连接失败
        {
            errcount = 0;
            break;
        }
		}  
	  Gui_DrawFont_GBK16(16,10,RED,WHITE, "SERVER  [OK]");
     Clear_Buffer();	
    
}

/*****************************************************
发送数据函数
*****************************************************/
void CSTX_4G_Senddata(int len,uint8_t *data)//发送字符串数据
{
	  Gui_DrawFont_GBK16(16,10,RED,WHITE, "SEND DATA...");
		memset(ATSTR,0,BUFLEN);
		sprintf(ATSTR,"AT+QISEND=0,%d\r\n",len);
    Uart2_SendStr(ATSTR);
    delay_ms(300);
		//等待模块反馈 >
		strx=strstr((const char*)buf_uart2.buf,(const char*)">");//模块反馈可以发送数据了
    while(strx==NULL)
    {
        errcount++;
        strx=strstr((const char*)buf_uart2.buf,(const char*)">");//模块反馈可以发送数据了
        if(errcount>100)     //防止死循环跳出
        {
            errcount = 0;
            break;
        }
    }
		
		Uart2_SendStr((char *)data);//发送真正的数据
    delay_ms(300);
		
		strx=strstr((const char*)buf_uart2.buf,(const char*)"ERROR");//如果发送失败
		if(strx)
		{
				errCountData++;
				Gui_DrawFont_GBK16(16,10,RED,WHITE, "SEND DATA NO");
				if(errCountData>3)     //超时退出死循环 表示服务器连接失败
        {
							__set_FAULTMASK(1);//关闭总中断
							NVIC_SystemReset();//请求单片机重启
        }
				return ;	//发送数据失败了就不要去下面判断是否成功了
		}
		
    strx=strstr((const char*)buf_uart2.buf,(const char*)"SEND OK");//检查是否发送成功
		errcount=0;
		while(strx==NULL)	//如果没有收到SEND OK就循环查询 
		{
				errcount++;
				strx=strstr((const char*)buf_uart2.buf,(const char*)"SEND OK");//检查是否发送成功
				delay_ms(100);
				if(errcount>100)     //超时退出死循环 表示服务器连接失败
        {
            errcount = 0;
            break;
        }
		}  
    Clear_Buffer();	//发送完毕清空
	  Gui_DrawFont_GBK16(16,10,RED,WHITE, "SEND DATA OK");
}


/*****************************************************
收到服务器下发的数据就直接打印
*****************************************************/
//+QMTRECV: 0,0,"/ict71NcvRb1/test/user/wwzt","hongganji1 on"
void CSTX_4G_RECTCPData(void)
{
		char get_msg[256] = {0};
		char result[8] = {0};
    strx=strstr((const char*)buf_uart2.buf,(const char*)"+QMTRECV:");//receive "+QMTRECV: 0,0,:",jieshoushuju
    if(strx)
    {  
				memset(get_msg, 0, sizeof(get_msg));
				sscanf(buf_uart2.buf, "\r\n+QMTRECV: 0,0,\"/ict71NcvRb1/test/user/get\",\"%[^\"]", &get_msg);
				printf("get msg is %s\r\n", get_msg);
				Clear_Buffer();
				Gui_DrawFont_GBK16(16,10,RED,WHITE, "RECEIVE DATA"); 
				
				memset(result, 0, sizeof(result));
				snprintf(result, sizeof(result), "%s", func_set_work_status(get_msg)?"Fail":"OK");
				CSTX_4G_ALYIOTSenddata((u8 *)strlen(result), (u8 *)result);
    }
}

/*****************************************************
注册到阿里云物联网平台
*****************************************************/
void CSTX_4G_RegALIYUNIOT(void)//平台注册
{
		int errcount 	= 0;
		int reg_ok		= 0;
		int time_idx 	= 0;

		Uart2_SendStr("AT+QMTDISC=0\r\n");//Disconnect a client from MQTT server
    delay_ms(300);
    Clear_Buffer();

    Uart2_SendStr("AT+QMTCLOSE=0\r\n");//删除句柄
    delay_ms(300);
    Clear_Buffer();

    memset(ATSTR,0,BUFLEN);
    sprintf(ATSTR,"AT+QMTCFG=\"ALIAUTH\",0,\"%s\",\"%s\",\"%s\"\r\n",PRODUCEKEY,DEVICENAME,DEVICESECRET);
    printf("ATSTR = %s \r\n",ATSTR);
    Uart2_SendStr(ATSTR);//发送阿里云配置参数
    delay_ms(300);  //等待300ms反馈OK
    strx=strstr((const char*)buf_uart2.buf,(const char*)"OK");//返OK
    while(strx==NULL)
    {
        strx=strstr((const char*)buf_uart2.buf,(const char*)"OK");//返OK
    }
    Clear_Buffer();
		
		for(time_idx = 0; time_idx < 3 && !reg_ok; time_idx++)
		{
				errcount = 0;
				memset(ATSTR, 0, BUFLEN);
				snprintf(ATSTR, BUFLEN, "AT+QMTOPEN=0,\"%s\",%d\r\n", MQTTHOSTURL, MQTTHOSTPORT);
				printf("ATSTR = %s \r\n",ATSTR);
				Uart2_SendStr(ATSTR);//登录阿里云平台
				delay_ms(300);
				strx=strstr((const char*)buf_uart2.buf,(const char*)"+QMTOPEN: 0,0");//返+QMTOPEN: 0,0
				while((NULL == strx) && (errcount <= 10000))
				{
					errcount++;
					delay_ms(30);
					strx=strstr((const char*)buf_uart2.buf, (const char*)"+QMTOPEN: 0,0");
					if(10000 == errcount)
					{
							GPIO_SetBits(GPIOA,GPIO_Pin_12); 	
							delay_ms(1000);
							GPIO_ResetBits(GPIOA,GPIO_Pin_12); 	 
							delay_ms(300);
							NVIC_SystemReset();	//没有创建TCP SOCKET就重启系统等到服务器就绪
							continue;
					}
				}
				Clear_Buffer();
				
				memset(ATSTR,0,BUFLEN);
				sprintf(ATSTR,"AT+QMTCONN=0,\"%s\"\r\n",DEVICENAME);
				printf("ATSTR = %s \r\n",ATSTR);
				Uart2_SendStr(ATSTR);//发送链接到阿里云
				delay_ms(300);
				strx=strstr((const char*)buf_uart2.buf,(const char*)"+QMTCONN: 0,0,0");//返+QMTCONN: 0,0,0
				while((strx==NULL) && (errcount <= 10000))
				{
						errcount++;
						delay_ms(30);
						strx=strstr((const char*)buf_uart2.buf,(const char*)"+QMTCONN: 0,0,0");//返+QMTCONN: 0,0,0
						
						if(10000 == errcount)
						{
								continue;
						}
				}
				Clear_Buffer();
				reg_ok = 1;
			}
}

void CSTX_4G_ALYIOTSenddata(u8 *len,u8 *data)//上发数据，上发的数据跟对应的插件有关系，用户需要注意插件然后对应数据即可
{
		Gui_DrawFont_GBK16(16,10,RED,WHITE, "SEND DATA...");
    memset(ATSTR,0,BUFLEN);
    sprintf(ATSTR,"AT+QMTPUB=0,1,1,0,\"/sys/%s/%s/thing/event/property/post\",\"{params:{CurrentDeviceStatus:%s}}\"\r\n",PRODUCEKEY,DEVICENAME,data);
    printf("ATSTR = %s \r\n",ATSTR);
    Uart2_SendStr(ATSTR);//输入IMEI,LWM2M协议要求填入IMEI信息
    delay_ms(300);
    strx=strstr((const char*)buf_uart2.buf,(const char*)"+QMTPUB: 0,1,0");//返SEND OK
    while(strx==NULL)
    {
        strx=strstr((const char*)buf_uart2.buf,(const char*)"+QMTPUB: 0,1,0");//返SEND OK
    }
    Clear_Buffer();
		Gui_DrawFont_GBK16(16,10,RED,WHITE, "SEND DATA OK");
}

void CSTX_4G_ALYIOTSenddata1(u8 *len,u8 *data)//上发数据，上发的数据跟对应的插件有关系，用户需要注意插件然后对应数据即可
{
		Gui_DrawFont_GBK16(16,10,RED,WHITE, "SEND DATA...");
    memset(ATSTR,0,BUFLEN);
    sprintf(ATSTR,"AT+QMTPUB=0,1,1,0,\"/sys/%s/%s/thing/event/property/post\",\"{params:{CurrentTemperature:%s}}\"\r\n",PRODUCEKEY,DEVICENAME,data);
    printf("ATSTR = %s \r\n",ATSTR);
    Uart2_SendStr(ATSTR);//输入IMEI,LWM2M协议要求填入IMEI信息
    delay_ms(300);
    strx=strstr((const char*)buf_uart2.buf,(const char*)"+QMTPUB: 0,1,0");//返SEND OK
    while(strx==NULL)
    {
        strx=strstr((const char*)buf_uart2.buf,(const char*)"+QMTPUB: 0,1,0");//返SEND OK
    }
    Clear_Buffer();
		Gui_DrawFont_GBK16(16,10,RED,WHITE, "SEND DATA OK");
}


char* Get_4GIMEI_NUM(void)
{
		Clear_Buffer();	
		memset(IMEINUMBER,0,BUFLEN);
		Uart2_SendStr("AT+CGSN=1\r\n");//查询激活状态
		delay_ms(300);
		strx=strstr((const char*)buf_uart2.buf,(const char*)"+CGSN: \"");//如果反馈错误就表示没有定位好
		if(strx)	//没有反馈错误就表示有经纬度了 然后来进行显示 反馈得到LOC就表示有位置了
		{
				strncpy(IMEINUMBER,strx+8,15); //获取维度数据
				Gui_DrawFont_GBK16(0,90,RED,WHITE, (u8*) IMEINUMBER);	//显示经度到液晶屏
				return IMEINUMBER;
		}
		return 0;
}






