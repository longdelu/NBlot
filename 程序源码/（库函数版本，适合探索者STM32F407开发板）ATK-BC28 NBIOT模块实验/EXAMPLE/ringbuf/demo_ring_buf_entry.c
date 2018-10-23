 /************************************************
 Copyright (c) ������������ӿƼ����޹�˾ 2014-2024
 All rights reserved 
 ALIENTEK ������STM32F429������ 
 NBIOT ���λ�����ʵ��
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/ 

#include "atk_sys.h"
#include "atk_soft_timer.h"
#include "atk_ring_buf.h"

static struct atk_soft_timer timer;
static atk_ring_buf_t g_uart_ring_buf; 

static void timer_callback (void *p_arg)
{
   
    uint8_t buf[8] = {0,1,2,3,4,5,6,7};
  
    //��������д����
    atk_ring_buf_size_write(&g_uart_ring_buf, buf, sizeof(buf));  
}

/**
  * @brief  The demo ring buf entry entry point.
  *
  * @retval None
  */
void demo_atk_ring_buf_entry(void)
{
    int len = 0;
    int i   = 0;
  
    uint8_t buf[32];
                    
    //��ʼ�����λ�����
    atk_ring_buf_init(&g_uart_ring_buf);

    atk_soft_timer_init(&timer, timer_callback, NULL, 1000, 1000); //1s loop
    atk_soft_timer_start(&timer);  
     
    while(1) {        
        
        //��ȡ���λ��嵱����Ч���ݸ���
        len = atk_ring_buf_avail_len(&g_uart_ring_buf);
                            
        if (len > 0)
        {
            if(len > sizeof(buf)) 
            {
               len = sizeof(buf);
              
               //��ȡ����������
               atk_ring_buf_size_read(&g_uart_ring_buf, buf, sizeof(buf));
            }
            else
            {
               //��ȡ����������
               atk_ring_buf_size_read(&g_uart_ring_buf, buf, len);
            }
           
            printf("ring buf data len %d\r\n", len);            
            printf("ring buf data is\r\n");
            for (i = 0; i < len; i++)
            {
                printf("%d ", buf[i]); 
            }
            
            printf("\r\n"); 
  
        };
    }
}
