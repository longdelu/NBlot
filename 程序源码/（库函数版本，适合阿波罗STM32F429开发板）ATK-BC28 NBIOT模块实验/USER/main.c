#include "atk_sys.h"
#include "atk_delay.h"
#include "atk_usart.h"
#include "atk_led.h"
#include "lcd.h"
#include "sdram.h"
#include "demo_entry.h"

/************************************************
 ALIENTEK ������STM32F429������ʵ��12
 TFTLCD��ʾʵ��--HAL�⺯����
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com  
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/
int main(void)
{
    HAL_Init();                     //��ʼ��HAL��   
    Stm32_Clock_Init(360,25,2,8);   //����ʱ��180Mhz
    delay_init(180);                //��ʼ����ʱ����
    uart1_init(9600);               //��ʼ������USART
    led_init();                     //��ʼ��LED 
    LCD_Init();                     //��ʼ��LCD 
    SDRAM_Init();                   //��ʼ��SDRAM 
  
    POINT_COLOR=RED;  
    LCD_ShowString(30,10,200,16,16,(uint8_t *)"ATOM@ALIENTEK");  
    LCD_ShowString(30,30,200,16,16,(uint8_t *)"ATK@BC28@NBIoT");  
    
//    demo_nbiot_huaweiiot_entry();  //����demo��ں��� 
  
    demo_dht11_entry();
      
    while(1)
    {              
        delay_ms(1000);
    }
}
