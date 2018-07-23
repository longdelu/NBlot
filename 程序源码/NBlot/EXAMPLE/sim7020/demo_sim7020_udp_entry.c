/**
  *
  * @file           : demo_sim7020_udp_entry.c
  * @brief          : sim7020 udp 收发数据实验
  */
/* Includes ------------------------------------------------------------------*/
#include "sys.h"
#include "led.h"
#include "delay.h"
#include "nblot_usart.h"
#include "sim7020.h"
#include "stm32l4xx_hal.h"

static int sm7020_main_status = SIM7020_TCPUDP_CR;

//sim7020消息事件处理函数
static void __sim7020_event_cb_handler (void *p_arg, sim7020_msg_id_t msg_id, int len, char *msg)
{ 
    sim7020_handle_t sim7020_handle = (sim7020_handle_t)p_arg; 
    
    (void)sim7020_handle;
    
    switch(msg_id)
    {
        case SIM7020_MSG_CMD_RETRY:
          printf("%s cmd error and retry\r\n",msg);      
        break;
        
        case SIM7020_MSG_CMD_NEXT:
          printf("%s cmd error and retry\r\n",msg);      
        break;        
        
        case SIM7020_MSG_CMD_FAIL:
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
             printf("\r\nmsg reg status=%s\r\n", msg);
                                                     
        }
        break;
        
        case SIM7020_MSG_SIGNAL:
        {         
          printf("rssi=%sdbm\r\n",msg);
        }
              
        break;
        
        case SIM7020_MSG_NBLOT_INFO:
          
        {
          printf("info get=%s\r\n",msg);
                     
        }

        break;

        case SIM7020_MSG_BAND:
             printf("\r\nFreq=%s\r\n",msg);
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
            printf("\r\n%s msg create and connect\r\n",msg);
            sm7020_main_status = SIM7020_TCPUDP_SEND;
        }
        break;
        
        case SIM7020_MSG_TCPUDP_CLOSE:
        {
            printf("\r\nmsg tcpudp close=%s\r\n",msg);
        }
        break;
        
        case SIM7020_MSG_TCPUDP_SEND:
        {
            printf("\r\nmsg tcp udp_send=%s\r\n",msg);
          
        }
        break;
        
        case SIM7020_MSG_TCPUDP_RECV:
        {
            printf("\r\nmsg udp recv=%s\r\n",msg);
          
            sm7020_main_status = SIM7020_TCPUDP_CL;
        }
        break;
        
        case SIM7020_MSG_COAP_SERVER:
        {
          printf("\r\nmsg COAP=%s\r\n",msg);
        }
        break;
        
        case SIM7020_MSG_COAP_SEND:
        {
          printf("\r\nmsg COAP_SENT=%s\r\n",msg);
        }
        break;

        case SIM7020_MSG_COAP_RECV:
        {
            printf("\r\nmsg COAP_RECEV=%s\r\n",msg);
        }
        break;

        default :
        {
            break;
        }
    }             
}

//sim7020 状态处理函数
//sim7020_main_status；sim7020所处的主状态阶段
static void sim7020_app_status_poll(sim7020_handle_t sim7020_handle, int *sim7020_main_status)
{    
    switch(*sim7020_main_status)
    {
    case SIM7020_NONE:
      {
        
      }
      break;
           
    case SIM7020_NBLOT_INIT:
      {
        printf("sim7020 init start\r\n");
                
        sim7020_nblot_init(sim7020_handle);        

        *sim7020_main_status = SIM7020_END;
      }
      break;
      
    case SIM7020_NBLOT_INFO:
      {
         printf("sim7020 get signal start\r\n");
                
         sim7020_nblot_info_get(sim7020_handle);

         *sim7020_main_status = SIM7020_END;
      }
            
      break;
      
    case SIM7020_SIGNAL:
      {
        printf("sim7020 rssi(db) start\r\n");
        
        sim7020_nblot_signal_get(sim7020_handle);
        
        *sim7020_main_status = SIM7020_END;
      }
      break;
      
    case SIM7020_TCPUDP_CR:
      {
        printf("tcp/udp socket creat start\r\n");  
        
        sim7020_nblot_tcpudp_create(sim7020_handle, SIM7020_UDP);
        
//        sim7020_nblot_tcpudp_create(sim7020_handle, SIM7020_TCP);
        
        //do nothing
        *sim7020_main_status = SIM7020_END;
      }
      break;
      
    case SIM7020_TCPUDP_CL:
      {          
        //do nothing
        printf("tcp/udp socket close start\r\n");  

        sim7020_nblot_tcpudp_close(sim7020_handle, SIM7020_UDP); 

//        sim7020_nblot_tcpudp_close(sim7020_handle, SIM7020_TCP);        
          
        *sim7020_main_status = SIM7020_END;
      }
      break;
      
    case SIM7020_TCPUDP_SEND:
      {
          
        printf("tcp/udp send start\r\n"); 

        sim7020_nblot_tcpudp_send_str(sim7020_handle, sizeof("hello world"), "hello world", SIM7020_UDP);
        
//        sim7020_nblot_tcpudp_send_str(sim7020_handle, sizeof("hello world"), "hello world", SIM7020_TCP);        
        
        //do nothing
        *sim7020_main_status = SIM7020_END;
      }
      break;
      
    case SIM7020_TCPUDP_RECV:
      {         
        printf("tcp/udp recv start\r\n");    
        //do nothing
        *sim7020_main_status = SIM7020_END; 
      }
      break;
      
    case SIM7020_CoAP_SEVER:
      {
        printf("CoAP Server set start\r\n");

        *sim7020_main_status = SIM7020_END;
      }
      break;
      
    case SIM7020_CoAP_SEND:
      {
        printf("CoAP send start\r\n");
        *sim7020_main_status = SIM7020_END;
               
      }
      break;
      
    case SIM7020_CoAP_RECV:
      {
        printf("CoAP recv start\r\n");
        *sim7020_main_status = SIM7020_END;        
      }
      break;  
      
    default:
      {
        
      }
      break;
    }
}

/**
  * @brief  The demo sim7020 udp entry entry point.
  *
  * @retval None
  */
void demo_sim7020_udp_entry(void)
{         
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
