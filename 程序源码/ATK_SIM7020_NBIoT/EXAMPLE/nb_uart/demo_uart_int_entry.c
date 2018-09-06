
/**
  *
  * @file           : demo_uart_int_entry.c
  * @brief          : 串口异步收发数据实验
  * @note           : 使用该demo的时候，必须先注释 UART_ANY_DATA_LEN_RECV 该宏定义 
  */
/* Includes ------------------------------------------------------------------*/
#include "sys.h"
#include "led.h"
#include "delay.h"
#include "nblot_usart.h"
#include "stm32l4xx_hal.h"

#ifndef  UART_ANY_DATA_LEN_RECV

static void uart_event_callback_handle(void *p_arg)
{    
    uart_dev_t *p_uart_dev = (uart_dev_t *)p_arg; 
    
    if (p_uart_dev->uart_event & UART_NONE_EVENT) {
        
    } 

    if (p_uart_dev->uart_event & UART_TX_EVENT) {
        printf("tx data ok\r\n"); 
        lpuart_event_clr(p_uart_dev, UART_TX_EVENT); 
    }

    if (p_uart_dev->uart_event & UART_RX_EVENT) {
        printf("rx data ok\r\n");
        
        lpuart_event_clr(p_uart_dev, UART_RX_EVENT); 
    } 

    if (p_uart_dev->uart_event & UART_TX_TIMEOUT_EVENT) {
        printf("tx data timeout\r\n");
        
        lpuart_event_clr(p_uart_dev, UART_TX_TIMEOUT_EVENT); 
    } 

    if (p_uart_dev->uart_event & UART_RX_TIMEOUT_EVENT) {
        printf("rx data timeout\r\n");
        
        lpuart_event_clr(p_uart_dev, UART_RX_TIMEOUT_EVENT); 
    }            
}


/**
  * @brief  The uart poll application entry point.
  *
  * @retval None
  */
void demo_uart_int_entry(void)
{ 
    uint8_t buf[32];  

    uart_handle_t lpuart_handle = NULL;  

    lpuart_handle = lpuart1_init(115200);  
     
    lpuart_event_registercb(lpuart_handle, uart_event_callback_handle, lpuart_handle);  

    uart_data_rx_int(lpuart_handle,  buf, sizeof("nblot_uart rx tx test ok\r\n") - 1, 20000); 
    uart_data_tx_poll(lpuart_handle, buf, sizeof("nblot_uart rx tx test ok\r\n") - 1, 20000);    

    while (1)
    {
        uart_event_poll(lpuart_handle);
        LED0_Toggle;        
        delay_ms(100);  
    }  
}

#endif

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
