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
 ALIENTEKս��STM32������ʵ��31
 DHT11������ʪ�ȴ����� ʵ��  
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�  
    delay_init(SystemCoreClock);                   //��ʱ������ʼ��      
    uart_init(9600);                               //���ڳ�ʼ��Ϊ115200
    led_init();                                    //��ʼ����LED���ӵ�Ӳ���ӿ�
    LCD_Init();                                    //��ʼ��LCD 
    POINT_COLOR=RED;  
    LCD_ShowString(30,10,200,16,16,(uint8_t *)"ATOM@ALIENTEK");  
    LCD_ShowString(30,30,200,16,16,(uint8_t *)"ATK@BC28@NBIoT"); 
  
    atk_tim3_int_init(4999,7199); 
  
    //demo��ں���
//    demo_dht11_entry(); 
    demo_soft_timer_entry();
               
    while(1)
    {          
        delay_ms(500);
              
    }
}




