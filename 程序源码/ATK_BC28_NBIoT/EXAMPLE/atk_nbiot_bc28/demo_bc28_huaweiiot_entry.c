/**
  *
  * @file           : demo_nbiot_udp_entry.c
  * @brief          : nbiot 电信iot平台对接数据实验
  */
/* Includes ------------------------------------------------------------------*/
#include "atk_sys.h"
#include "atk_led.h"
#include "atk_delay.h"
#include "atk_nbiot_usart.h"
#include "atk_bc28_nbiot.h"
#include "atk_bc28.h"
#include "atk_key.h"
#include "stm32l4xx_hal.h"

static int nbiot_main_status = NBIOT_INIT;

//nbiot消息事件处理函数
static void __nbiot_event_cb_handler (void *p_arg, int msg_id, int len, char *msg)
{ 
    nbiot_handle_t nbiot_handle = (nbiot_handle_t)p_arg; 
    
    (void)nbiot_handle;
    
    switch(msg_id)
    {
        case NBIOT_MSG_CMD_NEXT:
          printf("msg %s cmd error but next\r\n",msg);         ;        
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
          
          //连接iot平台时必须先得到imie码
          nbiot_main_status = NBIOT_INFO;
                     
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
           //跳到创建CM2M客户端
           nbiot_main_status = NBIOT_CM2M_CLIENT;                     
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
    
        }
        break;
        
        case  NBIOT_MSG_COAP_CLIENT:
          printf("\r\nmsg COAP client =%s\r\n",msg);
    
          
        break;
     
        
        case NBIOT_MSG_COAP_SEND:
        {
          printf("\r\nmsg COAP_SENT=%s\r\n",msg);
          
        }
        break;

        case NBIOT_MSG_COAP_RECV:
        {
            printf("\r\n msg COAP_RECV=%s\r\n",msg);

        }
        break;
        
        
        case NBIOT_MSG_COAP_CLOSE:
        {
            printf("\r\nmsg coap close=%s\r\n",msg);
        }
        break;

        case  NBIOT_MSG_CM2M_CLIENT:
          printf("\r\nmsg cm2m client =%s\r\n",msg);
          nbiot_main_status = NBIOT_CM2M_SEND; 
          
        break;
     
        
        case NBIOT_MSG_CM2M_SEND:
        {
          printf("\r\nmsg cm2m sent=%s\r\n",msg);
          
        }
        break;

        case NBIOT_MSG_CM2M_RECV:
        {
            printf("\r\n msg cm2m recv=%s\r\n",msg);
          
            
            char recv_buf[12] = {'0','4', '0', '0'};
            char cmd_rsp[12] = {'0','4', '0', '0'};
                             
            nbiot_srcbuf2hex(msg, recv_buf, strlen(msg));
          
            //是LED灯的消息
            if (recv_buf[0] == 0x02)
            {
                if (recv_buf[1] == 0x01) 
                {
                    LED0(1);
                }
                else
                {
                    LED0(0);
                  
                }
                
              memcpy(&cmd_rsp[4],  &msg[4], 4);
                
              cmd_rsp[8] = '0';
                
              cmd_rsp[9] = '0';  
                                
              //代表该命令执行成功
              nbiot_cm2m_send_hex(nbiot_handle, strlen(cmd_rsp), cmd_rsp, NBIOT_CM2M);                       
            }
            
            //是蜂鸣器的消息
            else if (recv_buf[0] == 0x03)  
            {
              
                if (msg[1] == 0x01) 
                {
                    LED0(1);
                }
                else
                {
                    LED0(0);
                }              
              
            }
            
            else 
            {
            
            
            }
            
      
            
            //关闭连接
//            nbiot_main_status = NBIOT_CM2M_CL; 
        }
        break;
        
        case NBIOT_MSG_CM2M_STATUS:
        {
            printf("\r\nmsg cm2m status=%d\r\n",*msg);
                 
            switch(*msg) 
            {
              case 1:
                
                printf("connect iot success , Take the initiative to report\r\n");
               
                break;

              case 2:
                
                printf("connect iot update , Take the initiative to report\r\n");
              
                break;
              
              case 3:
                printf("connect iot break , Take the initiative to report\r\n");
              
                break; 
              
              case 4:
                printf("recv connect iot observe msg , Take the initiative to report\r\n");
                break; 

              case 5:
                printf("send data to iot , Take the initiative to report\r\n");
                break;
              
              default:
                
                break;                
            }
        }        
        break;        
        
        case NBIOT_MSG_CM2M_CLOSE:
        {
            printf("\r\nmsg cm2m close=%s\r\n",msg);
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
                
        nbiot_init(nbiot_handle);        

        *nbiot_main_status = NBIOT_END;
      }
      break;

    case NBIOT_APP_NCONFIG:
      {
        printf("atk_nbiot auto reg start\r\n");
                
        nbiot_init(nbiot_handle);        

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
      
    case NBIOT_CoAP_CL:
      {
        printf("CoAP close start\r\n");
        nbiot_coap_close(nbiot_handle, NBIOT_CM2M);
        *nbiot_main_status = NBIOT_END;        
      }      
      
      break; 

    case NBIOT_APP_CM2M_CLIENT:
      {
        printf("CM2M client set start\r\n");
        
        nbiot_cm2m_client_create(nbiot_handle, NBIOT_CM2M); 

        *nbiot_main_status = NBIOT_END;
      }
      break;    
      
    case NBIOT_APP_CM2M_SEND:
      {
        printf("CM2M send start\r\n");
        
        //创建完成CM2M客户端之后，需要根据当前网络的状态延时一段时间保证数据连接稳定
        delay_ms(5000);        
               
        nbiot_cm2m_send_hex(nbiot_handle, strlen("00023132"), "00023132", NBIOT_CM2M); 
                  
        *nbiot_main_status = NBIOT_END;               
      }
      break;
      
    case NBIOT_APP_CM2M_RECV:
      {
        printf("CM2M recv start\r\n");
        *nbiot_main_status = NBIOT_END;        
      }
      
    case NBIOT_APP_CM2M_CL:
      {
        printf("CM2M close start\r\n");
        //创建完成CM2M客户端之后，需要根据当前网络的状态延时一段时间保证数据连接稳定
        delay_ms(2000);
        nbiot_cm2m_close(nbiot_handle, NBIOT_CM2M);
        *nbiot_main_status = NBIOT_END;        
      }      
      
      break;  
      
      
    default:
      {
        
      }
      break;
    }
}


static void key_event_handle(u32 key_event,void *p_arg)
{   
  
    switch(key_event)
    {
        case KEY0_PRES://KEY0按下,再一次发送数据
            printf("key0 press\r\n");
            nbiot_main_status = NBIOT_CM2M_SEND;
            break;
        
        case KEY1_PRES://KEY1按下,写入sector
            printf("key1 press\r\n");
            nbiot_main_status = NBIOT_CM2M_CL;
            break;
        case KEY2_PRES://KEY2按下,恢复sector的数据
            printf("key2 press\r\n");
            break;
        
        case WKUP_PRES://KEY2按下,恢复sector的数据
            printf("WKUP_PRES press\r\n");
            break;        
        
    }   
    
}

/**
  * @brief  The demo nbiot huaweiiot entry entry point.
  *
  * @retval None
  */
void demo_nbiot_huaweiiot_entry(void)
{         
    uart_handle_t uart_handle = NULL; 

    nbiot_handle_t  nbiot_handle = NULL; 

    key_handle_t  key_handle = NULL;

    key_handle = key_init(1); 
    
    atk_key_registercb(key_handle, key_event_handle, NULL);  
   
    uart_handle = atk_nbiot_uart_init(115200);  
    
    nbiot_handle = nbiot_dev_init(uart_handle);
     
    nbiot_event_registercb(nbiot_handle, __nbiot_event_cb_handler, nbiot_handle);
    
    //nbiot上电需要等待10s
    delay_ms(1000);
             
    while (1)
    {
        nbiot_app_status_poll(nbiot_handle, &nbiot_main_status);      
        nbiot_event_poll(nbiot_handle);      
        uart_event_poll(uart_handle);         
        atk_key_event_poll(key_handle);
    }
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
