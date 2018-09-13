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
#include "atk_ring_buf.h"

/**
  * @brief  The demo ring buf entry entry point.
  *
  * @retval None
  */
void demo_atk_ring_buf_entry(void)
{
    int err = 0;
         
    atk_ring_buf_t g_uart_ring_buf; 
    
    
    //接收接收环形缓冲区
    err = atk_ring_buf_init(&g_uart_ring_buf);    

      
    while(1) {        
        ;
    }
}
