#include "4G.h"
#include "string.h"
#include "usart.h"
#include "wdg.h"
#include "led.h"
///////////////������Һ����ͷ�ļ�/////////////////////
#include "Lcd_Driver.h"
#include "GUI.h"
#include "delay.h"
//#include "Picture.h"
#include "QDTFT_demo.h"
#include "func.h"
/////////////////////////////////////////////////////
char *strx,*extstrx;

CSTX_4G CSTX_4G_Status;	//ģ���״̬��Ϣ
int  errcount=0;	//��������ʧ�ܴ��� ��ֹ��ѭ��
int  errCountData=0;
char ATSTR[BUFLEN];	//�齨AT����ĺ���
char IMEINUMBER[BUFLEN];//+CGSN: "869523052178994"
////////////////////////ֻҪ�޸���Ҫ��/////////////////////////////////
/*
#define PRODUCEKEY "a1RTeaNN74s"        //�޸Ĳ�Ʒ��Կ
#define DEVICENAME "CAT1GNSS"           //�޸��豸����
#define DEVICESECRET "ec8f743ccdcd4edb1f2090918e7165a7" //�޸��豸��Կ
*/
///////////////////////////////////////////////////////////////////////
/*
#define PRODUCEKEY "ict71NcvRb1"        //�޸Ĳ�Ʒ��Կ
#define DEVICENAME "test"           //�޸��豸����
#define DEVICESECRET "3011abb39a869cdf7d29d9fbaac39754" //�޸��豸��Կ
*/

#define PRODUCEKEY 		"ict71NcvRb1"        //�޸Ĳ�Ʒ��Կ
#define DEVICENAME 		"test"           //�޸��豸����
#define DEVICESECRET 	"3011abb39a869cdf7d29d9fbaac39754" //�޸��豸��Կ
#define MQTTHOSTURL 	"iot-06z00daa2jr8gpj.mqtt.iothub.aliyuncs.com"
#define MQTTHOSTPORT 	1883


/*****************************************************
���ģ�鷴������Ϣ
*****************************************************/
void Clear_Buffer(void)//��ջ���
{
		printf(buf_uart2.buf);
	
		strx=strstr((const char*)buf_uart2.buf,(const char*)"+QMTRECV:");//����+QIURC:���������յ�TCP���������ص�����
    if(strx)
    {
				Gui_DrawFont_GBK16(16,10,RED,WHITE, "RECEIVE DATA");      
    }
		
    delay_ms(300);
    buf_uart2.index=0;
    memset(buf_uart2.buf,0,BUFLEN);
		IWDG_Feed();//ι��
	
}


/*****************************************************
��ʼ��ģ�� �͵�Ƭ�����ӣ���ȡ���ź��ź�����
*****************************************************/
void CSTX_4G_Init(void)
{
		//��ӡ��ʼ����Ϣ
		printf("start init EC200X\r\n");
		//����һ������ATE1
    Uart2_SendStr("ATE1\r\n"); 
    delay_ms(300);
		printf(buf_uart2.buf);      //��ӡ�����յ�����Ϣ
    strx=strstr((const char*)buf_uart2.buf,(const char*)"OK");//����OK
    Clear_Buffer();	
    while(strx==NULL)
    {
				//Gui_DrawFont_GBK16(16,70,RED,WHITE, "ȫ�̼���֧��");
				Gui_DrawFont_GBK16(16,10,RED,WHITE, "CONNECT 4G..");
				printf("��Ƭ����������ģ��......\r\n");
        Clear_Buffer();	
        Uart2_SendStr("ATE1\r\n"); 
        delay_ms(300);
        strx=strstr((const char*)buf_uart2.buf,(const char*)"OK");//����OK
    }
	  Gui_DrawFont_GBK16(16,10,RED,WHITE, "CONNECT [OK]");
		printf("****��Ƭ����ģ�����ӳɹ�*****\r\n");
		Uart2_SendStr("ATI\r\n");//��ȡģ��İ汾
		delay_ms(300);
		Clear_Buffer();	
		
    Uart2_SendStr("AT+CIMI\r\n");//��ȡ���ţ������Ƿ���ڿ�����˼���Ƚ���Ҫ��
    delay_ms(300);
    strx=strstr((const char*)buf_uart2.buf,(const char*)"460");//��460������ʶ�𵽿���
    while(strx==NULL)
    {
        Clear_Buffer();	
        Uart2_SendStr("AT+CIMI\r\n");//��ȡ���ţ������Ƿ���ڿ�����˼���Ƚ���Ҫ��
        delay_ms(300);
        strx=strstr((const char*)buf_uart2.buf,(const char*)"460");//����OK,˵�����Ǵ��ڵ�
    } 
		printf("�ҵĿ����� : %s \r\n",buf_uart2.buf+8);
		Clear_Buffer();	
		
	  Gui_DrawFont_GBK16(16,10,RED,WHITE, "SIMCARD [OK]");
		
		Uart2_SendStr("AT+CGATT?\r\n");//��ѯ����״̬
		delay_ms(300);
		strx=strstr((const char*)buf_uart2.buf,(const char*)"+CGATT: 1");//��1
		Clear_Buffer();	
		while(strx==NULL)
		{
				Clear_Buffer();	
				Uart2_SendStr("AT+CGATT?\r\n");//��ȡ����״̬
				delay_ms(300);
				strx=strstr((const char*)buf_uart2.buf,(const char*)"+CGATT: 1");//����1,����ע���ɹ�
		}
		
	  Gui_DrawFont_GBK16(16,10,RED,WHITE, "REGISTER[OK]");
		
		
		Clear_Buffer();	
		Uart2_SendStr("AT+CSQ\r\n");//�鿴��ȡCSQֵ
		delay_ms(300);
    strx=strstr((const char*)buf_uart2.buf,(const char*)"+CSQ:");//����CSQ
		if(strx)
		{
				
				printf("�ź�������:%s ע�⣺�ź����ֵ��31 \r\n",buf_uart2.buf+14);      
		}
		IWDG_Feed();//ι��
}



/*****************************************************
�ر�֮ǰ���ڵĺͷ����������� ���ܷ���ʧ�� 
*****************************************************/
void CSTX_4G_ConTCP(void)
{		
		//�ر�֮ǰ����������
		Uart2_SendStr("AT+QICLOSE=0\r\n");//�ر�socekt����
		delay_ms(100);
		Uart2_SendStr("AT+QICLOSE=1\r\n");//�ر�socekt����
		delay_ms(100);
		Uart2_SendStr("AT+QICLOSE=2\r\n");//�ر�socekt����
		delay_ms(100);
	
    Clear_Buffer();
    IWDG_Feed();//ι��
}

/*****************************************************
���������Ҫ�޸ĵĵط����޸ķ�������IP��ַ�Ͷ˿ں�
*****************************************************/
#define SERVERIP "101.200.212.234"
#define SERVERPORT "1001"
/*****************************************************
����TCP���� 
*****************************************************/
void CSTX_4G_CreateTCPSokcet(void)//����sokcet
{
		Gui_DrawFont_GBK16(0,30,BLUE,WHITE,SERVERIP);
		Gui_DrawFont_GBK16(0,50,BLUE,WHITE,"SERVER PORT");
		Gui_DrawFont_GBK16(88,50,BLUE,WHITE,SERVERPORT);	//��ʾIP��ַ�Ͷ˿ں� ������16������ Ӣ����8������
		//Gui_DrawFont_GBK16(16,10,RED,WHITE, "SERVER  [OK]");
		Gui_DrawFont_GBK16(16,10,RED,WHITE, "SERVER  [..]");
		memset(ATSTR,0,BUFLEN);
		sprintf(ATSTR,"AT+QIOPEN=1,0,\"TCP\",\"%s\",%s,0,1\r\n",SERVERIP,SERVERPORT);
    Uart2_SendStr(ATSTR);//��������TCP,����IP�Լ��������˿ں��� 
    delay_ms(300);
	
		strx=strstr((const char*)buf_uart2.buf,(const char*)"+QIOPEN: 0,566");//����Ƿ��½�ɹ�
		if(strx)
		{
			 Gui_DrawFont_GBK16(16,10,RED,WHITE, "SERVER  [NO]");	//���ӷ�����ʧ��
			 return ;	//������ӷ�����ʧ�ܾͷ��� ���治��Ҫ�ж��Ƿ�ɹ���
		}
	
    strx=strstr((const char*)buf_uart2.buf,(const char*)"+QIOPEN: 0,0");//����Ƿ��½�ɹ�
	  errcount=0;
		while(strx==NULL)
		{
				errcount++;
				strx=strstr((const char*)buf_uart2.buf,(const char*)"+QIOPEN: 0,0");//����Ƿ��½�ɹ�
				delay_ms(100);
				if(errcount>100)     //��ʱ�˳���ѭ�� ��ʾ����������ʧ��
        {
            errcount = 0;
            break;
        }
		}  
	  Gui_DrawFont_GBK16(16,10,RED,WHITE, "SERVER  [OK]");
     Clear_Buffer();	
    
}

/*****************************************************
�������ݺ���
*****************************************************/
void CSTX_4G_Senddata(int len,uint8_t *data)//�����ַ�������
{
	  Gui_DrawFont_GBK16(16,10,RED,WHITE, "SEND DATA...");
		memset(ATSTR,0,BUFLEN);
		sprintf(ATSTR,"AT+QISEND=0,%d\r\n",len);
    Uart2_SendStr(ATSTR);
    delay_ms(300);
		//�ȴ�ģ�鷴�� >
		strx=strstr((const char*)buf_uart2.buf,(const char*)">");//ģ�鷴�����Է���������
    while(strx==NULL)
    {
        errcount++;
        strx=strstr((const char*)buf_uart2.buf,(const char*)">");//ģ�鷴�����Է���������
        if(errcount>100)     //��ֹ��ѭ������
        {
            errcount = 0;
            break;
        }
    }
		
		Uart2_SendStr((char *)data);//��������������
    delay_ms(300);
		
		strx=strstr((const char*)buf_uart2.buf,(const char*)"ERROR");//�������ʧ��
		if(strx)
		{
				errCountData++;
				Gui_DrawFont_GBK16(16,10,RED,WHITE, "SEND DATA NO");
				if(errCountData>3)     //��ʱ�˳���ѭ�� ��ʾ����������ʧ��
        {
							__set_FAULTMASK(1);//�ر����ж�
							NVIC_SystemReset();//����Ƭ������
        }
				return ;	//��������ʧ���˾Ͳ�Ҫȥ�����ж��Ƿ�ɹ���
		}
		
    strx=strstr((const char*)buf_uart2.buf,(const char*)"SEND OK");//����Ƿ��ͳɹ�
		errcount=0;
		while(strx==NULL)	//���û���յ�SEND OK��ѭ����ѯ 
		{
				errcount++;
				strx=strstr((const char*)buf_uart2.buf,(const char*)"SEND OK");//����Ƿ��ͳɹ�
				delay_ms(100);
				if(errcount>100)     //��ʱ�˳���ѭ�� ��ʾ����������ʧ��
        {
            errcount = 0;
            break;
        }
		}  
    Clear_Buffer();	//����������
	  Gui_DrawFont_GBK16(16,10,RED,WHITE, "SEND DATA OK");
}


/*****************************************************
�յ��������·������ݾ�ֱ�Ӵ�ӡ
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
ע�ᵽ������������ƽ̨
*****************************************************/
void CSTX_4G_RegALIYUNIOT(void)//ƽ̨ע��
{
		int errcount 	= 0;
		int reg_ok		= 0;
		int time_idx 	= 0;

		Uart2_SendStr("AT+QMTDISC=0\r\n");//Disconnect a client from MQTT server
    delay_ms(300);
    Clear_Buffer();

    Uart2_SendStr("AT+QMTCLOSE=0\r\n");//ɾ�����
    delay_ms(300);
    Clear_Buffer();

    memset(ATSTR,0,BUFLEN);
    sprintf(ATSTR,"AT+QMTCFG=\"ALIAUTH\",0,\"%s\",\"%s\",\"%s\"\r\n",PRODUCEKEY,DEVICENAME,DEVICESECRET);
    printf("ATSTR = %s \r\n",ATSTR);
    Uart2_SendStr(ATSTR);//���Ͱ��������ò���
    delay_ms(300);  //�ȴ�300ms����OK
    strx=strstr((const char*)buf_uart2.buf,(const char*)"OK");//��OK
    while(strx==NULL)
    {
        strx=strstr((const char*)buf_uart2.buf,(const char*)"OK");//��OK
    }
    Clear_Buffer();
		
		for(time_idx = 0; time_idx < 3 && !reg_ok; time_idx++)
		{
				errcount = 0;
				memset(ATSTR, 0, BUFLEN);
				snprintf(ATSTR, BUFLEN, "AT+QMTOPEN=0,\"%s\",%d\r\n", MQTTHOSTURL, MQTTHOSTPORT);
				printf("ATSTR = %s \r\n",ATSTR);
				Uart2_SendStr(ATSTR);//��¼������ƽ̨
				delay_ms(300);
				strx=strstr((const char*)buf_uart2.buf,(const char*)"+QMTOPEN: 0,0");//��+QMTOPEN: 0,0
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
							NVIC_SystemReset();	//û�д���TCP SOCKET������ϵͳ�ȵ�����������
							continue;
					}
				}
				Clear_Buffer();
				
				memset(ATSTR,0,BUFLEN);
				sprintf(ATSTR,"AT+QMTCONN=0,\"%s\"\r\n",DEVICENAME);
				printf("ATSTR = %s \r\n",ATSTR);
				Uart2_SendStr(ATSTR);//�������ӵ�������
				delay_ms(300);
				strx=strstr((const char*)buf_uart2.buf,(const char*)"+QMTCONN: 0,0,0");//��+QMTCONN: 0,0,0
				while((strx==NULL) && (errcount <= 10000))
				{
						errcount++;
						delay_ms(30);
						strx=strstr((const char*)buf_uart2.buf,(const char*)"+QMTCONN: 0,0,0");//��+QMTCONN: 0,0,0
						
						if(10000 == errcount)
						{
								continue;
						}
				}
				Clear_Buffer();
				reg_ok = 1;
			}
}

void CSTX_4G_ALYIOTSenddata(u8 *len,u8 *data)//�Ϸ����ݣ��Ϸ������ݸ���Ӧ�Ĳ���й�ϵ���û���Ҫע����Ȼ���Ӧ���ݼ���
{
		Gui_DrawFont_GBK16(16,10,RED,WHITE, "SEND DATA...");
    memset(ATSTR,0,BUFLEN);
    sprintf(ATSTR,"AT+QMTPUB=0,1,1,0,\"/sys/%s/%s/thing/event/property/post\",\"{params:{CurrentDeviceStatus:%s}}\"\r\n",PRODUCEKEY,DEVICENAME,data);
    printf("ATSTR = %s \r\n",ATSTR);
    Uart2_SendStr(ATSTR);//����IMEI,LWM2MЭ��Ҫ������IMEI��Ϣ
    delay_ms(300);
    strx=strstr((const char*)buf_uart2.buf,(const char*)"+QMTPUB: 0,1,0");//��SEND OK
    while(strx==NULL)
    {
        strx=strstr((const char*)buf_uart2.buf,(const char*)"+QMTPUB: 0,1,0");//��SEND OK
    }
    Clear_Buffer();
		Gui_DrawFont_GBK16(16,10,RED,WHITE, "SEND DATA OK");
}

void CSTX_4G_ALYIOTSenddata1(u8 *len,u8 *data)//�Ϸ����ݣ��Ϸ������ݸ���Ӧ�Ĳ���й�ϵ���û���Ҫע����Ȼ���Ӧ���ݼ���
{
		Gui_DrawFont_GBK16(16,10,RED,WHITE, "SEND DATA...");
    memset(ATSTR,0,BUFLEN);
    sprintf(ATSTR,"AT+QMTPUB=0,1,1,0,\"/sys/%s/%s/thing/event/property/post\",\"{params:{CurrentTemperature:%s}}\"\r\n",PRODUCEKEY,DEVICENAME,data);
    printf("ATSTR = %s \r\n",ATSTR);
    Uart2_SendStr(ATSTR);//����IMEI,LWM2MЭ��Ҫ������IMEI��Ϣ
    delay_ms(300);
    strx=strstr((const char*)buf_uart2.buf,(const char*)"+QMTPUB: 0,1,0");//��SEND OK
    while(strx==NULL)
    {
        strx=strstr((const char*)buf_uart2.buf,(const char*)"+QMTPUB: 0,1,0");//��SEND OK
    }
    Clear_Buffer();
		Gui_DrawFont_GBK16(16,10,RED,WHITE, "SEND DATA OK");
}


char* Get_4GIMEI_NUM(void)
{
		Clear_Buffer();	
		memset(IMEINUMBER,0,BUFLEN);
		Uart2_SendStr("AT+CGSN=1\r\n");//��ѯ����״̬
		delay_ms(300);
		strx=strstr((const char*)buf_uart2.buf,(const char*)"+CGSN: \"");//�����������ͱ�ʾû�ж�λ��
		if(strx)	//û�з�������ͱ�ʾ�о�γ���� Ȼ����������ʾ �����õ�LOC�ͱ�ʾ��λ����
		{
				strncpy(IMEINUMBER,strx+8,15); //��ȡά������
				Gui_DrawFont_GBK16(0,90,RED,WHITE, (u8*) IMEINUMBER);	//��ʾ���ȵ�Һ����
				return IMEINUMBER;
		}
		return 0;
}






