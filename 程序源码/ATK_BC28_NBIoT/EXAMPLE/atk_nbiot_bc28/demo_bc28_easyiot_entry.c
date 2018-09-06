/**
  *
  * @file           : demo_sim7020_udp_entry.c
  * @brief          : sim7020 电信iot平台对接数据实验
  */
/* Includes ------------------------------------------------------------------*/
#include "atk_sys.h"
#include "atk_led.h"
#include "atk_delay.h"
#include "atk_bc28_nbiot.h"
#include "atk_bc28.h"
#include "atk_bc28_nbiot.h"
#include "atk_key.h"
#include "stm32l4xx_hal.h"

static int sm7020_main_status = SIM7020_NBLOT_INIT;

//sim7020消息事件处理函数
static void __sim7020_event_cb_handler (void *p_arg, int msg_id, int len, char *msg)
{ 
    sim7020_handle_t sim7020_handle = (sim7020_handle_t)p_arg; 
    
    (void)sim7020_handle;
    
    switch(msg_id)
    {
        case SIM7020_MSG_CMD_NEXT:
          printf("msg %s cmd error but next\r\n",msg);         ;        
        break;
        
        case SIM7020_MSG_CMD_RETRY:
          printf("msg %s cmd error but try\r\n",msg);      
        break;        
        
        case SIM7020_MSG_CMD_FAIL:
        {
          printf("msg %s cmd failed\r\n",msg);
          
          break;                     
        }
        
        case SIM7020_MSG_NBLOT_INIT:
        {
          printf("init=%s\r\n",msg);
          
          //连接iot平台时必须先得到imie码
          sm7020_main_status = SIM7020_NBLOT_INFO;
                     
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
           //跳到创建CM2M客户端
           sm7020_main_status = SIM7020_CM2M_CLIENT;                     
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
          printf("\r\nmsg COAP server=%s\r\n",msg);
    
        }
        break;
        
        case  SIM7020_MSG_COAP_CLIENT:
          printf("\r\nmsg COAP client =%s\r\n",msg);
    
          
        break;
     
        
        case SIM7020_MSG_COAP_SEND:
        {
          printf("\r\nmsg COAP_SENT=%s\r\n",msg);
          
        }
        break;

        case SIM7020_MSG_COAP_RECV:
        {
            printf("\r\n msg COAP_RECV=%s\r\n",msg);

        }
        break;
        
        
        case SIM7020_MSG_COAP_CLOSE:
        {
            printf("\r\nmsg coap close=%s\r\n",msg);
        }
        break;

        case  SIM7020_MSG_CM2M_CLIENT:
          printf("\r\nmsg cm2m client =%s\r\n",msg);
          sm7020_main_status = SIM7020_CM2M_SEND; 
          
        break;
     
        
        case SIM7020_MSG_CM2M_SEND:
        {
          printf("\r\nmsg cm2m sent=%s\r\n",msg);
          
        }
        break;

        case SIM7020_MSG_CM2M_RECV:
        {
            printf("\r\n msg cm2m recv=%s\r\n",msg);
          
            printf("\r\n msg cm2m recv=%s\r\n",msg);
          
            
      
                                
            //代表该命令执行成功
            //sim7020_nblot_cm2m_send_hex(sim7020_handle, strlen(cmd_rsp), cmd_rsp, SIM7020_CM2M);                       
                                               
        }
        break;
        
        case SIM7020_MSG_CM2M_STATUS:
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
        
        case SIM7020_MSG_CM2M_CLOSE:
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
        printf("atk_sim7020 init start\r\n");
                
        sim7020_nblot_init(sim7020_handle);        

        *sim7020_main_status = SIM7020_END;
      }
      break;
      
    case SIM7020_NBLOT_INFO:
      {
         printf("atk_sim7020 get signal start\r\n");
                
         sim7020_nblot_info_get(sim7020_handle);

         *sim7020_main_status = SIM7020_END;
      }
            
      break;
      
    case SIM7020_SIGNAL:
      {
        printf("atk_sim7020 rssi(db) start\r\n");
        
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
        
        sim7020_nblot_coap_server_create(sim7020_handle, SIM7020_COAP); 

        *sim7020_main_status = SIM7020_END;
      }
      break;
      
      
    case SIM7020_CoAP_CLIENT:
      {
        printf("CoAP client set start\r\n");
        
        sim7020_nblot_coap_client_create(sim7020_handle, SIM7020_COAP); 

        *sim7020_main_status = SIM7020_END;
      }
      break;    
      
    case SIM7020_CoAP_SEND:
      {
        printf("CoAP send start\r\n");      
        
//        sim7020_nblot_coap_send_str(sim7020_handle, 15, "400141C7B7636F756E746572FF0001", SIM7020_COAP); 
        sim7020_nblot_coap_send_str(sim7020_handle, 15 + strlen("ep=868334030037430&pw=909026"), "400141C7B7636F756E746572FF65703d3836383333343033303033373433302670773d393039303236", SIM7020_COAP); 
                  
        *sim7020_main_status = SIM7020_END;               
      }
      break;
      
    case SIM7020_CoAP_RECV:
      {
        printf("CoAP recv start\r\n");
        *sim7020_main_status = SIM7020_END;        
      }
      
    case SIM7020_CoAP_CL:
      {
        printf("CoAP close start\r\n");
        sim7020_nblot_coap_close(sim7020_handle, SIM7020_CM2M);
        *sim7020_main_status = SIM7020_END;        
      }      
      
      break; 

    case SIM7020_CM2M_CLIENT:
      {
        printf("CM2M client set start\r\n");
        
        sim7020_nblot_cm2m_client_create(sim7020_handle, SIM7020_CM2M); 

        *sim7020_main_status = SIM7020_END;
      }
      break;    
      
    case SIM7020_CM2M_SEND:
      {
        printf("CM2M send start\r\n");
        
        //创建完成CM2M客户端之后，需要根据当前网络的状态延时一段时间保证数据连接稳定
        delay_ms(5000);

        //easyiot平台数据
        char *easyiot_buf="01F00035000150FFFFFFCE313233343536373839303132333435373834303139323833373531303233000001658EE53A4001000401000101B5";       
               
        sim7020_nblot_cm2m_send_hex(sim7020_handle, strlen(easyiot_buf), easyiot_buf, SIM7020_CM2M); 
                  
        *sim7020_main_status = SIM7020_END;               
      }
      break;
      
    case SIM7020_CM2M_RECV:
      {
        printf("CM2M recv start\r\n");
        *sim7020_main_status = SIM7020_END;        
      }
      
    case SIM7020_CM2M_CL:
      {
        printf("CM2M close start\r\n");
        //创建完成CM2M客户端之后，需要根据当前网络的状态延时一段时间保证数据连接稳定
        delay_ms(2000);
        sim7020_nblot_cm2m_close(sim7020_handle, SIM7020_CM2M);
        *sim7020_main_status = SIM7020_END;        
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
            sm7020_main_status = SIM7020_CM2M_SEND;
            break;
        
        case KEY1_PRES://KEY1按下,写入sector
            printf("key1 press\r\n");
            sm7020_main_status = SIM7020_CM2M_CL;
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
  * @brief  The demo sim7020 iot entry entry point.
  *
  * @retval None
  */
void demo_sim7020_easyiot_entry(void)
{         
    uart_handle_t nbiot_handle = NULL; 

    sim7020_handle_t  sim7020_handle = NULL; 
    key_handle_t  key_handle = NULL;

    key_handle = key_init(1);
    
    atk_key_registercb(key_handle, key_event_handle, NULL);  
  
    nbiot_handle = atk_nbiot_uart_init(115200);  
    
    sim7020_handle = sim7020_init(nbiot_handle);
     
    sim7020_event_registercb(sim7020_handle, __sim7020_event_cb_handler, sim7020_handle);
    
    //sim7020上电需要等待10s
    delay_ms(1000);
             
    while (1)
    {
        sim7020_app_status_poll(sim7020_handle, &sm7020_main_status);      
        sim7020_event_poll(sim7020_handle);      
        uart_event_poll(nbiot_handle);         
        atk_key_event_poll(key_handle);
    }
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
