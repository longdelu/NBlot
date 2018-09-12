
/**
 * Copyright (c) 广州市星翼电子科技有限公司 2014-2024
 * All rights reserved 
 * @file   demo_uart_any_data_len_recv_entry.c
 * @brief  串口任意长度数据收发
 */

#include "atk_sys.h"
#include "atk_led.h"
#include "atk_delay.h"
#include "atk_bc28_nbiot.h"
#include "stm32f4xx_hal.h"

uint8_t buf[512];
int len = 0;

static void uart_event_callback_handle(void *p_arg)
{    
    uart_dev_t *p_uart_dev = (uart_dev_t *)p_arg; 
    
    if (p_uart_dev->uart_event & UART_NONE_EVENT) {
        
    } 

    if (p_uart_dev->uart_event & UART_TX_EVENT) {
        printf("tx data c\r\n"); 
        
       uart_event_clr(p_uart_dev, UART_TX_EVENT);  

    }

    if (p_uart_dev->uart_event & UART_RX_EVENT) {
        printf("rx data c\r\n");
                
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
