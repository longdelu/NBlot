/************************************************
 Copyright (c) ������������ӿƼ����޹�˾ 2014-2024
 All rights reserved 
 ALIENTEK ������STM32F429������ 
 NBIOT��ȡģ����Ϣ
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/

#include "atk_sys.h"
#include "atk_led.h"
#include "atk_key.h"
#include "lcd.h"
#include "atk_delay.h"
#include "atk_bc28_nbiot.h"
#include "atk_bc28.h"
#include "atk_bc28_nbiot.h"
#include "stm32f4xx_hal.h"


#define NBIOT_APP_DEBUG  
#ifdef NBIOT_APP_DEBUG
#define NBIOT_APP_DEBUG_INFO(...)    (int)printf(__VA_ARGS__)    
#else
#define NBIOT_APP_DEBUG_INFO(...)
#endif

#define NBIOT_APP_DEBUG  
#ifdef NBIOT_APP_DEBUG
#define NBIOT_APP_DEBUG_INFO(...)    (int)printf(__VA_ARGS__)    
#else
#define NBIOT_APP_DEBUG_INFO(...)
#endif

static int nbiot_app_status = NBIOT_APP_NCONFIG;

/**
  * @brief  nbiot��Ϣ�¼�������
  */
static void __nbiot_msg_cb_handler (void *p_arg, int msg_id, int len, char *msg)
{ 
    nbiot_handle_t nbiot_handle = (nbiot_handle_t)p_arg; 
    
    (void)nbiot_handle;
  
    uint8_t lcd_buf[128] = {0};
    
    switch(msg_id)
    {
        //�����������������������Ϣ
        case NBIOT_MSG_CMD_NEXT:
          
            sprintf((char*)lcd_buf,"msg %s err but next", msg);           
            //���������
            LCD_Fill(30,50,30+200,50+16,WHITE);
            LCD_ShowString(30,50,200,16,16, lcd_buf);         
        
            NBIOT_APP_DEBUG_INFO("msg %s err but next\r\n",msg);
        
            break;
        
        //�����������������Ϣ       
        case NBIOT_MSG_CMD_RETRY:
          
            sprintf((char*)lcd_buf,"msg %s err but try", msg);           
            //���������
            LCD_Fill(30,50,30+200,50+16,WHITE);            
            LCD_ShowString(30,50,200,16,16, lcd_buf);  
        
            NBIOT_APP_DEBUG_INFO("msg %s err but try\r\n",msg);
        
            break;        
        
        //��������ʧ����Ϣ
        case NBIOT_MSG_CMD_FAIL: 

            sprintf((char*)lcd_buf,"msg %s failed", msg);           
            //���������
            LCD_Fill(30,50,30+200,50+16,WHITE);            
            LCD_ShowString(30,50,200,16,16, lcd_buf);  
        
            NBIOT_APP_DEBUG_INFO("msg %s failed\r\n",msg);         
            break;                     
        
        //����nbiotģ���ʼ�������Ϣ        
        case NBIOT_MSG_INIT:
        {         
            sprintf((char*)lcd_buf,"msg init=%s", msg);           
            //���������
            LCD_Fill(30,50,30+200,50+16,WHITE);            
            LCD_ShowString(30,50,200,16,16, lcd_buf);
          
            NBIOT_APP_DEBUG_INFO("msg init=%s\r\n",msg);
                    
            break;                    
        }
        
        //nbiot��λ�����Ϣ         
        case NBIOT_MSG_RESET:        
        {          
            sprintf((char*)lcd_buf,"msg reboot=%s", msg); 
            //���������
            LCD_Fill(30,50,30+200,50+16,WHITE);          
            LCD_ShowString(30,50,200,16,16, lcd_buf);          
            NBIOT_APP_DEBUG_INFO("msg reboot=%s\r\n",msg);          
            break;  
        }
        //�Զ��������������Ϣ                      
        case NBIOT_MSG_NCONFIG:       
        {
            nbiot_app_status = NBIOT_APP_RESET;
            //���������
            LCD_Fill(30,50,30+200,50+16,WHITE);          
            sprintf((char*)lcd_buf,"msg nconfig=%s", msg);                       
            LCD_ShowString(30,50,200,16,16, lcd_buf);           
          
            NBIOT_APP_DEBUG_INFO("msg nconfig=%s\r\n",msg); 
            break;  
        }      
        //����imsi����Ϣ       
        case NBIOT_MSG_IMSI:
        {
            sprintf((char*)lcd_buf,"msg imsi=%s", msg);                       
            LCD_ShowString(30,110,200,16,16, lcd_buf);            
            NBIOT_APP_DEBUG_INFO("msg imsi=%s\r\n",msg);           
            break;
        }       
        //����imei����Ϣ
        case NBIOT_MSG_IMEI:
        {
            sprintf((char*)lcd_buf,"msg imei=%s", msg);                       
            LCD_ShowString(30,130,200,16,16, lcd_buf);            
            NBIOT_APP_DEBUG_INFO("msg imei=%s\r\n",msg); 
            break;
        }        
        //��������ע����Ϣ                            
        case NBIOT_MSG_REG:
        {
             NBIOT_APP_DEBUG_INFO("msg reg status=%d\r\n",*msg);  
            //���������
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
        //�����ѯCSQ��Ϣ
        case NBIOT_MSG_CSQ:
            sprintf((char*)lcd_buf,"msg rssi=%sdbm", msg); 
            //���������
            LCD_Fill(30,70,30+200,70+16,WHITE);          
            LCD_ShowString(30,70,200,16,16, lcd_buf);   
          
            NBIOT_APP_DEBUG_INFO("msg rssi=%sdbm\r\n",msg);       
            break;        
        
        //�����ѯ�ź�������Ϣ
        case NBIOT_MSG_SIGNAL:
        {  
            NBIOT_APP_DEBUG_INFO("msg signal get=%s\r\n",msg);
            sprintf((char*)lcd_buf,"msg signal get=%s", msg);
          
            //���������
            LCD_Fill(30,50,30+200,50+16,WHITE);          
            LCD_ShowString(30,50,200,16,16, lcd_buf);  
            break;
        }
        //�����ѯģ����Ϣ�ɹ���Ϣ                     
        case NBIOT_MSG_INFO:
          
        {
            NBIOT_APP_DEBUG_INFO("msg info get=%s\r\n",msg);
            sprintf((char*)lcd_buf,"msg info get=%s", msg);
            //���������
            LCD_Fill(30,50,30+200,50+16,WHITE);          
            LCD_ShowString(30,50,200,16,16, lcd_buf);  
       
          
            break;                   
        }
        //����ģ�����ӣ����״̬��Ϣ
        case NBIOT_MSG_CSCON_STATUS:
        {
            //���������
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
        
        //������Ϣ      
        case NBIOT_MSG_BAND:
            NBIOT_APP_DEBUG_INFO("msg freq=%s\r\n",msg);
            break;
        
        //����ID��Ϣ
        case NBIOT_MSG_MID:
        {
            NBIOT_APP_DEBUG_INFO("msg mid=%s\r\n",msg);
            break;
        }
        
        
        //ģ���ͺ���Ϣ
        case NBIOT_MSG_MMODEL:
        {
            NBIOT_APP_DEBUG_INFO("msg mmodel=%s\r\n",msg); 
            break; 
        }
               

        //����汾��
        case NBIOT_MSG_MREV:
        {
            NBIOT_APP_DEBUG_INFO("msg mrev=%s\r\n",msg);
            break;
        }
        
        //cdp������������Ϣ                                                             
        case NBIOT_MSG_NCDP_SERVER:
        {         
            sprintf((char*)lcd_buf,"msg cdp server update=%s", msg); 
            //���������
            LCD_Fill(30,50,30+200,50+16,WHITE);          
            LCD_ShowString(30,50,200,16,16, lcd_buf);            
            NBIOT_APP_DEBUG_INFO("msg cdp server update=%s\r\n",msg);
          
            break;
        }
        
        //ҵ�����ݷ�����Ϣ
        case NBIOT_MSG_NCDP_SEND:
        {
            NBIOT_APP_DEBUG_INFO("msg data send=%s\r\n",msg); 
            sprintf((char*)lcd_buf,"msg data send=ok"); 
            //���������
            LCD_Fill(30,50,30+200,50+16,WHITE);          
            LCD_ShowString(30,50,200,16,16, lcd_buf);                        
            break;
          
        }
        
        //ҵ�����ݽ�����Ϣ
        case NBIOT_MSG_NCDP_RECV:
        {
            NBIOT_APP_DEBUG_INFO("msg data recv=%s\r\n",msg);
            sprintf((char*)lcd_buf,"msg data recv=ok"); 
            //���������
            LCD_Fill(30,50,30+200,50+16,WHITE);          
            LCD_ShowString(30,50,200,16,16, lcd_buf);             
                        
            break;
        }
        
        //�豸��iotƽ̨״̬��Ϣ      
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

                    //���������
                    LCD_Fill(30,50,30+200,50+16,WHITE);            
                    LCD_ShowString(30,50,200,16,16,(uint8_t *)"msg dev reg=ok"); 
                
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
        
        //�豸�����iotƽ̨ע����Ϣ             
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
  * @brief  app����
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
                       
        case NBIOT_APP_NCDP_SEND:
        {
            //��������iotƽ̨            
            nbiot_ncdp_send_hexstr(nbiot_handle, strlen("313232"), "313232", NBIOT_NCDP, NULL);                      
                      
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
  * @brief  �����¼��ص�����
  */
static void __key_event_handle(u32 key_event,void *p_arg)
{   
  
    switch(key_event)
    {
      case KEY0_PRES://KEY0����,Ӧ��״̬ΪNBIOT_APP_NCONFIG 
            
            //��ʼ������ע��
            nbiot_app_status = NBIOT_APP_INIT; 
            
            break;
        
        case KEY1_PRES://KEY1���£�Ӧ��״̬ΪNBIOT_APP_INFO 
             
            //��ѯģ����Ϣ���õ�imei��
            nbiot_app_status = NBIOT_APP_INFO; 
                            
            break;
        
        case KEY2_PRES://KEY2���£�Ӧ��״̬ΪNBIOT_APP_NCDP_SERVER
                       
             //��������cdp������,��iotƽ̨ע���豸
             nbiot_app_status = NBIOT_APP_NCDP_SERVER;  
                       
            break;
        
        case WKUP_PRES://WKUP���£�Ӧ��״̬ΪNBIOT_APP_NCDP_SEND
                  
            //��������
            nbiot_app_status = NBIOT_APP_NCDP_SEND;
                       
            break; 

        default:
          
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
       
    //�����豸���  
    uart_handle_t   uart_handle = NULL; 

    //nbiot�豸���
    nbiot_handle_t  nbiot_handle = NULL; 

    //�����豸��� 
    key_handle_t    key_handle = NULL;

    //��ȡ�����豸��� 
    key_handle = atk_key_exit_init(); 
    
    //ע�ᰴ���豸�ص�����
    atk_key_registercb(key_handle, __key_event_handle, NULL); 
  
    //��ȡ�����豸���
    uart_handle = atk_nbiot_uart_init(9600);  
    
    //��ȡnbiot�豸���
    nbiot_handle = nbiot_dev_init(uart_handle);
    
    //ע��nb�豸�ص�����   
    nbiot_event_registercb(nbiot_handle, __nbiot_msg_cb_handler, nbiot_handle);
                 
    while (1)
    {   
        //Ӧ��״̬��ѯ
        nbiot_app_status_poll(nbiot_handle, &nbiot_app_status);
      
        //nbiot�¼���ѯ
        nbiot_event_poll(nbiot_handle);
      
        //�����¼���ѯ
        uart_event_poll(uart_handle);
        
        //�����¼���ѯ       
        atk_key_event_poll(key_handle);    
    }
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

