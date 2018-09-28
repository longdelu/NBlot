#include "atk_sys.h"
#include "atk_delay.h"
#include "atk_usart.h"
#include "atk_led.h"
#include "lcd.h"
#include "sdram.h"
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
    SDRAM_Init();                   //初始化SDRAM 
  
    POINT_COLOR=RED;  
    LCD_ShowString(30,10,200,16,16,(uint8_t *)"ATOM@ALIENTEK");  
    LCD_ShowString(30,30,200,16,16,(uint8_t *)"ATK@BC28@NBIoT");  
    
//    demo_nbiot_huaweiiot_entry();  //调用demo入口函数 
  
    demo_dht11_entry();
      
    while(1)
    {              
        delay_ms(1000);
    }
}
