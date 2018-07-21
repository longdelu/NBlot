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


static int sm7020_main_status =  SIM7020_NBLOT_INFO;
static sim7020_handle_t  sim7020_handle = NULL;  

//sim7020消息事件处理函数
static void __sim7020_event_cb_handler (void *p_arg, sim7020_msg_id_t msg_id, int len, char *msg)
{ 
    sim7020_handle_t sim7020_handle = (sim7020_handle_t)p_arg; 
    
    (void)sim7020_handle;
    
    switch(msg_id)
    {
        case SIM7020_MSG_RETRY:
          printf("%s cmd error and retry\r\n",msg);      
        break;  

        case SIM7020_MSG_FAIL:
        {
          printf("%s cmd failed\r\n",msg);
          
          break;                     
        }          
      
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
             printf("\r\nmsg creg status=%s\r\n", msg);
                                        
        }
        break;
                  
        case SIM7020_MSG_CSQ:
          printf("rssi=%sdbm\r\n",msg);      
        break;        
        
        case SIM7020_MSG_SIGNAL:
          printf("signal=%s\r\n",msg);      
        break;
        
        case SIM7020_MSG_NBLOT_INFO:
          
        {
          printf("info get=%s\r\n",msg);
          
          sm7020_main_status =  SIM7020_SIGNAL;                     
        }

        break;

        case SIM7020_MSG_BAND:
             printf("\r\nfreq=%s\r\n",msg);
        break;
        
        //产商ID
        case SIM7020_MSG_MID:
        {
            printf("\r\nmid=%s\r\n",msg);
        }
        break;
        
        //模块型号
        case SIM7020_MSG_MMODEL:
        {
            printf("\r\nmmodel=%s\r\n",msg);
        }
        break;        

        //软件版本号
        case SIM7020_MSG_MREV:
        {
            printf("\r\nmrev=%s\r\n",msg);
        }
        break;        
        
        case SIM7020_MSG_IMEI:
        {
            printf("\r\nimei=%s\r\n",msg);
        }
        break;
        
        case SIM7020_MSG_TCPUDP_CREATE:
        {
          printf("\r\nudp_cr=%s\r\n",msg);
        }
        break;
        
        case SIM7020_MSG_TCPUDP_CLOSE:
        {
          printf("\r\nUDP_CL=%s\r\n",msg);
        }
        break;
        
        case SIM7020_MSG_TCPUDP_SEND:
        {
          printf("\r\nUDP_SEND=%s\r\n",msg);
        }
        break;
        
        case SIM7020_MSG_TCPUDP_RECV:
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
  * @brief  The demo sim7020 nblot info get application entry point.
  *
  * @retval None
  */
void demo_sim7020_nblot_info_get_entry(void)
{ 
       
    uart_handle_t lpuart_handle = NULL; 
 
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

