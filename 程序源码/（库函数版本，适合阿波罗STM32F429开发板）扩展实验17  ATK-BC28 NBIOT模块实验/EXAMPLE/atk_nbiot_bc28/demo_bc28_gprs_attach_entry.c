/**
  *
  * @file           : demo_nbiot_gprs_attach_entry.c
  * @brief          : nbiot 网络附着实验
  */
/* Includes ------------------------------------------------------------------*/
#include "atk_sys.h"
#include "atk_led.h"
#include "atk_delay.h"
#include "atk_bc28_nbiot.h"
#include "atk_bc28.h"
#include "atk_bc28_nbiot.h"


int nbiot_main_status =  NBIOT_APP_NCONFIG;


//nbiot消息事件处理函数
static void __nbiot_msg_cb_handler (void *p_arg, int msg_id, int len, char *msg)
{ 
    nbiot_handle_t nbiot_handle = (nbiot_handle_t)p_arg; 
    
    (void)nbiot_handle;
    
    switch(msg_id)
    {      
        case NBIOT_MSG_CMD_RETRY:
          printf("%s cmd error and retry\r\n",msg);      
        break; 
        
        case NBIOT_MSG_CMD_FAIL:
        {
          printf("%s cmd failed and exit\r\n",msg);
          
          break;                     
        }         
        
        case NBIOT_MSG_INIT:
        {
          printf("init=%s\r\n",msg);
                     
        }
        break;
        
                
        case NBIOT_MSG_RESET:        //NB复位完成消息
        {
            nbiot_main_status =  NBIOT_APP_INIT;
            printf("reboot=%s\r\n",msg);   
        }
        break;
            
        
        case NBIOT_MSG_NCONFIG:      //自动入网设置完成消息 
        {
            nbiot_main_status =  NBIOT_APP_RESET;
            printf("nconfig=%s\r\n",msg);
        }

        break;        

        case NBIOT_MSG_IMSI:
        {
           printf("\r\nIMSI=%s\r\n",msg);
        }
        break;
                       
        case NBIOT_MSG_REG:
        {             
           printf("\r\nmsg reg status is ok\r\n");                                      
              
        }
        break;
        
        case NBIOT_MSG_SIGNAL:
        {         
          printf("rssi=%sdbm\r\n",msg);
        }
        
        break;
        
        case NBIOT_MSG_INFO:
          
        {
          printf("info get=%s\r\n",msg);
                     
        }

        break;

        case NBIOT_MSG_BAND:
             printf("\r\nFreq=%s\r\n",msg);
        break;
        
        //产商ID
        case NBIOT_MSG_MID:
        {
            printf("\r\nMID=%s\r\n",msg);
        }
        break;
        
        //模块型号
        case NBIOT_MSG_MMODEL:
        {
            printf("\r\nMMODEL=%s\r\n",msg);
        }
        break;        

        //软件版本号
        case NBIOT_MSG_MREV:
        {
            printf("\r\nMREV=%s\r\n",msg);
        }
        break;        
        
        case NBIOT_MSG_IMEI:
        {
            printf("\r\nIMEI=%s\r\n",msg);
        }
        break;
        
        case NBIOT_MSG_TCPUDP_CREATE:
        {
          printf("\r\nUDP_CR=%s\r\n",msg);
        }
        break;
        
        case NBIOT_MSG_TCPUDP_CLOSE:
        {
          printf("\r\nUDP_CL=%s\r\n",msg);
        }
        break;
        
        case NBIOT_MSG_TCPUDP_SEND:
        {
          printf("\r\nUDP_SEND=%s\r\n",msg);
        }
        break;
        
        case NBIOT_MSG_TCPUDP_RECV:
        {
          printf("\r\nUDP_RECE=%s\r\n",msg);
        }
        break;
        
        case NBIOT_MSG_COAP_SERVER:
        {
          printf("\r\nCOAP=%s\r\n",msg);
        }
        break;
        
        case NBIOT_MSG_COAP_SEND:
        {
          printf("\r\nCOAP_SENT=%s\r\n",msg);
        }
        break;

        case NBIOT_MSG_COAP_RECV:
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


//nbiot 状态处理函数
//nbiot_main_status；nbiot所处的主状态阶段
static void nbiot_app_status_poll(nbiot_handle_t nbiot_handle, int *nbiot_main_status)
{    
    switch(*nbiot_main_status)
    {
    case NBIOT_APP_NONE:
      {
        
      }
      break;
           
          
    case NBIOT_APP_INIT:
      {
        printf("atk_nbiot init start\r\n");
                
        nbiot_init(nbiot_handle);        

        *nbiot_main_status = NBIOT_END;
      }
      break;
      
    case NBIOT_APP_RESET:
      {
        printf("atk_nbiot reboot start\r\n");
                
        nbiot_reboot(nbiot_handle);        

        *nbiot_main_status = NBIOT_END;
      }
      break;

    case NBIOT_APP_NCONFIG:
      {
        printf("atk_nbiot auto reg start\r\n");
                
        nbiot_nconfig(nbiot_handle, 0);        

        *nbiot_main_status = NBIOT_END;
      }
      break;      
      
    case NBIOT_APP_INFO:
      {
         printf("atk_nbiot get signal start\r\n");
                
         nbiot_info_get(nbiot_handle);

         *nbiot_main_status = NBIOT_END;
      }
            
      break;
      
    case NBIOT_APP_SIGNAL:
      {
        printf("atk_nbiot rssi(db) start\r\n");
        
        nbiot_signal_get(nbiot_handle);
        
        *nbiot_main_status = NBIOT_END;
      }
      break;
      
    case NBIOT_APP_TCPUDP_CR:
      {
        printf("tcp/udp socket creat start\r\n");  
        
//        nbiot_tcpudp_create(nbiot_handle, NBIOT_UDP);
        
        nbiot_tcpudp_create(nbiot_handle, NBIOT_TCP);
        
        //do nothing
        *nbiot_main_status = NBIOT_END;
      }
      break;
      
    case NBIOT_APP_TCPUDP_CL:
      {          
        //do nothing
        printf("tcp/udp socket close start\r\n");  

//        nbiot_tcpudp_close(nbiot_handle, NBIOT_UDP); 

//        nbiot_tcpudp_close(nbiot_handle, NBIOT_TCP);        
          
        *nbiot_main_status = NBIOT_END;
      }
      break;
      
    case NBIOT_APP_TCPUDP_SEND:
      {
          
        printf("tcp/udp send start\r\n"); 

//        nbiot_tcpudp_send_str(nbiot_handle, sizeof("hello world"), "hello world", NBIOT_UDP);
        
        nbiot_tcpudp_send_str(nbiot_handle, sizeof("hello world"), "hello world", NBIOT_TCP);        
        
        //do nothing
        *nbiot_main_status = NBIOT_END;
      }
      break;
      
    case NBIOT_APP_TCPUDP_RECV:
      {         
        printf("tcp/udp recv start\r\n");    
        //do nothing
        *nbiot_main_status = NBIOT_END; 
      }
      break;
      
    case NBIOT_APP_CoAP_SEVER:
      {
        printf("CoAP Server set start\r\n");

        *nbiot_main_status = NBIOT_END;
      }
      break;
      
    case NBIOT_APP_CoAP_SEND:
      {
        printf("CoAP send start\r\n");
        *nbiot_main_status = NBIOT_END;
        
        
      }
      break;
      
    case NBIOT_APP_CoAP_RECV:
      {
        printf("CoAP recv start\r\n");
        *nbiot_main_status = NBIOT_END;        
      }
      break;  
      
    default:
      {
        
      }
      break;
    }
}


/**
  * @brief  The demo nbiot gprs attach application entry point.
  *
  * @retval None
  */
void demo_nbiot_gprs_attach_entry(void)
{           
    uart_handle_t uart_handle = NULL; 

    nbiot_handle_t  nbiot_handle = NULL; 

    //nbiot上电需要等待10s
    delay_ms(1000);    

    uart_handle = atk_nbiot_uart_init(9600);  
    
    nbiot_handle = nbiot_dev_init(uart_handle);
     
    nbiot_event_registercb(nbiot_handle, __nbiot_msg_cb_handler, nbiot_handle);
              
    while (1)
    {   
        nbiot_app_status_poll(nbiot_handle, &nbiot_main_status);      
        nbiot_event_poll(nbiot_handle);      
        uart_event_poll(uart_handle);       
    }
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

