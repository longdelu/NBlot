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
static void __sim7020_event_cb_handler (void *p_arg, sim7020_msg_id_t msg_id, int len, char *msg)
{ 
    sim7020_handle_t sim7020_handle = (sim7020_handle_t)p_arg; 
    
    (void)sim7020_handle;
    
    switch(msg_id)
    {
        case SIM7020_MSG_NBLOT_INIT:
        {
          printf("init=%s\r\n",msg);
                     
        }
        break;

        case SIM7020_MSG_IMSI:
        {
           printf("\r\nIMSI=%s\r\n",msg);
        }
        break;
        
        case SIM7020_MSG_REG:
        {

        }
        break;
        
        case SIM7020_MSG_SIGN:
      
        break;
        
        case SIM7020_MSG_NBLOT_INFO:

        break;

        case SIM7020_MSG_BAND:

        break;
        
        case SIM7020_MSG_IMEI:
        {
            printf("\r\nIMEI=%s\r\n",msg);
        }
        break;
        
        case SIM7020_MSG_UDP_CREATE:
        {
          printf("\r\nUDP_CR=%s\r\n",msg);
        }
        break;
        
        case SIM7020_MSG_UDP_CLOSE:
        {
          printf("\r\nUDP_CL=%s\r\n",msg);
        }
        break;
        
        case SIM7020_MSG_UDP_SEND:
        {
          printf("\r\nUDP_SEND=%s\r\n",msg);
        }
        break;
        
        case SIM7020_MSG_UDP_RECV:
        {
          printf("\r\nUDP_RECE=%s\r\n",msg);
        }
        break;
        
        case SIM7020_MSG_COAP:
        {
          printf("\r\nCOAP=%s\r\n",msg);
        }
        break;
        
        case SIM7020_MSG_COAP_SEND:
        {
          printf("\r\nCOAP_SENT=%s\r\n",msg);
        }
        break;

        case SIM7020_MSG_COAP_RECV:
        {
            printf("\r\nCOAP_RECE=%s\r\n",msg);
        }
        break;

        default :
        {
            break;
        }
    }             
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
    delay_ms(1000);
             
    while (1)
    {   
        uart_event_poll(lpuart_handle);         
        sim7020_event_poll(sim7020_handle);
        sim7020_app_status_poll(sim7020_handle, &sm7020_main_status);
    }
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

