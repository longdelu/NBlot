 /************************************************
 Copyright (c) ������������ӿƼ����޹�˾ 2014-2024
 All rights reserved 
 ALIENTEK ������STM32F429������ 
 NBIOT �����ʱ��ʵ��
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/ 

#include "atk_sys.h"
#include "atk_soft_timer.h"

static struct atk_soft_timer timer1;
static struct atk_soft_timer timer2;

static void timer1_callback (void *p_arg)
{
    printf("timer1 timeout!\r\n");
}

static void timer2_callback (void *p_arg)
{
    printf("timer2 timeout!\r\n");
}

/**
  * @brief  The demo soft timer  entry entry point.
  *
  * @retval None
  */
void demo_soft_timer_entry(void)
{
    atk_soft_timer_init(&timer1, timer1_callback, NULL, 1000, 1000); //1s loop
    atk_soft_timer_start(&timer1);
    
    atk_soft_timer_init(&timer2, timer2_callback, NULL, 500, 0);     //50ms delay
    atk_soft_timer_start(&timer2);
    
    while(1) 
    {        
        ;
    }
}
