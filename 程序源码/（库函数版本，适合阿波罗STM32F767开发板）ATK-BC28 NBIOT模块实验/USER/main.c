#include "atk_sys.h"
#include "atk_delay.h"
#include "atk_usart.h"
#include "atk_led.h"
#include "lcd.h"
#include "sdram.h"
#include "demo_entry.h"

/************************************************
 ALIENTEK ������STM32F7������ ʵ��34
 DHT11������ʪ�ȴ�����ʵ��-HAL�⺯����
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/
    
int main(void)
{
    Cache_Enable();                 //��L1-Cache  
    HAL_Init();                     //��ʼ��HAL��   
    Stm32_Clock_Init(432,25,2,9);   //����ʱ��,216Mhz 
    delay_init(216);                //��ʱ��ʼ��
    uart_init(9600);               //��ʼ������USART
    led_init();                     //��ʼ��LED 
    LCD_Init();                     //��ʼ��LCD 
    SDRAM_Init();                   //��ʼ��SDRAM 
  
    POINT_COLOR=RED;  
    LCD_ShowString(30,10,200,16,16,(uint8_t *)"ATOM@ALIENTEK");  
    LCD_ShowString(30,30,200,16,16,(uint8_t *)"ATK@BC28@NBIoT");  
         
    //demo��ں���
//    demo_soft_timer_entry();
//    demo_atk_ring_buf_entry();
//    demo_key_entry();  
//    demo_uart_any_data_len_recv_entry();    
   demo_nbiot_huaweiiot_entry();  
//   demo_dht11_entry();  
//    demo_rgb_led_entry();
  
//    demo_led_beep_entry();

//   demo_bc28_low_power_entry();

//    demo_ds18b20_entry();

//    demo_ap3216_entry();

//    demo_timer_timing_entry();
      
    while(1)
    {              
        LED0_Toggle;
        LED1_Toggle;
        delay_ms(1000);
    }    
}

//Ӳ��������
void HardFault_Handler(void)
{
	u32 i;
	u8 t=0;
	u32 temp;
	temp=SCB->CFSR;					//fault״̬�Ĵ���(@0XE000ED28)����:MMSR,BFSR,UFSR
 	printf("CFSR:%8X\r\n",temp);	//��ʾ����ֵ
	temp=SCB->HFSR;					//Ӳ��fault״̬�Ĵ���
 	printf("HFSR:%8X\r\n",temp);	//��ʾ����ֵ
 	temp=SCB->DFSR;					//����fault״̬�Ĵ���
 	printf("DFSR:%8X\r\n",temp);	//��ʾ����ֵ
   	temp=SCB->AFSR;					//����fault״̬�Ĵ���
 	printf("AFSR:%8X\r\n",temp);	//��ʾ����ֵ
 	while(t<5)
	{
		t++; 
		//BEEP=!BEEP;
		for(i=0;i<0X1FFFFF;i++);	
 	}
}
