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
static void __sim7020_event_cb_handler (void *p_arg, sim7020_msg_id_t msg_id, int len, char *msg_buf)
{ 

    sim7020_handle_t sim7020_handle = (sim7020_handle_t)p_arg; 
    
      
}


/**
  * @brief  The demo sim7020 gprs attach application entry point.
  *
  * @retval None
  */
void demo_sim7020_gprs_attach_entry(void)
{ 
    int sm7020_main_status =  SIM7020_NBLOT_INIT;
        
    uart_handle_t lpuart_handle = NULL; 

    sim7020_handle_t  sim7020_handle = NULL;   

    lpuart_handle = lpuart1_init(115200);  
    
    sim7020_handle = sim7020_init(lpuart_handle);
     
    sim7020_event_registercb(sim7020_handle, __sim7020_event_cb_handler, sim7020_handle);
    
    //sim7020上电需要等待10s
    delay_ms(10000);
             
    while (1)
    {            
        sim7020_event_poll(sim7020_handle);
        sim7020_app_status_poll(&sm7020_main_status);
    }
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

