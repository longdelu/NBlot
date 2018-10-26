#include "sys.h"
#include "delay.h"
#include "usart.h" 
#include "led.h"
#include "atk_sys.h"
#include "atk_delay.h"
#include "atk_usart.h"
#include "atk_led.h"
#include "lcd.h"
#include "sdram.h"
#include "demo_entry.h"

/************************************************
 ALIENTEK 阿波罗STM32H7开发板 实验34
 DHT11数字温湿度传感器实验-HAL库函数版
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

int main(void)
{
    HAL_Init();                     //初始化HAL库   
    Stm32_Clock_Init(160,5,2,4);    //设置时钟,400Mhz 
    delay_init(400);                //延时初始化
    uart_init(9600);                //初始化调试USART
    atk_led_init();                 //初始化LED 
    LCD_Init();                     //初始化LCD 
    SDRAM_Init();                   //初始化SDRAM 
  
    POINT_COLOR=RED;  
    LCD_ShowString(30,10,200,16,16,(uint8_t *)"ATOM@ALIENTEK");  
    LCD_ShowString(30,30,200,16,16,(uint8_t *)"ATK@BC28@NBIoT");

//    printf("test\r\n");  
    
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

//    demo_ap3216_entry();

//    demo_timer_timing_entry();
      
    while(1)
    {              
        LED0_Toggle;
        LED1_Toggle;
        delay_ms(500);
    }
}


