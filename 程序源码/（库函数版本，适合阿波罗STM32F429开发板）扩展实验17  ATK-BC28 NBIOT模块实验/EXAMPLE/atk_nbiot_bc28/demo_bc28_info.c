/************************************************
 Copyright (c) 广州市星翼电子科技有限公司 2014-2024
 All rights reserved 
 ALIENTEK 阿波罗STM32F429开发板 
 NBIOT获取模块信息
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
#include "atk_bc28.h"
#include "atk_bc28_nbiot.h"
#include "stm32f4xx_hal.h"


static int nbiot_app_status =  NBIOT_INFO;
static nbiot_handle_t  nbiot_handle = NULL;  


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
          printf("%s cmd failed\r\n",msg);
          
          break;                     
        }          
      
        case NBIOT_MSG_INIT:
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
             printf("\r\nmsg creg status=%s\r\n", msg);
                                        
        }
        break;
                  
        case NBIOT_MSG_CSQ:
          printf("rssi=%sdbm\r\n",msg);      
        break;        
        
        case NBIOT_MSG_SIGNAL:
          printf("signal=%s\r\n",msg);      
        break;
        
        case NBIOT_MSG_INFO:
          
        {
          printf("info get=%s\r\n",msg);
          
          nbiot_app_status =  NBIOT_SIGNAL;                     
        }

        break;

        case NBIOT_MSG_BAND:
             printf("\r\nfreq=%s\r\n",msg);
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
          printf("\r\nudp_cr=%s\r\n",msg);
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
//nbiot_app_status；nbiot所处的主状态阶段
static void nbiot_app_status_poll(nbiot_handle_t nbiot_handle, int *nbiot_app_status)
{    
    switch(*nbiot_app_status)
    {
    case NBIOT_APP_NONE:
      {
        
      }
      break;
           
    case NBIOT_APP_INIT:
      {
        printf("atk_nbiot init start\r\n");
                
        nbiot_reg_init(nbiot_handle);        

        *nbiot_app_status = NBIOT_END;
      }
      break;
      
    case NBIOT_APP_RESET:
      {
        printf("atk_nbiot reboot start\r\n");
                
        nbiot_reg_init(nbiot_handle);        

        *nbiot_app_status = NBIOT_END;
      }
      break;

    case NBIOT_APP_NCONFIG:
      {
        printf("atk_nbiot auto reg start\r\n");
                
        nbiot_reg_init(nbiot_handle);        

        *nbiot_app_status = NBIOT_END;
      }
      break;       
      
    case NBIOT_APP_INFO:
      {
         printf("atk_nbiot get signal start\r\n");
                
         nbiot_info_get(nbiot_handle);

         *nbiot_app_status = NBIOT_END;
      }
            
      break;
      
    case NBIOT_APP_SIGNAL:
      {
        printf("atk_nbiot rssi(db) start\r\n");
        
        nbiot_signal_get(nbiot_handle);
        
        *nbiot_app_status = NBIOT_END;
      }
      break;
      
        
    case NBIOT_APP_CoAP_SEVER:
      {
        printf("CoAP Server set start\r\n");

        *nbiot_app_status = NBIOT_END;
      }
      break;
      
    case NBIOT_APP_CoAP_SEND:
      {
        printf("CoAP send start\r\n");
        *nbiot_app_status = NBIOT_END;
        
        
      }
      break;
      
    case NBIOT_APP_CoAP_RECV:
      {
        printf("CoAP recv start\r\n");
        *nbiot_app_status = NBIOT_END;        
      }
      break;  
      
    default:
      {
        
      }
      break;
    }
}


/**
  * @brief  The demo nbiot nblot info get application entry point.
  *
  * @retval None
  */
void demo_nbiot_info_get_entry(void)
{ 
       
    uart_handle_t uart_handle = NULL; 
 
    uart_handle = atk_nbiot_uart_init(9600);  
    
    nbiot_handle = nbiot_dev_init(uart_handle);
     
    nbiot_event_registercb(nbiot_handle, __nbiot_msg_cb_handler, nbiot_handle);
    
             
    while (1)
    {   
        nbiot_app_status_poll(nbiot_handle, &nbiot_app_status);      
        nbiot_event_poll(nbiot_handle);      
        uart_event_poll(uart_handle);       
    }
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

