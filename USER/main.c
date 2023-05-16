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
///////////////下面是液晶屏头文件/////////////////////
#include "Lcd_Driver.h"
#include "GUI.h"
#include "delay.h"
#include "Picture.h"
#include "QDTFT_demo.h"
/////////////////////////////////////////////////////
#include "dht11.h"
#include "func.h"

///***使用于4G版本**************/
int main(void)
{
    char temp_disp[128];
    int delay_time = 0;
    delay_init();            //延时函数初始化
    NVIC_Configuration();    //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
    CSTX_4GCTR_Init();        //初始化CSTX_4G的供电引脚 对模块进行供电
    uart_init(115200);//串口1初始化，可连接PC进行打印模块返回数据
    Uart1_SendStr("UART1 Init Successful\r\n");

    //////////下面是液晶屏显示代码///////////////////////////
    Lcd_Init();
    LCD_LED_SET;//通过IO控制背光亮
    //Redraw_Mainmenu();//绘制主菜单(部分内容由于分辨率超出物理值可能无法显示)
    //Color_Test();//简单纯色填充测试
    Num_Test();//数码管字体测试

    showimage(gImage_qq);//图片显示示例
    Font_Test();//中英文显示测试
    delay_ms(1200);
    //LCD_LED_CLR;//IO控制背光灭
    /////////////////////////////////////////////////////////////

    LED_Init();             //初始化与LED连接的硬件接口
    LED_Run();                  //初始化跑马灯



    uart2_init(115200);//初始化和EC200连接串口
    Uart2_SendStr("UART2 Init Successful\r\n");

    uart3_init(115200);
    Uart3_SendStr("UART3 Init Successful\r\n");
    printf("\r\n ############ http://www.csgsm.com/ ############\r\n ############("__DATE__ " - " __TIME__ ")############");


    //IWDG_Init(7,625);    //看门狗8S一次

    CSTX_4G_Init();//对设备初始化

    CSTX_4G_RegALIYUNIOT();//直接注册到阿里云
    Clear_Buffer_UART1();       //清空串口1的数据
    //DHT11_Init(); //初始化温湿度 用PA11
    Get_4GIMEI_NUM();   //显示IMEI序列号
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
        IWDG_Feed();//喂狗
        delay_time ++;
    }
    return 0;
}






