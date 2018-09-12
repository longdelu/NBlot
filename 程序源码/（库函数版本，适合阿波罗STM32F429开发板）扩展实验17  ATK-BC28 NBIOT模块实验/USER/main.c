#include "atk_sys.h"
#include "atk_delay.h"
#include "atk_usart.h"
#include "atk_led.h"
#include "lcd.h"
#include "demo_entry.h"

/************************************************
 ALIENTEK 阿波罗STM32F429开发板实验12
 TFTLCD显示实验--HAL库函数版
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com  
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/
int main(void)
{
    HAL_Init();                     //初始化HAL库   
    Stm32_Clock_Init(360,25,2,8);   //设置时钟180Mhz
    delay_init(180);                //初始化延时函数
    uart1_init(9600);               //初始化调试USART
    led_init();                     //初始化LED 
    LCD_Init();                     //初始化LCD  
    
//    demo_led_entry(); 
    
//    demo_key_entry();
    
//    demo_timer_timing_entry();

//    demo_uart_any_data_len_recv_entry();   

//    demo_nbiot_gprs_attach_entry();  

//    demo_nbiot_info_get_entry(); 

//    demo_nbiot_huaweiiot_entry();    
        
    while(1)
    {              
        delay_ms(1000);
    }
}
