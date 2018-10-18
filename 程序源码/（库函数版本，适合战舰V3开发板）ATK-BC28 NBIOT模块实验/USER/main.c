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
  
    //demo��ں���
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




