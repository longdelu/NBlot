#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"     
#include "dht11.h"  
#include "atk_bsp.h"
#include "atk_timer.h"
#include "demo_entry.h"
 
/************************************************
 ALIENTEK战舰STM32开发板实验31
 DHT11数字温湿度传感器 实验  
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级  
    delay_init(SystemCoreClock);                   //延时函数初始化      
    uart_init(9600);                               //串口初始化为115200
    led_init();                                    //初始化与LED连接的硬件接口
    LCD_Init();                                    //初始化LCD 
    POINT_COLOR=RED;  
    LCD_ShowString(30,10,200,16,16,(uint8_t *)"ATOM@ALIENTEK");  
    LCD_ShowString(30,30,200,16,16,(uint8_t *)"ATK@BC28@NBIoT"); 
  
    //demo入口函数
//    demo_soft_timer_entry();
//    demo_atk_ring_buf_entry();
//    demo_key_entry();  
//    demo_uart_any_data_len_recv_entry();    
//    demo_nbiot_huaweiiot_entry();  
//    demo_dht11_entry();  
//    demo_rgb_led_entry();
  
//    demo_led_beep_entry();

    demo_bc28_low_power_entry();

//    demo_ds18b20_entry();


               
    while(1)
    {          
        LED0_Toggle;
        LED1_Toggle;
        delay_ms(1000);
              
    }
}




