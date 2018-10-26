#include "sys.h"
#include "lpuart.h"
#include "delay.h"
#include "led.h"
#include "key.h"
#include "tftlcd.h"
#include "sdram.h"
#include "usmart.h"
#include "pcf8574.h"
#include "dht11.h"
/************************************************
 ALIENTEK RT1052开发板 实验38
 DHT11数字温湿度传感器实验-FSL库版本
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/
    
int main(void)
{     
    MPU_Memory_Protection();    //初始化MPU
//    RT1052_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);//中断优先级分组4
//    RT1052_Clock_Init();        //配置系统时钟
//    DELAY_Init(600);            //延时函数初始化
//    LPUART1_Init(9600);       //初始化串口1
//    usmart_dev.init(75);        //初始化USMART    
//    
//    //代码不在SDRAM里面跑的时候需要初始化SDRAM，如果是在SDRAM 
//    //中调试代码的话不需要在这里初始化SDRAM，SDRAM的初始化会在文件
//    //rt1052_sdram.ini中初始化，在这里初始化SDRAM的话可能会导致SDRAM
//    //中的数据丢失
//#ifndef DEBUG_IN_SDRAM        
//    SDRAM_Init();               //初始化SDRAM
//#endif
//  
//    LCD_Init();                 //初始化LCD
//    POINT_COLOR=RED;  
//    LCD_ShowString(30,10,200,16,16,(uint8_t *)"ATOM@ALIENTEK");  
//    LCD_ShowString(30,30,200,16,16,(uint8_t *)"ATK@BC28@NBIoT");

//    printf("test\r\n");  
//  
  
    //demo入口函数
//    demo_soft_timer_entry();
//    demo_atk_ring_buf_entry();
//    demo_key_entry();  
//    demo_uart_any_data_len_recv_entry();    
//    demo_nbiot_huaweiiot_entry();  
//    demo_dht11_entry();  
//    demo_rgb_led_entry();
  
//    demo_led_beep_entry();

//    demo_bc28_low_power_entry();

//    demo_ds18b20_entry();  
  
    
    while(1)
    {                
        LED0_Toggle;
        LED1_Toggle;
        delay_ms(500);        
    }
}
