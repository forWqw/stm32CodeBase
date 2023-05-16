#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "math.h"
#include "stdio.h"
#include "stm32f10x_flash.h"
#include "stdlib.h"
#include "string.h"
#include "wdg.h"
#include "timer.h"
#include "stm32f10x_tim.h"
#include "4G.h"
///////////////������Һ����ͷ�ļ�/////////////////////
#include "Lcd_Driver.h"
#include "GUI.h"
#include "delay.h"
#include "Picture.h"
#include "QDTFT_demo.h"
/////////////////////////////////////////////////////
#include "dht11.h"
#include "func.h"

///***ʹ����4G�汾**************/
int main(void)
{
    char temp_disp[128];
    int delay_time = 0;
    delay_init();            //��ʱ������ʼ��
    NVIC_Configuration();    //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
    CSTX_4GCTR_Init();        //��ʼ��CSTX_4G�Ĺ������� ��ģ����й���
    uart_init(115200);//����1��ʼ����������PC���д�ӡģ�鷵������
    Uart1_SendStr("UART1 Init Successful\r\n");

    //////////������Һ������ʾ����///////////////////////////
    Lcd_Init();
    LCD_LED_SET;//ͨ��IO���Ʊ�����
    //Redraw_Mainmenu();//�������˵�(�����������ڷֱ��ʳ�������ֵ�����޷���ʾ)
    //Color_Test();//�򵥴�ɫ������
    Num_Test();//������������

    showimage(gImage_qq);//ͼƬ��ʾʾ��
    Font_Test();//��Ӣ����ʾ����
    delay_ms(1200);
    //LCD_LED_CLR;//IO���Ʊ�����
    /////////////////////////////////////////////////////////////

    LED_Init();             //��ʼ����LED���ӵ�Ӳ���ӿ�
    LED_Run();                  //��ʼ�������



    uart2_init(115200);//��ʼ����EC200���Ӵ���
    Uart2_SendStr("UART2 Init Successful\r\n");

    uart3_init(115200);
    Uart3_SendStr("UART3 Init Successful\r\n");
    printf("\r\n ############ http://www.csgsm.com/ ############\r\n ############("__DATE__ " - " __TIME__ ")############");


    //IWDG_Init(7,625);    //���Ź�8Sһ��

    CSTX_4G_Init();//���豸��ʼ��

    CSTX_4G_RegALIYUNIOT();//ֱ��ע�ᵽ������
    Clear_Buffer_UART1();       //��մ���1������
    //DHT11_Init(); //��ʼ����ʪ�� ��PA11
    Get_4GIMEI_NUM();   //��ʾIMEI���к�
    deviceStatusInit();
    while (1)
    {
        printf("\r\n ############ ok! ############\r\n");
        delay_ms(300);
        CSTX_4G_RECTCPData();

        if (10 == delay_time)
        {
            do_get();
            delay_time = 0;
        }
        if (deviceFunc())
        {
            printf("set relay status fail!\r\n");
            return -1;
        }
        IWDG_Feed();//ι��
        delay_time ++;
    }
    return 0;
}






