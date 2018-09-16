 /************************************************
 Copyright (c) 广州市星翼电子科技有限公司 2014-2024
 All rights reserved 
 ALIENTEK 阿波罗STM32F429开发板 
 NBIOT 温湿度传感器数据上传华为物联网IOT平台实验
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
#include "atk_key.h"
#include "dht11.h"
#include "lcd.h"
#include "pcf8574.h"
#include "atk_soft_timer.h"


#define NBIOT_APP_DEBUG  
#ifdef NBIOT_APP_DEBUG
#define NBIOT_APP_DEBUG_INFO(...)    (int)printf(__VA_ARGS__)    
#else
#define NBIOT_APP_DEBUG_INFO(...)
#endif


static int nbiot_app_status = NBIOT_APP_NCONFIG;
u8 temperature = 0; 
u8 humidity    = 0; 
u8 dht11_flag  = 0;


//nbiot消息事件处理函数
static void __nbiot_msg_cb_handler (void *p_arg, int msg_id, int len, char *msg)
{ 
    nbiot_handle_t nbiot_handle = (nbiot_handle_t)p_arg; 
    
    (void)nbiot_handle;
    
    switch(msg_id)
    {
        case NBIOT_MSG_CMD_NEXT:
            NBIOT_APP_DEBUG_INFO("msg %s cmd error but next\r\n",msg);         ;        
            break;
        
        case NBIOT_MSG_CMD_RETRY:
             NBIOT_APP_DEBUG_INFO("msg %s cmd error but try\r\n",msg);      
             break;        
        
        case NBIOT_MSG_CMD_FAIL:
        
            NBIOT_APP_DEBUG_INFO("msg %s cmd failed\r\n",msg);
          
            break;                     
        
        
        case NBIOT_MSG_INIT:
        {
            NBIOT_APP_DEBUG_INFO("init=%s\r\n",msg);
          
            //连接iot平台时必须先得到imei码
            nbiot_app_status = NBIOT_INFO; 
            break;
                     
        }
            
        
        case NBIOT_MSG_RESET:        //NB复位完成消息
        {
            nbiot_app_status =  NBIOT_APP_INIT;
            NBIOT_APP_DEBUG_INFO("reboot=%s\r\n",msg); 
            break;  
        }
          
            
        
        case NBIOT_MSG_NCONFIG:      //自动入网设置完成消息 
        {
            nbiot_app_status =  NBIOT_APP_RESET;
            NBIOT_APP_DEBUG_INFO("nconfig=%s\r\n",msg); 
            break;  
        }
               
        case NBIOT_MSG_IMSI:
        {
            NBIOT_APP_DEBUG_INFO("\r\nIMSI=%s\r\n",msg); 
            break;
        }
                                    
        case NBIOT_MSG_REG:
        {
            NBIOT_APP_DEBUG_INFO("\r\nmsg reg status=%s\r\n", msg);
            break;
                                                     
        }
               
        case NBIOT_MSG_SIGNAL:
        {         
            NBIOT_APP_DEBUG_INFO("rssi=%sdbm\r\n",msg);  
            break;
        }
                             
        case NBIOT_MSG_INFO:
          
        {
            NBIOT_APP_DEBUG_INFO("info get=%s\r\n",msg);
            //跳到创建NCDP客户端
            nbiot_app_status = NBIOT_NCDP_SERVER;  
            break;                   
        }

        case NBIOT_MSG_CSCON:
        {
            NBIOT_APP_DEBUG_INFO("\r\n cscon status=%d\r\n",*msg);
          
            switch(*msg) 
            {
                case 0:

                    NBIOT_APP_DEBUG_INFO("Idle status\r\n");

                    break;                

                case 1:

                    NBIOT_APP_DEBUG_INFO("Connect status\r\n");

                    break;             

                default:

                    break;                
            }          
                               
            break;
        } 

        case NBIOT_MSG_CSCON_STATUS:
        {
            NBIOT_APP_DEBUG_INFO("\r\ncscon status=%d\r\n",*msg);
            switch(*msg) 
            {
                case 0:

                    NBIOT_APP_DEBUG_INFO("Idle status\r\n");

                    break;                

                case 1:

                    NBIOT_APP_DEBUG_INFO("Connect status\r\n");

                    break;             

                default:

                    break;                
            }          
                       
                               
            break;
        }
        
       
        case NBIOT_MSG_BAND:
            NBIOT_APP_DEBUG_INFO("\r\nFreq=%s\r\n",msg);
            break;
        
        //产商ID
        case NBIOT_MSG_MID:
        {
            NBIOT_APP_DEBUG_INFO("\r\nmid=%s\r\n",msg);
            break;
        }
        
        
        //模块型号
        case NBIOT_MSG_MMODEL:
        {
            NBIOT_APP_DEBUG_INFO("\r\nmmodel=%s\r\n",msg); 
            break; 
        }
               

        //软件版本号
        case NBIOT_MSG_MREV:
        {
            NBIOT_APP_DEBUG_INFO("\r\nmrev=%s\r\n",msg);
            break;
        }
                
        
        case NBIOT_MSG_IMEI:
        {
            NBIOT_APP_DEBUG_INFO("\r\nimei=%s\r\n",msg); 
            break;
        }
        
        
        case NBIOT_MSG_TCPUDP_CREATE:
        {
            NBIOT_APP_DEBUG_INFO("\r\n%s msg create and connect\r\n",msg);
            nbiot_app_status = NBIOT_TCPUDP_SEND;
            break;
        }
        
        
        case NBIOT_MSG_TCPUDP_CLOSE:
        {
            NBIOT_APP_DEBUG_INFO("\r\nmsg tcpudp close=%s\r\n",msg);
            break;
        }
                
        case NBIOT_MSG_TCPUDP_SEND:
        {
            NBIOT_APP_DEBUG_INFO("\r\nmsg tcp udp_send=%s\r\n",msg);
            break;
          
        }
              
        case NBIOT_MSG_TCPUDP_RECV:
        {
            NBIOT_APP_DEBUG_INFO("\r\nmsg udp recv=%s\r\n",msg);
          
            nbiot_app_status = NBIOT_TCPUDP_CL;
            break;
        }
        
                          
        case NBIOT_MSG_NCDP_SERVER:
        {
            NBIOT_APP_DEBUG_INFO("\r\nmsg ncdp server =%s\r\n",msg);
          
            break;
        }
     
        
        case NBIOT_MSG_NCDP_SEND:
        {
            NBIOT_APP_DEBUG_INFO("\r\nmsg ncdp sent=%s\r\n",msg);  
            break;
          
        }
        
        case NBIOT_MSG_NCDP_RECV:
        {
            NBIOT_APP_DEBUG_INFO("\r\n msg ncdp recv=%s\r\n",msg);
          
                        
            break;
        }
              
        case NBIOT_MSG_NCDP_STATUS:
        {
            NBIOT_APP_DEBUG_INFO("\r\nmsg ncdp status=%d\r\n",*msg);
                 
            switch(*msg) 
            {
                case 0:

                    NBIOT_APP_DEBUG_INFO("Register completed\r\n");

                    break;                

                case 1:

                    NBIOT_APP_DEBUG_INFO("Deregister completed\r\n");

                    break;

                case 2:

                    NBIOT_APP_DEBUG_INFO("Registration status updated\r\n");

                    break;
                  
                case 3:
                    NBIOT_APP_DEBUG_INFO("Object 19/0/0 observe completed\r\n");


                    //此时才算注册成功，发送数据
                    nbiot_app_status = NBIOT_NCDP_SEND; 

                    break; 

                case 4:
                    NBIOT_APP_DEBUG_INFO("Bootstrap completed\r\n");
                    break; 

                case 5:
                    NBIOT_APP_DEBUG_INFO("5/0/3 resource observe completed\r\n");
                    break;

                case 6:
                    NBIOT_APP_DEBUG_INFO("Notify the device to receive update package URL\r\n");

                    break; 

                case 7:
                    NBIOT_APP_DEBUG_INFO("Notify the device download has been completed\r\n");
                    break; 

                case 8:
                    NBIOT_APP_DEBUG_INFO("Cancel object 19/0/0 observe\r\n");
                    break;              

                default:

                    break;                
            }
            
            break; 
        }        
                      
        case NBIOT_MSG_NCDP_CLOSE:
        {
            NBIOT_APP_DEBUG_INFO("\r\nmsg ncdp close=%s\r\n",msg);
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
            NBIOT_APP_DEBUG_INFO("atk_nbiot init start\r\n");
                    
            nbiot_init(nbiot_handle);        

            *nbiot_app_status = NBIOT_END;
            break;
        }
                    
        case NBIOT_APP_RESET:
        {
            NBIOT_APP_DEBUG_INFO("atk_nbiot reboot start\r\n");
                    
            nbiot_reboot(nbiot_handle);        

            *nbiot_app_status = NBIOT_END;
            break;
        }
          
        case NBIOT_APP_NCONFIG:
        {
            NBIOT_APP_DEBUG_INFO("atk_nbiot auto reg start\r\n");
                    
            nbiot_nconfig(nbiot_handle, 0);        

            *nbiot_app_status = NBIOT_END;
            break;
        }
                     
        case NBIOT_APP_INFO:
        {
             NBIOT_APP_DEBUG_INFO("atk_nbiot get signal start\r\n");
                    
             nbiot_info_get(nbiot_handle);

             *nbiot_app_status = NBIOT_END;
            
             break;
        }
                                   
        case NBIOT_APP_SIGNAL:
        {
            NBIOT_APP_DEBUG_INFO("atk_nbiot rssi(db) start\r\n");
            
            nbiot_signal_get(nbiot_handle);
            
            *nbiot_app_status = NBIOT_END; 
            break;
        }
                         
        case NBIOT_APP_NCDP_SERVER:
        {
            NBIOT_APP_DEBUG_INFO("NCDP server set start\r\n");
            
            nbiot_ncdp_update(nbiot_handle, NBIOT_NCDP); 

            *nbiot_app_status = NBIOT_END; 
              
            break;
        }
                       
        case NBIOT_APP_NCDP_SEND:
        {
            char dht11_src_buf[10] = {0};
            char dht11_dest_buf[20] = {0};  

            
                                            
            NBIOT_APP_DEBUG_INFO("NCDP send start\r\n");
                                                             
            PCF8574_ReadBit(BEEP_IO);                   //读取一次PCF8574的任意一个IO，使其释放掉PB12引脚，
                                                        //否则读取DHT11可能会出问题            
            DHT11_Read_Data(&temperature, &humidity);    //读取温湿度值 
                               
            snprintf(dht11_src_buf,
                     sizeof(dht11_src_buf) - 1,
                     "%d%d",
                     temperature, humidity); 
                                
            
            //转换数据格式                     
            nbiot_srcbuf2hexchrbuf(dht11_src_buf, dht11_dest_buf, strlen(dht11_src_buf)); 
                                        
                     
            //发送当前温湿度到iot平台            
            nbiot_ncdp_send_hex(nbiot_handle, strlen(dht11_dest_buf), dht11_dest_buf, NBIOT_NCDP, NULL);                      
                      
            *nbiot_app_status = NBIOT_END; 
            
            break;        
        }
                    
        case NBIOT_APP_NCDP_RECV:
        {
            NBIOT_APP_DEBUG_INFO("NCDP recv start\r\n");
            *nbiot_app_status = NBIOT_END;
            break;          
        }
          
        case NBIOT_APP_NCDP_CL:
        {
            NBIOT_APP_DEBUG_INFO("NCDP close start\r\n");

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
            NBIOT_APP_DEBUG_INFO("key0 press\r\n");
            nbiot_app_status = NBIOT_NCDP_SEND;
            break;
        
        case KEY1_PRES://KEY1按下,写入sector
            NBIOT_APP_DEBUG_INFO("key1 press\r\n");
            nbiot_app_status = NBIOT_NCDP_CL;
            break;
        case KEY2_PRES://KEY2按下,恢复sector的数据
            NBIOT_APP_DEBUG_INFO("key2 press\r\n");
            break;
        
        case WKUP_PRES://KEY2按下,恢复sector的数据
            NBIOT_APP_DEBUG_INFO("WKUP_PRES press\r\n");
            break;        
        
    }   
    
}



/**
  * @brief  定时回调更新dht11采集到的温湿度的值.
  */
static void __dht11_timer_callback (void *p_arg)
{   
    dht11_flag = 1;
}




/**
  * @brief  The demo dht11 iot entry entry point.
  *
  * @retval None
  */
void demo_dht11_entry(void)
{         
  
    struct atk_soft_timer dht11_timer;

    uart_handle_t uart_handle = NULL; 

    nbiot_handle_t  nbiot_handle = NULL; 

    key_handle_t  key_handle = NULL;

    key_handle = atk_key_exit_init(); 
    
    atk_key_registercb(key_handle, key_event_handle, NULL);  
   
    uart_handle = atk_nbiot_uart_init(9600);  
    
    nbiot_handle = nbiot_dev_init(uart_handle);
     
    nbiot_event_registercb(nbiot_handle, __nbiot_msg_cb_handler, nbiot_handle); 
    
    PCF8574_Init();                 //初始化PCF8574
    POINT_COLOR=RED;
    LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
    LCD_ShowString(30,70,200,16,16,"NBIOT DHT11 ");    
    LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
    LCD_ShowString(30,110,200,16,16,"2018/9/13");    
    PCF8574_ReadBit(BEEP_IO);       //由于DHT11和PCF8574的中断引脚共用一个IO，
                                    //所以在初始化DHT11之前要先读取一次PCF8574的任意一个IO，
                                    //使其释放掉中断引脚所占用的IO(PB12引脚),否则初始化DS18B20会出问题    
    while(DHT11_Init())    //DHT11初始化    
    {
        LCD_ShowString(30,130,200,16,16,"DHT11 Error");
        delay_ms(200);
        LCD_Fill(30,130,239,130+16,WHITE);
        delay_ms(200);
    }
    
    LCD_ShowString(30,130,200,16,16,"DHT11 OK");
    POINT_COLOR=BLUE;//设置字体为蓝色 
    LCD_ShowString(30,150,200,16,16,"Temp:  C");
    LCD_ShowString(30,170,200,16,16,"Humi:  %");   
    
    //软件定时器200ms周期定时
    atk_soft_timer_init(&dht11_timer, __dht11_timer_callback, NULL, 200, 200); 
    atk_soft_timer_start(&dht11_timer);                                         
    
    while(1)
    {                       
        nbiot_app_status_poll(nbiot_handle, &nbiot_app_status);      
        nbiot_event_poll(nbiot_handle);      
        uart_event_poll(uart_handle);         
        atk_key_event_poll(key_handle); 

        if (dht11_flag == 1)
        {            
            PCF8574_ReadBit(BEEP_IO);                   //读取一次PCF8574的任意一个IO，使其释放掉PB12引脚，
                                                        //否则读取DHT11可能会出问题            
            DHT11_Read_Data(&temperature,&humidity);    //读取温湿度值                        
            LCD_ShowNum(30+40,150,temperature,2,16);    //显示温度                  
            LCD_ShowNum(30+40,170,humidity,2,16);       //显示湿度
            dht11_flag = 0;
            LED1=!LED1;            
        }
        
    }
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
