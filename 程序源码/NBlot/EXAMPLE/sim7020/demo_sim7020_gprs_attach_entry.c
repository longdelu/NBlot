/**
  *
  * @file           : demo_sim7020_gprs_attach_entry.c
  * @brief          : sim7020 网络附着实验
  */
/* Includes ------------------------------------------------------------------*/
#include "sys.h"
#include "led.h"
#include "delay.h"
#include "nblot_usart.h"
#include "sim7020.h"
#include "stm32l4xx_hal.h"

//sim7020消息事件处理函数
static void __sim7020_event_cb_handler (void *p_arg)
{ 

    sim7020_handle_t sim7020_handle = (sim7020_handle_t)p_arg; 
    
    if (sim7020_handle->sim7020_event & SIM7020_NONE_EVENT) {
        
    } 

    if (sim7020_handle->sim7020_event & SIM7020_RECV_EVENT) {                       
        printf("sim7020 recv data ok\r\n"); 
        lpuart_event_clr(UART_TX_EVENT); 
    }

    if (sim7020_handle->sim7020_event & SIM7020_TIMEOUT_EVENT) {
        printf("timeout\r\n");
        
        lpuart_event_clr(UART_RX_EVENT); 
    } 

    if (sim7020_handle->sim7020_event & SIM7020_REG_STA_EVENT) {
        printf("gprs attach ok\r\n");
        
        lpuart_event_clr(UART_TX_TIMEOUT_EVENT); 
    } 

    if (sim7020_handle->sim7020_event & SIM7020_TCP_RECV_EVENT) {
        printf("tcp recv ok\r\n");
        
        lpuart_event_clr(UART_RX_TIMEOUT_EVENT); 
    }

    if (sim7020_handle->sim7020_event & SIM7020_UDP_RECV_EVENT) {
        printf("udp recv ok\r\n");
        
        lpuart_event_clr(UART_RX_TIMEOUT_EVENT); 
    }
    
    if (sim7020_handle->sim7020_event & SIM7020_COAP_RECV_EVENT) {
        printf("coap recv ok\r\n");
        
        lpuart_event_clr(UART_RX_TIMEOUT_EVENT); 
    }          
}


/**
  * @brief  The demo sim7020 gprs attach application entry point.
  *
  * @retval None
  */
void demo_sim7020_gprs_attach_entry(void)
{ 
    int sm7020_main_status =  SIM7020_INIT;
        
    uart_handle_t lpuart_handle = NULL; 

    sim7020_handle_t  sim7020_handle = NULL;   

    lpuart_handle = lpuart1_init(115200);  
    
    sim7020_handle = sim7020_init(lpuart_handle);
     
    sim7020_event_registercb(__sim7020_event_cb_handler, sim7020_handle);
         
    while (1)
    {
        
        
        sim7020_event_poll(sim7020_handle);

        LED0_Toggle; 
        
        delay_ms(10);  
    }
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

