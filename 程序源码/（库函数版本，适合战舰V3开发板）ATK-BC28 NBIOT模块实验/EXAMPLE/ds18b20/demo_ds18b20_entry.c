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
#include "ds18b20.h"
#include "lcd.h"
#include "atk_soft_timer.h"


#define NBIOT_APP_DEBUG  
#ifdef NBIOT_APP_DEBUG
#define NBIOT_APP_DEBUG_INFO(...)    (int)printf(__VA_ARGS__)    
#else
#define NBIOT_APP_DEBUG_INFO(...)
#endif

static int nbiot_app_status = NBIOT_APP_NCONFIG; 
static short temperature = 0; 
static u8 ds18b20_flag  = 0;

/**
  * @brief  nbiot消息事件处理函数
  */
static void __nbiot_msg_cb_handler (void *p_arg, int msg_id, int len, char *msg)
{ 
    nbiot_handle_t nbiot_handle = (nbiot_handle_t)p_arg; 
    
    (void)nbiot_handle;
  
    uint8_t lcd_buf[128] = {0};
    
    switch(msg_id)
    {
        //处理命令出错但跳过该命令消息
        case NBIOT_MSG_CMD_NEXT:
          
            sprintf((char*)lcd_buf,"msg %s err but next", msg);           
            //先清该区域
            LCD_Fill(30,50,30+200,50+16,WHITE);
            LCD_ShowString(30,50,200,16,16, lcd_buf);         
        
            NBIOT_APP_DEBUG_INFO("msg %s err but next\r\n",msg);
        
            break;
        
        //处理命令出错重试消息       
        case NBIOT_MSG_CMD_RETRY:
          
            sprintf((char*)lcd_buf,"msg %s err but try", msg);           
            //先清该区域
            LCD_Fill(30,50,30+200,50+16,WHITE);            
            LCD_ShowString(30,50,200,16,16, lcd_buf);  
        
            NBIOT_APP_DEBUG_INFO("msg %s err but try\r\n",msg);
        
            break;        
        
        //处理命令失败消息
        case NBIOT_MSG_CMD_FAIL: 

            sprintf((char*)lcd_buf,"msg %s failed", msg);           
            //先清该区域
            LCD_Fill(30,50,30+200,50+16,WHITE);            
            LCD_ShowString(30,50,200,16,16, lcd_buf);  
        
            NBIOT_APP_DEBUG_INFO("msg %s failed\r\n",msg);         
            break;                     
        
        //处理nbiot模块初始化完成消息        
        case NBIOT_MSG_INIT:
        {  
            //查询模块信息，得到imei号
            nbiot_app_status = NBIOT_APP_INFO; 
          
            sprintf((char*)lcd_buf,"msg init=%s", msg);           
            //先清该区域
            LCD_Fill(30,50,30+200,50+16,WHITE);            
            LCD_ShowString(30,50,200,16,16, lcd_buf);
          
            NBIOT_APP_DEBUG_INFO("msg init=%s\r\n",msg);
                    
            break;                    
        }
        
        //nbiot复位完成消息         
        case NBIOT_MSG_RESET:        
        {   
            //初始化网络注册
            nbiot_app_status = NBIOT_APP_INIT;           
            sprintf((char*)lcd_buf,"msg reboot=%s", msg); 
            //先清该区域
            LCD_Fill(30,50,30+200,50+16,WHITE);          
            LCD_ShowString(30,50,200,16,16, lcd_buf);          
            NBIOT_APP_DEBUG_INFO("msg reboot=%s\r\n",msg);          
            break;  
        }
        //自动入网设置完成消息                      
        case NBIOT_MSG_NCONFIG:       
        {
            nbiot_app_status = NBIOT_APP_RESET;
            //先清该区域
            LCD_Fill(30,50,30+200,50+16,WHITE);          
            sprintf((char*)lcd_buf,"msg nconfig=%s", msg);                       
            LCD_ShowString(30,50,200,16,16, lcd_buf);           
          
            NBIOT_APP_DEBUG_INFO("msg nconfig=%s\r\n",msg); 
            break;  
        }      
        //处理imsi号消息       
        case NBIOT_MSG_IMSI:
        {
            sprintf((char*)lcd_buf,"msg imsi=%s", msg);                       
            LCD_ShowString(30,110,200,16,16, lcd_buf);            
            NBIOT_APP_DEBUG_INFO("msg imsi=%s\r\n",msg);           
            break;
        }       
        //处理imei号消息
        case NBIOT_MSG_IMEI:
        {
            sprintf((char*)lcd_buf,"msg imei=%s", msg);                       
            LCD_ShowString(30,130,200,16,16, lcd_buf);            
            NBIOT_APP_DEBUG_INFO("msg imei=%s\r\n",msg); 
            break;
        }        
        //处理网络注册消息                            
        case NBIOT_MSG_REG:
        {
             NBIOT_APP_DEBUG_INFO("msg reg status=%d\r\n",*msg);  
            //先清该区域
            LCD_Fill(30,90,30+200,90+16,WHITE);            
            switch(*msg) 
            {
                case 0:
                  
                    LCD_ShowString(30,90,200,16,16, (uint8_t *)"msg reg=failed");
                    NBIOT_APP_DEBUG_INFO("msg reg=failed\r\n");                
                    break;                

                case 1:

                    LCD_ShowString(30,90,200,16,16, (uint8_t *)"msg reg=ok"); 
                    NBIOT_APP_DEBUG_INFO("msg reg=ok\r\n");
                    break;             

                default:

                    break;                
            }         
                                 
            break;                                                     
        }
        //处理查询CSQ消息
        case NBIOT_MSG_CSQ:
            sprintf((char*)lcd_buf,"msg rssi=%sdbm", msg); 
            //先清该区域
            LCD_Fill(30,70,30+200,70+16,WHITE);          
            LCD_ShowString(30,70,200,16,16, lcd_buf);   
          
            NBIOT_APP_DEBUG_INFO("msg rssi=%sdbm\r\n",msg);       
            break;        
        
        //处理查询信号质量消息
        case NBIOT_MSG_SIGNAL:
        {  
            NBIOT_APP_DEBUG_INFO("msg signal get=%s\r\n",msg);
            sprintf((char*)lcd_buf,"msg signal get=%s", msg);
          
            //先清该区域
            LCD_Fill(30,50,30+200,50+16,WHITE);          
            LCD_ShowString(30,50,200,16,16, lcd_buf);  
            break;
        }
        //处理查询模块信息成功消息                     
        case NBIOT_MSG_INFO:         
        {
            //跳到更新cdp服务器,在iot平台注册设备
            nbiot_app_status = NBIOT_APP_NCDP_SERVER;  
            NBIOT_APP_DEBUG_INFO("msg info get=%s\r\n",msg);
            sprintf((char*)lcd_buf,"msg info get=%s", msg);
            //先清该区域
            LCD_Fill(30,50,30+200,50+16,WHITE);          
            LCD_ShowString(30,50,200,16,16, lcd_buf);  
                
            break;                   
        }
        //处理模块连接（活动）状态消息
        case NBIOT_MSG_CSCON_STATUS:
        {
            //先清该区域
            LCD_Fill(30,150,30+200,150+16,WHITE); 
            NBIOT_APP_DEBUG_INFO("msg cscon status=%d\r\n",*msg);
            switch(*msg) 
            {
                case 0:
                    LCD_ShowString(30,150,200,16,16, (uint8_t *)"msg cscon status=idle"); 
                    NBIOT_APP_DEBUG_INFO("msg cscon status=idle\r\n");
                    break;                

                case 1:
                    LCD_ShowString(30,150,200,16,16, (uint8_t *)"msg cscon status=connect");
                    NBIOT_APP_DEBUG_INFO("msg cscon status=connect\r\n");
                    break;             

                default:
                    break;                
            }                                                                
            break;
        }
        
        //带宽消息      
        case NBIOT_MSG_BAND:
            NBIOT_APP_DEBUG_INFO("msg freq=%s\r\n",msg);
            break;
        
        //产商ID消息
        case NBIOT_MSG_MID:
        {
            NBIOT_APP_DEBUG_INFO("msg mid=%s\r\n",msg);
            break;
        }
        
        
        //模块型号消息
        case NBIOT_MSG_MMODEL:
        {
            NBIOT_APP_DEBUG_INFO("msg mmodel=%s\r\n",msg); 
            break; 
        }
               

        //软件版本号
        case NBIOT_MSG_MREV:
        {
            NBIOT_APP_DEBUG_INFO("msg mrev=%s\r\n",msg);
            break;
        }
        
        //cdp服务器更新消息                                                             
        case NBIOT_MSG_NCDP_SERVER:
        {         
            sprintf((char*)lcd_buf,"msg cdp server update=%s", msg); 
            //先清该区域
            LCD_Fill(30,50,30+200,50+16,WHITE);          
            LCD_ShowString(30,50,200,16,16, lcd_buf);            
            NBIOT_APP_DEBUG_INFO("msg cdp server update=%s\r\n",msg);
          
            break;
        }
        
        //业务数据发送消息
        case NBIOT_MSG_NCDP_SEND:
        {
            NBIOT_APP_DEBUG_INFO("msg data send=%s\r\n",msg); 
            sprintf((char*)lcd_buf,"msg data send=ok"); 
            //先清该区域
            LCD_Fill(30,50,30+200,50+16,WHITE);          
            LCD_ShowString(30,50,200,16,16, lcd_buf);                        
            break;
          
        }
        
        //业务数据接收消息
        case NBIOT_MSG_NCDP_RECV:
        {
            NBIOT_APP_DEBUG_INFO("msg data recv=%s\r\n",msg);
            sprintf((char*)lcd_buf,"msg data recv=ok"); 
            //先清该区域
            LCD_Fill(30,50,30+200,50+16,WHITE);          
            LCD_ShowString(30,50,200,16,16, lcd_buf);             
                        
            break;
        }
        
        //设备在iot平台状态消息      
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

                    //先清该区域
                    LCD_Fill(30,50,30+200,50+16,WHITE);            
                    LCD_ShowString(30,50,200,16,16,(uint8_t *)"msg dev reg=ok"); 
                
                    //发送数据
                    nbiot_app_status = NBIOT_APP_NCDP_SEND;
                
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
        
        //设备解除在iot平台注册消息             
        case NBIOT_MSG_NCDP_CLOSE:
        {
            NBIOT_APP_DEBUG_INFO("msg cdp close=%s\r\n",msg);
            sprintf((char*)lcd_buf,"msg cdp close=%s", msg);                       
            LCD_ShowString(30,50,200,16,16, lcd_buf);             
          
        }
        break; 
        
        default :
        {
            break;
        }
    }             
}


/**
  * @brief  app流程
  */
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
            NBIOT_APP_DEBUG_INFO("nbiot init start\r\n");
                    
            nbiot_reg_init(nbiot_handle);        

            *nbiot_app_status = NBIOT_END;
            break;
        }
                    
        case NBIOT_APP_RESET:
        {
            NBIOT_APP_DEBUG_INFO("nbiot reboot start\r\n");
                    
            nbiot_reboot(nbiot_handle);        

            *nbiot_app_status = NBIOT_END;
            break;
        }
          
        case NBIOT_APP_NCONFIG:
        {
            NBIOT_APP_DEBUG_INFO("nbiot auto reg start\r\n");
                    
            nbiot_nconfig(nbiot_handle, 0);        

            *nbiot_app_status = NBIOT_END;
            break;
        }
                     
        case NBIOT_APP_INFO:
        {
             NBIOT_APP_DEBUG_INFO("nbiot get info start\r\n");
                    
             nbiot_info_get(nbiot_handle);

             *nbiot_app_status = NBIOT_END;
            
             break;
        }
                                   
        case NBIOT_APP_SIGNAL:
        {
            NBIOT_APP_DEBUG_INFO("nbiot rssi(db) start\r\n");
            
            nbiot_signal_get(nbiot_handle);
            
            *nbiot_app_status = NBIOT_END; 
            break;
        }
                         
        case NBIOT_APP_NCDP_SERVER:
        {
            NBIOT_APP_DEBUG_INFO("cdp server set start\r\n");
            
            nbiot_ncdp_update(nbiot_handle, NBIOT_NCDP); 

            *nbiot_app_status = NBIOT_END; 
              
            break;
        }
        
        //发送业务数据         
        case NBIOT_APP_NCDP_SEND:
        {
            char ds18b20_src_buf[10] = {0};
            char ds18b20_dest_buf[20] = {0};  
                                                        
            NBIOT_APP_DEBUG_INFO("data send start\r\n");
                                                                     
            temperature = DS18B20_Get_Temp();           //读取温度值 
                               
            snprintf(ds18b20_src_buf,
                     sizeof(ds18b20_src_buf) - 1,
                     "%d",
                     temperature); 
                                
            
            //转换数据格式                     
            nbiot_srcbuf2hexchrbuf(ds18b20_src_buf, ds18b20_dest_buf, strlen(ds18b20_src_buf)); 
                                        
                     
            //发送当前温度到iot平台            
            nbiot_ncdp_send_hexstr(nbiot_handle, strlen(ds18b20_dest_buf), ds18b20_dest_buf, NBIOT_NCDP, NULL);                      
                      
            *nbiot_app_status = NBIOT_END;                    
                      
            
            break;        
        }
                    
        case NBIOT_APP_NCDP_RECV:
        {
            NBIOT_APP_DEBUG_INFO("data recv start\r\n");
            *nbiot_app_status = NBIOT_END;
            break;          
        }
          
        case NBIOT_APP_NCDP_CL:
        {
            NBIOT_APP_DEBUG_INFO("cdp close start\r\n");

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


/**
  * @brief  按键事件回调函数
  */
static void __key_event_handle(u32 key_event,void *p_arg)
{   
  
    switch(key_event)
    {
      case KEY0_PRES://KEY0按下,应用状态为NBIOT_APP_NCONFIG 
            
            //初始化网络注册
            nbiot_app_status = NBIOT_APP_INIT; 
            
            break;
        
        case KEY1_PRES://KEY1按下，应用状态为NBIOT_APP_INFO 
             
            //查询模块信息，得到imei号
            nbiot_app_status = NBIOT_APP_INFO; 
                            
            break;
        
        case KEY2_PRES://KEY2按下，应用状态为NBIOT_APP_NCDP_SERVER
                       
             //跳到更新cdp服务器,在iot平台注册设备
             nbiot_app_status = NBIOT_APP_NCDP_SERVER;  
                       
            break;
        
        case WKUP_PRES://WKUP按下，应用状态为NBIOT_APP_NCDP_SEND
                  
            //发送数据
            nbiot_app_status = NBIOT_APP_NCDP_SEND;
                       
            break; 

        default:
          
            break;          
        
    }      
}


/**
  * @brief  定时回调更新ds18b20采集到的温湿度的值.
  */
static void __ds18b20_timer_callback (void *p_arg)
{   
    ds18b20_flag = 1;
}




/**
  * @brief  The demo ds18b20 iot entry entry point.
  *
  * @retval None
  */
void demo_ds18b20_entry(void)
{
    int t = 0;
  
    char ds18b20_src_buf[10] = {0};
    char ds18b20_dest_buf[20] = {0};  
    
    struct atk_soft_timer ds18b20_timer;

    uart_handle_t uart_handle = NULL; 

    nbiot_handle_t  nbiot_handle = NULL; 

    key_handle_t  key_handle = NULL;

    key_handle = atk_key_exit_init(); 
    
    atk_key_registercb(key_handle, __key_event_handle, NULL);  
   
    uart_handle = atk_nbiot_uart_init(9600);  
    
    nbiot_handle = nbiot_dev_init(uart_handle);
     
    nbiot_event_registercb(nbiot_handle, __nbiot_msg_cb_handler, nbiot_handle); 
    


    LCD_ShowString(30,170,200,16,16,"NBIOT ds18b20");  

    while(DS18B20_Init())    //ds18b20初始化    
    {
        LCD_ShowString(30,190,200,16,16,"ds18b20 Error");
        delay_ms(200);
        LCD_Fill(30,190,239,130+16,WHITE);
        delay_ms(200);
    }

    //先清该区域
    LCD_Fill(30,190,30+200,190+16,WHITE);
    LCD_ShowString(30,190,200,16,16,"ds18b20 OK");
    LCD_ShowString(30,210,200,16,16,"Temp:   . C");
        
    temperature = DS18B20_Get_Temp();           //读取温度值 

    LCD_ShowNum(30+40,210,temperature,2,16);    //显示温度                     
    
    //软件定时器1000ms周期定时
    atk_soft_timer_init(&ds18b20_timer, __ds18b20_timer_callback, NULL, 1000, 1000); 
    atk_soft_timer_start(&ds18b20_timer);                                         
    
    while(1)
    {                       
        nbiot_app_status_poll(nbiot_handle, &nbiot_app_status);      
        nbiot_event_poll(nbiot_handle);      
        uart_event_poll(uart_handle);         
        atk_key_event_poll(key_handle); 
      
        //定时采集温度
        if (ds18b20_flag == 1)
        {            

                                                        //否则读取ds18b20可能会出问题            
            temperature = DS18B20_Get_Temp();           //读取温度值度值 
                               
            t++;
          
            if (t == 1800)
            {
                snprintf(ds18b20_src_buf,
                         sizeof(ds18b20_src_buf) - 1,
                         "%d",
                         temperature); 
                                    
                
                //转换数据格式                     
                nbiot_srcbuf2hexchrbuf(ds18b20_src_buf, ds18b20_dest_buf, strlen(ds18b20_src_buf)); 
                                            
                         
                //发送当前温度到iot平台            
                nbiot_ncdp_send_hexstr(nbiot_handle, strlen(ds18b20_dest_buf), ds18b20_dest_buf, NBIOT_NCDP, NULL);  
                         
                t = 0;          
            }

            if(temperature<0)
            {
                LCD_ShowChar(30+40,210,'-',16,0);       //显示负号
                temperature=-temperature;               //转为正数
            }
            else
            {
                LCD_ShowChar(30+40,210,' ',16,0);             //去掉负号
                LCD_ShowNum(30+40+8,210,temperature/10,2,16); //显示正数部分	    
                LCD_ShowNum(30+40+32,210,temperature%10,1,16);//显示小数部分  
            }             
                     
               
            ds18b20_flag = 0;
            LED1=!LED1;            
        }
        
    }
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
