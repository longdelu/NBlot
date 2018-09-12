/**
  *
  * @file           : demo_nbiot_udp_entry.c
  * @brief          : nbiot coap 收发数据实验
  */
/* Includes ------------------------------------------------------------------*/
#include "atk_sys.h"
#include "atk_led.h"
#include "atk_delay.h"
#include "atk_bc28_nbiot.h"
#include "atk_bc28.h"
#include "atk_bc28_nbiot.h"
#include "stm32l4xx_hal.h"

static int nbiot_main_status = NBIOT_CoAP_SEVER;

//nbiot消息事件处理函数
static void __nbiot_msg_cb_handler (void *p_arg, int msg_id, int len, char *msg)
{ 
    nbiot_handle_t nbiot_handle = (nbiot_handle_t)p_arg; 
    
    (void)nbiot_handle;
    
    switch(msg_id)
    {
        case NBIOT_MSG_CMD_NEXT:
          printf("msg %s cmd error but next\r\n",msg);
          nbiot_main_status = NBIOT_CoAP_CLIENT;        
        break;
        
        case NBIOT_MSG_CMD_RETRY:
          printf("msg %s cmd error but try\r\n",msg);      
        break;        
        
        case NBIOT_MSG_CMD_FAIL:
        {
          printf("msg %s cmd failed\r\n",msg);
          
          break;                     
        }
        
        case NBIOT_MSG_INIT:
        {
          printf("init=%s\r\n",msg);
                     
        }
        break;
        
        case NBIOT_MSG_NCONFIG:
        {
          printf("init=%s\r\n",msg);
                     
        }
        break;        

        case NBIOT_MSG_IMSI:
        {
           printf("\r\nIMSI=%s\r\n",msg);
        }
        break;
        
        case NBIOT_MSG_REG:
        {
             printf("\r\nmsg reg status=%s\r\n", msg);
                                                     
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
            printf("\r\nmid=%s\r\n",msg);
        }
        break;
        
        //模块型号
        case NBIOT_MSG_MMODEL:
        {
            printf("\r\nmmodel=%s\r\n",msg);
        }
        break;        

        //软件版本号
        case NBIOT_MSG_MREV:
        {
            printf("\r\nmrev=%s\r\n",msg);
        }
        break;        
        
        case NBIOT_MSG_IMEI:
        {
            printf("\r\nimei=%s\r\n",msg);
        }
        break;
        
        case NBIOT_MSG_TCPUDP_CREATE:
        {
            printf("\r\n%s msg create and connect\r\n",msg);
            nbiot_main_status = NBIOT_TCPUDP_SEND;
        }
        break;
        
        case NBIOT_MSG_TCPUDP_CLOSE:
        {
            printf("\r\nmsg tcpudp close=%s\r\n",msg);
        }
        break;
        
        case NBIOT_MSG_TCPUDP_SEND:
        {
            printf("\r\nmsg tcp udp_send=%s\r\n",msg);
          
        }
        break;
        
        case NBIOT_MSG_TCPUDP_RECV:
        {
            printf("\r\nmsg udp recv=%s\r\n",msg);
          
            nbiot_main_status = NBIOT_TCPUDP_CL;
        }
        break;
        
        case NBIOT_MSG_COAP_SERVER:
        {
          printf("\r\nmsg COAP server=%s\r\n",msg);
          nbiot_main_status = NBIOT_CoAP_CLIENT;   
        }
        break;
        
        case  NBIOT_MSG_COAP_CLIENT:
          printf("\r\nmsg COAP client =%s\r\n",msg);
          nbiot_main_status = NBIOT_CoAP_SEND;   
          
        break;
     
        
        case NBIOT_MSG_COAP_SEND:
        {
          printf("\r\nmsg COAP_SENT=%s\r\n",msg);
          
        }
        break;

        case NBIOT_MSG_COAP_RECV:
        {
            //收到的数据是十六进制的数字, data_buf缓冲区当中
            //每两个字节组成一个十六进制数，2个字节换算成1个字符
            nbiot_buf2chr(msg, strlen(msg));
            printf("\r\n msg COAP_RECV=%s\r\n",msg);
            nbiot_main_status = NBIOT_CoAP_CL; 
        }
        break;
        
        
        case NBIOT_MSG_COAP_CLOSE:
        {
            printf("\r\nmsg coap close=%s\r\n",msg);
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
        
        //禁能自动入网          
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
      
    case NBIOT_TCPUDP_CR:
      {
        printf("tcp/udp socket creat start\r\n");  
        
        nbiot_tcpudp_create(nbiot_handle, NBIOT_UDP);
        
//        nbiot_tcpudp_create(nbiot_handle, NBIOT_TCP);
        
        //do nothing
        *nbiot_main_status = NBIOT_END;
      }
      break;
      
    case NBIOT_APP_TCPUDP_CL:
      {          
        //do nothing
        printf("tcp/udp socket close start\r\n");  

        nbiot_tcpudp_close(nbiot_handle, NBIOT_UDP); 

//        nbiot_tcpudp_close(nbiot_handle, NBIOT_TCP);        
          
        *nbiot_main_status = NBIOT_END;
      }
      break;
      
    case NBIOT_APP_TCPUDP_SEND:
      {
          
        printf("tcp/udp send start\r\n"); 

        nbiot_tcpudp_send_str(nbiot_handle, sizeof("hello world"), "hello world", NBIOT_UDP);
        
//        nbiot_tcpudp_send_str(nbiot_handle, sizeof("hello world"), "hello world", NBIOT_TCP);        
        
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
        
        nbiot_coap_server_create(nbiot_handle, NBIOT_COAP); 

        *nbiot_main_status = NBIOT_END;
      }
      break;
      
      
    case NBIOT_APP_CoAP_CLIENT:
      {
        printf("CoAP client set start\r\n");
        
        nbiot_coap_client_create(nbiot_handle, NBIOT_COAP); 

        *nbiot_main_status = NBIOT_END;
      }
      break;    
      
    case NBIOT_APP_CoAP_SEND:
      {
        printf("CoAP send start\r\n");
               
//        nbiot_coap_send_str(nbiot_handle, 15, "400141C7B7636F756E746572FF0001", NBIOT_COAP); 
        nbiot_coap_send_str(nbiot_handle, 15 + strlen("ep=868334030037430&pw=909026"), "400141C7B7636F756E746572FF65703d3836383333343033303033373433302670773d393039303236", NBIOT_COAP); 
                  
        *nbiot_main_status = NBIOT_END;               
      }
      break;
      
    case NBIOT_APP_CoAP_RECV:
      {
        printf("CoAP recv start\r\n");
        *nbiot_main_status = NBIOT_END;        
      }
      
    case NBIOT_APP_CoAP_CL:
      {
        printf("CoAP close start\r\n");
        nbiot_coap_close(nbiot_handle, NBIOT_COAP);
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
  * @brief  The demo nbiot coap entry entry point.
  *
  * @retval None
  */
void demo_nbiot_coap_entry(void)
{         
    uart_handle_t uart_handle = NULL; 

    nbiot_handle_t  nbiot_handle = NULL;   

    uart_handle = atk_nbiot_uart_init(9600);  
    
    nbiot_handle = nbiot_dev_init(uart_handle);
     
    nbiot_event_registercb(nbiot_handle, __nbiot_msg_cb_handler, nbiot_handle);
    
    //nbiot上电需要等待10s
    delay_ms(1000);
             
    while (1)
    {   
        uart_event_poll(uart_handle);         
        nbiot_event_poll(nbiot_handle);
        nbiot_app_status_poll(nbiot_handle, &nbiot_main_status);
    }
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
