 /************************************************
 Copyright (c) 广州市星翼电子科技有限公司 2014-2024
 All rights reserved 
 ALIENTEK 阿波罗STM32F429开发板 
 NBIOT 环形缓冲区实现
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/ 

#include "atk_sys.h"
#include "atk_soft_timer.h"
#include "atk_ring_buf.h"

static struct atk_soft_timer timer;
static atk_ring_buf_t g_uart_ring_buf; 

static void timer_callback (void *p_arg)
{
    printf("timer timeout!\r\n");
   
    uint8_t buf[8] = {0,1,2,3,4,5,6,7};
  
    //往缓冲区写数据
    atk_ring_buf_size_write(&g_uart_ring_buf, buf, sizeof(buf));  
}

/**
  * @brief  The demo ring buf entry entry point.
  *
  * @retval None
  */
void demo_atk_ring_buf_entry(void)
{
    int err = 0;
    int len = 0;
    int i   = 0;
  
    uint8_t buf[32];
                    
    //接收接收环形缓冲区
    err = atk_ring_buf_init(&g_uart_ring_buf);

    printf("ring buf init failed %d\r\n", err);  

    atk_soft_timer_init(&timer, timer_callback, NULL, 1000, 1000); //1s loop
    atk_soft_timer_start(&timer);  
     
    while(1) {        
        
        len = atk_ring_buf_avail_len(&g_uart_ring_buf);
        
        printf("rx data timeout %d\r\n", len);
                     
        if (len > 0)
        {
            if(len > sizeof(buf)) 
            {
               len = sizeof(buf);
              
               atk_ring_buf_size_read(&g_uart_ring_buf, buf, sizeof(buf));
            }
            else
            {
               atk_ring_buf_size_read(&g_uart_ring_buf, buf, len);
            }
            
            printf("the ring buf is\r\n");
            for (i = 0; i < len; i++)
            {
                printf("%d ", buf[i]); 
            }
  
        };
    }
}
