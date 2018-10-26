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
 ALIENTEK RT1052������ ʵ��38
 DHT11������ʪ�ȴ�����ʵ��-FSL��汾
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/
    
int main(void)
{     
    MPU_Memory_Protection();    //��ʼ��MPU
//    RT1052_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);//�ж����ȼ�����4
//    RT1052_Clock_Init();        //����ϵͳʱ��
//    DELAY_Init(600);            //��ʱ������ʼ��
//    LPUART1_Init(9600);       //��ʼ������1
//    usmart_dev.init(75);        //��ʼ��USMART    
//    
//    //���벻��SDRAM�����ܵ�ʱ����Ҫ��ʼ��SDRAM���������SDRAM 
//    //�е��Դ���Ļ�����Ҫ�������ʼ��SDRAM��SDRAM�ĳ�ʼ�������ļ�
//    //rt1052_sdram.ini�г�ʼ�����������ʼ��SDRAM�Ļ����ܻᵼ��SDRAM
//    //�е����ݶ�ʧ
//#ifndef DEBUG_IN_SDRAM        
//    SDRAM_Init();               //��ʼ��SDRAM
//#endif
//  
//    LCD_Init();                 //��ʼ��LCD
//    POINT_COLOR=RED;  
//    LCD_ShowString(30,10,200,16,16,(uint8_t *)"ATOM@ALIENTEK");  
//    LCD_ShowString(30,30,200,16,16,(uint8_t *)"ATK@BC28@NBIoT");

//    printf("test\r\n");  
//  
  
    //demo��ں���
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
