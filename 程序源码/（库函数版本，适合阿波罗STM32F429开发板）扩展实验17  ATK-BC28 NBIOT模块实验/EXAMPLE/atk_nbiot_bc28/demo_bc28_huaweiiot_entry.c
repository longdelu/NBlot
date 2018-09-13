/************************************************
 Copyright (c) 广州市星翼电子科技有限公司 2014-2024
 All rights reserved 
 ALIENTEK 阿波罗STM32F429开发板 
 NBIOT华为OC物联网IOT平台数据透传实现
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/ 

#include "atk_sys.h"
#include "atk_led.h"
#include "atk_delay.h"
#include "atk_nbiot_usart.h"
#include "atk_bc28_nbiot.h"
#include "atk_bc28.h"
#include "atk_key.h"
#include "stm32f4xx_hal.h"


static int nbiot_app_status = NBIOT_APP_NCONFIG;

//nbiot消息事件处理函数
static void __nbiot_msg_cb_handler (void *p_arg, int msg_id, int len, char *msg)
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
        
            printf("msg %s cmd failed\r\n",msg);
          
            break;                     
        
        
        case NBIOT_MSG_INIT:
        {
            printf("init=%s\r\n",msg);
          
            //连接iot平台时必须先得到imei码
            nbiot_app_status = NBIOT_INFO; 
            break;
                     
        }
            
        
        case NBIOT_MSG_RESET:        //NB复位完成消息
        {
            nbiot_app_status =  NBIOT_APP_INIT;
            printf("reboot=%s\r\n",msg); 
            break;  
        }
          
            
        
        case NBIOT_MSG_NCONFIG:      //自动入网设置完成消息 
        {
            nbiot_app_status =  NBIOT_APP_RESET;
            printf("nconfig=%s\r\n",msg); 
            break;  
        }

                 

        case NBIOT_MSG_IMSI:
        {
            printf("\r\nIMSI=%s\r\n",msg); 
            break;
        }
                                    
        case NBIOT_MSG_REG:
        {
            printf("\r\nmsg reg status=%s\r\n", msg);
            break;
                                                     
        }
               
        case NBIOT_MSG_SIGNAL:
        {         
            printf("rssi=%sdbm\r\n",msg);  
            break;
        }
                             
        case NBIOT_MSG_INFO:
          
        {
            printf("info get=%s\r\n",msg);
            //跳到创建NCDP客户端
            nbiot_app_status = NBIOT_NCDP_SERVER;  
            break;                   
        }

       
        case NBIOT_MSG_BAND:
            printf("\r\nFreq=%s\r\n",msg);
            break;
        
        //产商ID
        case NBIOT_MSG_MID:
        {
            printf("\r\nmid=%s\r\n",msg);
            break;
        }
        
        
        //模块型号
        case NBIOT_MSG_MMODEL:
        {
            printf("\r\nmmodel=%s\r\n",msg); 
            break; 
        }
               

        //软件版本号
        case NBIOT_MSG_MREV:
        {
            printf("\r\nmrev=%s\r\n",msg);
            break;
        }
                
        
        case NBIOT_MSG_IMEI:
        {
            printf("\r\nimei=%s\r\n",msg); 
            break;
        }
        
        
        case NBIOT_MSG_TCPUDP_CREATE:
        {
            printf("\r\n%s msg create and connect\r\n",msg);
            nbiot_app_status = NBIOT_TCPUDP_SEND;
            break;
        }
        
        
        case NBIOT_MSG_TCPUDP_CLOSE:
        {
            printf("\r\nmsg tcpudp close=%s\r\n",msg);
            break;
        }
                
        case NBIOT_MSG_TCPUDP_SEND:
        {
            printf("\r\nmsg tcp udp_send=%s\r\n",msg);
            break;
          
        }
              
        case NBIOT_MSG_TCPUDP_RECV:
        {
            printf("\r\nmsg udp recv=%s\r\n",msg);
          
            nbiot_app_status = NBIOT_TCPUDP_CL;
            break;
        }
        
                          
        case NBIOT_MSG_NCDP_SERVER:
             printf("\r\nmsg ncdp =%s\r\n",msg);
          
            break;
     
        
        case NBIOT_MSG_NCDP_SEND:
        {
            printf("\r\nmsg ncdp sent=%s\r\n",msg);  
            break;
          
        }
        
        case NBIOT_MSG_NCDP_RECV:
        {
            printf("\r\n msg ncdp recv=%s\r\n",msg);
          
                        
            break;
        }
              
        case NBIOT_MSG_NCDP_STATUS:
        {
            printf("\r\nmsg ncdp status=%d\r\n",*msg);
                 
            switch(*msg) 
            {
                case 0:

                    printf("Register completed\r\n");

                    break;                

                case 1:

                    printf("Deregister completed\r\n");

                    break;

                case 2:

                    printf("Registration status updated\r\n");

                    break;
                  
                case 3:
                    printf("Object 19/0/0 observe completed\r\n");


                    //此时才算注册成功，发送数据
                    nbiot_app_status = NBIOT_NCDP_SEND; 

                    break; 

                case 4:
                    printf("Bootstrap completed\r\n");
                    break; 

                case 5:
                    printf("5/0/3 resource observe completed\r\n");
                    break;

                case 6:
                    printf("Notify the device to receive update package URL\r\n");

                    break; 

                case 7:
                    printf("Notify the device download has been completed\r\n");
                    break; 

                case 8:
                    printf("Cancel object 19/0/0 observe\r\n");
                    break;              

                default:

                    break;                
            }
            
            break; 
        }        
                      
        case NBIOT_MSG_NCDP_CLOSE:
        {
            printf("\r\nmsg ncdp close=%s\r\n",msg);
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
    switch (*nbiot_app_status)
    {
        case NBIOT_APP_NONE:
        {
            break;
        }
                                      
        case NBIOT_APP_INIT:
        {
            printf("atk_nbiot init start\r\n");
                    
            nbiot_init(nbiot_handle);        

            *nbiot_app_status = NBIOT_END;
            break;
        }
                    
        case NBIOT_APP_RESET:
        {
            printf("atk_nbiot reboot start\r\n");
                    
            nbiot_reboot(nbiot_handle);        

            *nbiot_app_status = NBIOT_END;
            break;
        }
          
        case NBIOT_APP_NCONFIG:
        {
            printf("atk_nbiot auto reg start\r\n");
                    
            nbiot_nconfig(nbiot_handle, 0);        

            *nbiot_app_status = NBIOT_END;
            break;
        }
                     
        case NBIOT_APP_INFO:
        {
             printf("atk_nbiot get signal start\r\n");
                    
             nbiot_info_get(nbiot_handle);

             *nbiot_app_status = NBIOT_END;
            
             break;
        }
                                   
        case NBIOT_APP_SIGNAL:
        {
            printf("atk_nbiot rssi(db) start\r\n");
            
            nbiot_signal_get(nbiot_handle);
            
            *nbiot_app_status = NBIOT_END; 
            break;
        }
                         
        case NBIOT_APP_NCDP_SERVER:
        {
            printf("NCDP server set start\r\n");
            
            nbiot_ncdp_update(nbiot_handle, NBIOT_NCDP); 

            *nbiot_app_status = NBIOT_END; 
              
            break;
        }
                       
        case NBIOT_APP_NCDP_SEND:
        {
            printf("NCDP send start\r\n");
                                  
            nbiot_ncdp_send_hex(nbiot_handle, strlen("00143031323334353637383930313233343536373839"), "00143031323334353637383930313233343536373839", NBIOT_NCDP, NULL); 
                      
            *nbiot_app_status = NBIOT_END; 
            
            break;        
        }
                    
        case NBIOT_APP_NCDP_RECV:
        {
            printf("NCDP recv start\r\n");
            *nbiot_app_status = NBIOT_END;
            break;          
        }
          
        case NBIOT_APP_NCDP_CL:
        {
            printf("NCDP close start\r\n");
            //创建完成NCDP客户端之后，需要根据当前网络的状态延时一段时间保证数据连接稳定
            delay_ms(2000);
              
            nbiot_ncdp_close(nbiot_handle, NBIOT_NCDP);
              
            *nbiot_app_status = NBIOT_END; 
            break;        
        }      
                                          
        default:
        {
            break;
        }
      
    }
}


static void key_event_handle(u32 key_event,void *p_arg)
{   
  
    switch(key_event)
    {
        case KEY0_PRES://KEY0按下,再一次发送数据
            printf("key0 press\r\n");
            nbiot_app_status = NBIOT_NCDP_SEND;
            break;
        
        case KEY1_PRES://KEY1按下,写入sector
            printf("key1 press\r\n");
            nbiot_app_status = NBIOT_NCDP_CL;
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

    key_handle = atk_key_exit_init(); 
    
    atk_key_registercb(key_handle, key_event_handle, NULL);  
   
    uart_handle = atk_nbiot_uart_init(9600);  
    
    nbiot_handle = nbiot_dev_init(uart_handle);
     
    nbiot_event_registercb(nbiot_handle, __nbiot_msg_cb_handler, nbiot_handle);
    
             
    while (1)
    {
        nbiot_app_status_poll(nbiot_handle, &nbiot_app_status);      
        nbiot_event_poll(nbiot_handle);      
        uart_event_poll(uart_handle);         
        atk_key_event_poll(key_handle);
    }
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
