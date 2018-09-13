
/************************************************
 Copyright (c) 广州市星翼电子科技有限公司 2014-2024
 All rights reserved 
 ALIENTEK 阿波罗STM32F429开发板 
 NBIOT串口任意数据收发实现
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/ 

#include "atk_sys.h"
#include "atk_led.h"
#include "atk_delay.h"
#include "atk_bc28_nbiot.h"
#include "stm32f4xx_hal.h"

/**
 * @brief  接收缓冲缓冲区.
 */
static uint8_t buf[512];


/**
  * @brief  uart回调函数.
  */
static void uart_event_callback_handle(void *p_arg)
{   
    
    uart_dev_t *p_uart_dev = (uart_dev_t *)p_arg;

    int len = 0;    
    
    if (p_uart_dev->uart_event & UART_NONE_EVENT) {
        
    } 

    if (p_uart_dev->uart_event & UART_TX_EVENT) {
        printf("tx data ok\r\n"); 
        
       uart_event_clr(p_uart_dev, UART_TX_EVENT);  

    }

    if (p_uart_dev->uart_event & UART_RX_EVENT) {
        printf("rx data ok\r\n");
                
        len = uart_ring_buf_avail_len(p_uart_dev);
              
        if (len > 0)
        {
            uart_ring_buf_read(p_uart_dev, buf, len);
        }
        
        //把收到的数据重新发送出去
        uart_data_tx_poll(p_uart_dev, buf, len, 20000);          
               
        uart_event_clr(p_uart_dev, UART_RX_EVENT); 
    } 

    if (p_uart_dev->uart_event & UART_TX_TIMEOUT_EVENT) {
           
        printf("tx data timeout\r\n");
                                       
        uart_event_clr(p_uart_dev, UART_TX_TIMEOUT_EVENT); 
    } 

    if (p_uart_dev->uart_event & UART_RX_TIMEOUT_EVENT) {
        len = uart_ring_buf_avail_len(p_uart_dev);
        
        printf("rx data timeout %d\r\n", len);
                     
        if (len > 0)
        {
            uart_ring_buf_read(p_uart_dev, buf, len);
        }
        
        //把收到的数据重新发送出去
        uart_data_tx_poll(p_uart_dev, buf, len, 20000);   
               
        uart_event_clr(p_uart_dev, UART_RX_TIMEOUT_EVENT); 
    }            
}

/**
  * @brief  The uart poll application entry point.
  *
  * @retval None
  */
void demo_uart_any_data_len_recv_entry(void)
{ 
   
    uart_handle_t nbiot_handle = NULL;  

    nbiot_handle = atk_nbiot_uart_init(9600);  
     
    uart_event_registercb(nbiot_handle, uart_event_callback_handle, nbiot_handle);  

    uart_data_tx_poll(nbiot_handle, (uint8_t *)"nblot_uart rx tx test\r\n", sizeof("nblot_uart rx tx test\r\n") - 1, 20000);    

    while (1)
    {
        uart_event_poll(nbiot_handle);
        
    }  
}
