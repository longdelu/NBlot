#include "atk_bc28.h"
#include "atk_delay.h"
#include "atk_bc28_nbiot.h"

////////////////////////////////////////////////////////////////////////////////// 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F429������
//����1��ʼ��           
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2015/9/7
//�汾��V1.5
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//********************************************************************************


/**
  * @brief ���Ժ꿪��
  */
//#define NBIOT_DEBUG                   
#ifdef NBIOT_DEBUG
#define NBIOT_DEBUG_INFO(...)    (int)printf(__VA_ARGS__)
#else
#define NBIOT_DEBUG_INFO(...)
#endif


//��Ҫ�߼�
//���������Ӧ��Ľ��������at_cmd_result_parse�������
//�����ȷ�����������������������Ƿ���Ҫִ����һ������
//������Ϣʱ������nbiot_msg_send����״̬�������ʱ��û�з����ı�
//ֱ������at_cmd_next�������һ��ָ���ʱ��Ż�ı���״̬
//��at_cmd_next����FALSEʱ���������е���״̬���Ѿ������ˣ���ʱ�Ḵ��״̬�����߼�


/**
  * @brief ����nbiot�����շ������ṹ�����
  */
static struct nbiot_recv  g_nbiot_recv_desc;
static struct nbiot_send  g_nbiot_send_desc;

/**
  * @brief �������ڱ��浱ǰ����ִ�е�ATָ������ṹ����
  */
static at_cmd_info_t g_at_cmd; 

/**
  * @brief ����nbiot�豸�ṹ�����
  */
static struct nbiot_dev       g_nbiot_dev;

/**
  * @brief ����nbiotָ��״̬������Ϣ�ṹ�����
  */
static nbiot_status_sm_t      g_nbiot_sm_status;

/**
  * @brief ����״̬��Ϣ�ṹ�����
  */
static nbiot_status_connect_t  g_nbiot_connect_status;     

/**
  * @brief ����nbiot�̼���Ϣ�ṹ�����
  */
static nbiot_firmware_info_t   g_firmware_info; 


/**
  * @brief ����socket��Ϣ�ṹ�����
  */
static nbiot_socket_info_t    g_socket_info[5];


/**
  * @brief ��غ�����ǰ����
  */
static int  __nbiot_uart_data_tx (void *p_arg, uint8_t *pData, uint16_t size, uint32_t Timeout);
static int  __nbiot_uart_data_rx (void *p_arg, uint8_t *pData, uint16_t size, uint32_t Timeout);
static void __uart_event_cb_handle(void *p_arg);
static int8_t at_cmd_result_parse(char *buf);
static uint8_t nbiot_response_handle (nbiot_handle_t nbiot_handle, uint8_t cmd_response);
static uint8_t at_cmd_next (void);   
static uint8_t nbiot_event_notify (nbiot_handle_t nbiot_handle, char *buf);
static void nbiot_msg_send (nbiot_handle_t nbiot_handle, char**buf, int8_t is_ok);
static int nbiot_data_recv(nbiot_handle_t nbiot_handle, uint8_t *pData, uint16_t size, uint32_t Timeout);


/**
  * @brief ����NBIoTģ����������������� 
  */
static struct nbiot_drv_funcs drv_funcs = {    
    __nbiot_uart_data_tx,
    __nbiot_uart_data_rx,        
};

/**
  * @brief ��ս��ջ���  
  * @param  None 
  * @retval None  
  */
static void nbiot_recv_buf_reset(void)
{   
    memset(&g_nbiot_recv_desc, 0, sizeof(struct nbiot_recv));
}


/**
  * @brief ��λATָ��ִ�е�����  
  * @param  None 
  * @retval None  
  */
static void nbiot_status_reset(void)
{
    g_nbiot_sm_status.main_status  = NBIOT_NONE;
    g_nbiot_sm_status.sub_status   = NBIOT_SUB_NONE;
}

/**
  * @brief ����ATָ��ִ�е�����  
  * @param  None 
  * @retval None  
  */
static void nbiot_status_set (nbiot_main_status_t  main_status, nbiot_sub_status_t  sub_status)
{
    g_nbiot_sm_status.main_status  = main_status;
    g_nbiot_sm_status.sub_status   = sub_status;
}

/**
  * @brief ��ȡATָ��ִ�е�����  
  * @param  None 
  * @retval None  
  */
static void nbiot_status_get (nbiot_status_sm_t *p_sm_status)
{
    p_sm_status->main_status  = g_nbiot_sm_status.main_status ;
    p_sm_status->sub_status   = g_nbiot_sm_status.sub_status;
}

/**
  * @brief ����nbiot�¼�  
  * @param  nbiot_handle  : ָ��nbiot�豸�����ָ��.
  * @param  nbiot_event   : �¼�����.
  * @retval None  
  */
void nbiot_event_set (nbiot_handle_t nbiot_handle, int nbiot_event)
{ 
    nbiot_handle->nbiot_event |= nbiot_event;   
}

/**
  * @brief  �жϵ�ǰnbiot�¼��Ƿ���  
  * @param  nbiot_handle  : ָ��nbiot�豸�����ָ��.
  * @param  nbiot_event   : �¼�����.
  * @retval ��0����ǰ�¼��Ѿ�����  
  */
int nbiot_event_get (nbiot_handle_t nbiot_handle,  int nbiot_event)
{ 
    return (nbiot_handle->nbiot_event & nbiot_event); 
}

/**
  * @brief ���nbiot�¼�  
  * @param  nbiot_handle  : ָ��nbiot�豸�����ָ��.
  * @param  nbiot_event   : �¼�����.
  * @retval None  
  */
void nbiot_event_clr (nbiot_handle_t nbiot_handle, int nbiot_event)
{ 
    nbiot_handle->nbiot_event &= ~nbiot_event;
}


/**
  * @brief �����¼��ص�������
  * @param  p_arg  : ������ָ��.
  * @retval None  
  */
static void __uart_event_cb_handle (void *p_arg)
{    
    nbiot_handle_t  nbiot_handle = (nbiot_handle_t)p_arg; 
    
    uart_dev_t       *p_uart_dev     = nbiot_handle->p_uart_dev;
  
    int size = g_nbiot_recv_desc.len;
    
    if (p_uart_dev->uart_event & UART_TX_EVENT)
    {
               
        NBIOT_DEBUG_INFO("atk_nbiot uart tx ok %s", g_nbiot_send_desc.buf);  

        uart_event_clr(p_uart_dev, UART_TX_EVENT);                 
    }

    if (p_uart_dev->uart_event & UART_RX_EVENT)
    {               
        size = uart_ring_buf_avail_len(p_uart_dev);
        
        //�ӻ��������ж�ȡ����
        if (size > 0)
        {                                  
            nbiot_data_recv(nbiot_handle, (uint8_t*)(&g_nbiot_recv_desc.buf[g_nbiot_recv_desc.len]), size, 0);
                     
            //�����첽�¼��ȴ�����
            nbiot_event_notify(nbiot_handle, g_nbiot_recv_desc.buf);
                        
            NBIOT_DEBUG_INFO("atk_nbiot uart rx ok %s\r\n", g_nbiot_recv_desc.buf);
          
            //�ı�������ݵĻ�����λ�ã�ȷ��һ������������������ȫ�յ�
            g_nbiot_recv_desc.len = g_nbiot_recv_desc.len + size;
            
        }
                                                                                 
        uart_event_clr(p_uart_dev, UART_RX_EVENT); 
    } 

    //�������ͳ�ʱ�¼���˵��ָ���п���û�з��ͳ�ȥ�����߷��͹����г���
    //����ģ�鹤���쳣��û�л�Ӧ���������
    if (p_uart_dev->uart_event & UART_TX_TIMEOUT_EVENT) 
    {        
        NBIOT_DEBUG_INFO("atk_nbiot uart tx timeout %s", g_nbiot_send_desc.buf);  

        nbiot_event_set(nbiot_handle, NBIOT_TIMEOUT_EVENT);
      
        uart_event_clr(p_uart_dev, UART_TX_TIMEOUT_EVENT); 
    } 

    //���ʹ�÷ǳ�ʱ��֡�����¼������ϲ��ᷢ��
    if (p_uart_dev->uart_event & UART_RX_TIMEOUT_EVENT) 
    {       
        size = uart_ring_buf_avail_len(p_uart_dev);
        
        //��ʱ��֡      
        if (g_nbiot_dev.frame_format == 1) 
        {
            if (size > 0)
            {               
                nbiot_data_recv(nbiot_handle, (uint8_t*)(&g_nbiot_recv_desc.buf[g_nbiot_recv_desc.len]), size, 0);
                                             
                //�����첽�¼��ȴ�����
                nbiot_event_notify(nbiot_handle, g_nbiot_recv_desc.buf);
              
                g_nbiot_recv_desc.len = g_nbiot_recv_desc.len + size;
            }
                                        
       //���ڳ�ʱ��֡��״̬�£������ȷ�����˳�ʱ�¼�      
       } else {
         
            if (size > 0)
            {               
                nbiot_data_recv(nbiot_handle, (uint8_t*)(&g_nbiot_recv_desc.buf[g_nbiot_recv_desc.len]), size, 0);
                                             
                //�����첽�¼��ȴ�����
                nbiot_event_notify(nbiot_handle, g_nbiot_recv_desc.buf);
              
                //�ı�������ݵĻ�����λ�ã�ȷ��һ������������������ȫ�յ�
                g_nbiot_recv_desc.len = g_nbiot_recv_desc.len + size;
            }          
         
            //������ʱ�¼�����ʱ�¼��Ƿ�ִ�еÿ�����������
            nbiot_event_set(nbiot_handle, NBIOT_TIMEOUT_EVENT);
       }
       
       NBIOT_DEBUG_INFO("atk_nbiot uart rx timeout %s\r\n", g_nbiot_recv_desc.buf);  
       
       uart_event_clr(p_uart_dev, UART_RX_TIMEOUT_EVENT);
    }            
}

/**
  * @brief nbiot�¼�������
  * @param  nbiot_handle  : ָ��nbiot�豸�����ָ��.
  * @retval NBIOT_OK �ɹ�  
  */
int nbiot_event_poll(nbiot_handle_t nbiot_handle)
{
    char *at_response_par[AT_CMD_RESPONSE_PAR_NUM_MAX] = {0};
    
    char *p_revc_buf_tmp = g_nbiot_recv_desc.buf;
        
    uint8_t index = 0;
        
    int8_t next_cmd = 0;
    
    int8_t cmd_is_pass = 0;
    
    static int8_t recv_cnt = 0;
            
    if (nbiot_event_get(nbiot_handle, NBIOT_RECV_EVENT)) 
    {       
        //�����ڳ�ʱʱ����������ɣ�ֹͣ���ճ�ʱ  
        atk_soft_timer_stop(&nbiot_handle->p_uart_dev->uart_rx_timer); 
        //�����������ڴ��󼴿ɽ������������������볬ʱ�¼�
        nbiot_event_clr(nbiot_handle, NBIOT_TIMEOUT_EVENT); 
        
        NBIOT_DEBUG_INFO("%s recv event\r\n", g_at_cmd.p_atcmd);
      
        cmd_is_pass = at_cmd_result_parse(g_nbiot_recv_desc.buf);
        
        //������Ӧ���     
        if (cmd_is_pass == AT_CMD_RESULT_OK) 
        {
            NBIOT_DEBUG_INFO("RESULT OK\r\n");            
                             
            recv_cnt=0; 
                              
            //��ȡATָ��صĲ���,��ʹ��strok�ڼ䣬������ı仺���������ݣ��м�����ٶ��\r\n��Ҳֻ�ᵱ��һ��������
            while((at_response_par[index] = strtok(p_revc_buf_tmp,"\r\n")) != NULL)
            {
                index++;
                p_revc_buf_tmp = NULL;
                
                if (index >= (AT_CMD_RESPONSE_PAR_NUM_MAX - 1))
                {
                    break;
                }  
            }

            if (index != 0)
            {               
                NBIOT_DEBUG_INFO("%s cmd excute ok\r\n\r\n\r\n", g_at_cmd.p_atcmd);
              
                //��������ͳɹ��˲��õ���ȷ����Ӧ
                nbiot_msg_send(nbiot_handle, at_response_par, TRUE);
                next_cmd = nbiot_response_handle(nbiot_handle, TRUE);                
                                                                    
            } 
            else 
            {
                
                //δ�յ�����\r\n...\r\n��ȷ������֡�����ݽ�����ط������������
                next_cmd = nbiot_response_handle(nbiot_handle, FALSE); 
            }                
                                                                                      
            //�建��            
            nbiot_recv_buf_reset();                        
       
        }       
        else if(cmd_is_pass == AT_CMD_RESULT_ERROR)
        { 
            NBIOT_DEBUG_INFO("RESULT ERROR\r\n");           

            recv_cnt=0;
          
            //���������������Գ����ط������������������ִ����һ������            
            next_cmd = nbiot_response_handle(nbiot_handle, FALSE);     
        
            if (g_at_cmd.cmd_action & ACTION_ERROR_AND_TRY)
            {                                   
                NBIOT_DEBUG_INFO("%s cmd is failed and try\r\n", g_at_cmd.p_atcmd);
                
                at_response_par[AT_CMD_RESPONSE_PAR_NUM_MAX - 1] = (char*)g_at_cmd.p_atcmd;
                
                //֪ͨ�ϲ�Ӧ�ã��˶���ִ��ʧ��,��������
                nbiot_msg_send(nbiot_handle, &at_response_par[AT_CMD_RESPONSE_PAR_NUM_MAX - 1], NBIOT_ERROR_RETRY);
                
            } 
            else if (g_at_cmd.cmd_action & ACTION_ERROR_BUT_NEXT)
            {               
                at_response_par[AT_CMD_RESPONSE_PAR_NUM_MAX - 1] = (char*)g_at_cmd.p_atcmd;
               
                NBIOT_DEBUG_INFO("%s cmd is failed and next\r\n", g_at_cmd.p_atcmd);
                
                //֪ͨ�ϲ�Ӧ�ã��˶���ִ��ʧ�ܺ�����������ִ��
                nbiot_msg_send(nbiot_handle, &at_response_par[AT_CMD_RESPONSE_PAR_NUM_MAX - 1], NBIOT_ERROR_NEXT);
                
            }
            else 
            {                
                NBIOT_DEBUG_INFO("%s cmd is failed and exit\r\n", g_at_cmd.p_atcmd);        
                at_response_par[AT_CMD_RESPONSE_PAR_NUM_MAX - 1] = (char*)g_at_cmd.p_atcmd;
                
                //֪ͨ�ϲ�Ӧ�ã��˶���ִ��ʧ�ܺ�����������ִ��
                nbiot_msg_send(nbiot_handle, &at_response_par[AT_CMD_RESPONSE_PAR_NUM_MAX - 1], FALSE);  

                //��λ״̬��־
                nbiot_status_reset();              

            } 
                    
            //�建��            
            nbiot_recv_buf_reset();            

        } 

        else if (cmd_is_pass == AT_CMD_RESULT_CONTINUE)
        {
          
            NBIOT_DEBUG_INFO("RESULT CONTINUE\r\n");
            recv_cnt=0;
            
            //����δִ����ɣ���������£��յ��ĵ����������, �������Ļ��ǵ�ǰ������Ӧ���ݵĽ���, �����������ճ�ʱ               
            atk_soft_timer_timeout_change(&nbiot_handle->p_uart_dev->uart_rx_timer, 8000);
                             
             
            //����δ���            
        }       
        else 
        {
         
            recv_cnt++;

            NBIOT_DEBUG_INFO("recv_cn =%d\r\n", recv_cnt);            
          
            //�����ϲ������е�����, ���е���������������
            //һ���Ǳ�ʾ��������յ������ݶ�������        
            //�ڶ�����IDLE��֡�жϲ�����ô׼ȷ��û������Ҳ��Ϊһ֡������
            if (recv_cnt > (AT_CMD_RESPONSE_PAR_NUM_MAX + 20))                         
            {  
               //�����ڳ�ʱʱ����δ������ɣ�ֹͣ���ճ�ʱ  
               atk_soft_timer_stop(&nbiot_handle->p_uart_dev->uart_rx_timer);   
              
               //�յ���������,ǿ�ƽ��ս�����ͬʱ��������������ж������Ƿ��ط�������������ִ����һ������
               next_cmd = nbiot_response_handle(nbiot_handle, FALSE);
              
               recv_cnt = 0;
               //�建��            
               nbiot_recv_buf_reset();                
            }
            else 
            {
              
               //����δ���,�յ��ĵ���������Ե������е�һ����                
               atk_soft_timer_timeout_change(&nbiot_handle->p_uart_dev->uart_rx_timer, 10000);
             
            }              
                               
        } 
            
        nbiot_event_clr(nbiot_handle, NBIOT_RECV_EVENT); 
    }
       
    if (nbiot_event_get(nbiot_handle, NBIOT_REG_STA_EVENT)) 
    {      
        NBIOT_DEBUG_INFO("reg event\r\n");

                               
        //֪ͨ�ϲ�Ӧ������ע����
        nbiot_msg_send(nbiot_handle, NULL, TRUE); 

        next_cmd = AT_CMD_NEXT;
        nbiot_event_clr(nbiot_handle,   NBIOT_REG_STA_EVENT); 
      
        //�����������    
        nbiot_recv_buf_reset();   
    }
    
        
    if (nbiot_event_get(nbiot_handle, NBIOT_COAP_RECV_EVENT)) 
    {
        
        NBIOT_DEBUG_INFO("coap recv event ok\r\n");
      
        //֪ͨ�ϲ�Ӧ�ý��յ�CoAP����
        nbiot_msg_send(nbiot_handle, NULL, TRUE);          
             
        nbiot_event_clr(nbiot_handle, NBIOT_COAP_RECV_EVENT);

         //��λ״̬��־
         nbiot_status_reset();         
      
        //�����������    
        nbiot_recv_buf_reset();  
    }
    
    if (nbiot_event_get(nbiot_handle, NBIOT_NCDP_RECV_EVENT)) 
    {
        
        NBIOT_DEBUG_INFO("ncdp recv event ok\r\n");
      
        //֪ͨ�ϲ�Ӧ�ý��յ�NCDP����
        nbiot_msg_send(nbiot_handle, NULL, TRUE);          
             
        nbiot_event_clr(nbiot_handle, NBIOT_NCDP_RECV_EVENT); 
      
        //��λ״̬��־
        nbiot_status_reset();   
      
        //�����������    
        nbiot_recv_buf_reset();  
    }

    if (nbiot_event_get(nbiot_handle, NBIOT_NCDP_STATUS_EVENT)) 
    {
        
        NBIOT_DEBUG_INFO("ncdp status event ok\r\n");
      
        //֪ͨ�ϲ�Ӧ��NCDP״̬���ӷ����仯
        nbiot_msg_send(nbiot_handle, NULL, TRUE);          
             
        nbiot_event_clr(nbiot_handle, NBIOT_NCDP_STATUS_EVENT); 
      
        //��λ״̬��־
        nbiot_status_reset();   
      
        //�����������    
        nbiot_recv_buf_reset();  
    }


    if (nbiot_event_get(nbiot_handle, NBIOT_CSON_STATUS_EVENT)) 
    {
        
        NBIOT_DEBUG_INFO("cson status event ok\r\n");
      
        //֪ͨ�ϲ�Ӧ��NCDP״̬���ӷ����仯
        nbiot_msg_send(nbiot_handle, NULL, TRUE);          
             
        nbiot_event_clr(nbiot_handle, NBIOT_CSON_STATUS_EVENT); 
      
             
        //�����������    
        nbiot_recv_buf_reset();  
    }     
    
    
    if (nbiot_event_get(nbiot_handle, NBIOT_SOCKET_ERR_EVENT)) 
    {       
        NBIOT_DEBUG_INFO("socket err event\r\n");
      
        //֪ͨ�ϲ�Ӧ��SOCKETʧ����
        nbiot_msg_send(nbiot_handle, NULL, TRUE);        
              
        nbiot_event_clr(nbiot_handle, NBIOT_SOCKET_ERR_EVENT); 
      
        //��λ״̬��־
        nbiot_status_reset();   
      
        //�����������    
        nbiot_recv_buf_reset();  
    }
    
    if (nbiot_event_get(nbiot_handle, NBIOT_OTHER_EVENT)) 
    {       
        NBIOT_DEBUG_INFO("other event\r\n");
         
        //�����������    
        nbiot_recv_buf_reset();        
        nbiot_event_clr(nbiot_handle, NBIOT_OTHER_EVENT); 
      
    }    

    if (nbiot_event_get(nbiot_handle, NBIOT_TIMEOUT_EVENT)) 
    {        
        //��ʱ����������������Գ����ط������������������ִ����һ������        
        next_cmd = nbiot_response_handle(nbiot_handle, FALSE);
      
        //֪ͨ�ϲ�Ӧ�ã��˶���ִ�г�ʱ
        if (g_at_cmd.cmd_action & ACTION_ERROR_BUT_NEXT) 
        {           
            at_response_par[AT_CMD_RESPONSE_PAR_NUM_MAX - 1] = (char *)g_at_cmd.p_atcmd;
           
            NBIOT_DEBUG_INFO("%s cmd not repsonse or send failed\r\n", g_at_cmd.p_atcmd);
                               
            //֪ͨ�ϲ�Ӧ�ã��˶�����ʱִ��ʧ�ܺ�����������ִ��
            nbiot_msg_send(nbiot_handle, &at_response_par[AT_CMD_RESPONSE_PAR_NUM_MAX - 1], NBIOT_ERROR_NEXT);            
        }        
        else if (g_at_cmd.cmd_action & ACTION_ERROR_AND_TRY) 
        {           
            at_response_par[AT_CMD_RESPONSE_PAR_NUM_MAX - 1] = (char *)g_at_cmd.p_atcmd;
           
            NBIOT_DEBUG_INFO("%s cmd not repsonse or send failed\r\n", g_at_cmd.p_atcmd);
                               
            //֪ͨ�ϲ�Ӧ�ã��˶�����ʱִ��ʧ�ܺ�����������ִ��
            nbiot_msg_send(nbiot_handle, &at_response_par[AT_CMD_RESPONSE_PAR_NUM_MAX - 1], NBIOT_ERROR_RETRY);            
        }
        else        
        {            
            NBIOT_DEBUG_INFO("%s cmd is failed and exit\r\n", g_at_cmd.p_atcmd);        
            at_response_par[AT_CMD_RESPONSE_PAR_NUM_MAX - 1] = (char*)g_at_cmd.p_atcmd;
            
            //֪ͨ�ϲ�Ӧ�ã��˶�����ʱִ��ʧ�ܺ��������ִ��
            nbiot_msg_send(nbiot_handle, &at_response_par[AT_CMD_RESPONSE_PAR_NUM_MAX - 1], FALSE);  

            //��λ״̬��־
            nbiot_status_reset();              
        } 
        
        //�����������      
        nbiot_recv_buf_reset(); 
        recv_cnt = 0;        
                                                    
        nbiot_event_clr(nbiot_handle, NBIOT_TIMEOUT_EVENT); 
    } 
    
    //�����¼���״̬�ж��Ƿ���Ҫִ����һ������
    if(next_cmd == AT_CMD_NEXT)
    {
        //ִ����һ������
        if (at_cmd_next())
        {
            nbiot_at_cmd_send(nbiot_handle, &g_at_cmd);
        }
        
        //����FALSE��ʾ�ӽ����Ѿ�������
        else
        {
            //�������״̬�����е���״̬�����Ѿ��ɹ�ִ�������
            nbiot_msg_send(nbiot_handle, NULL,TRUE);

            //��λsm״̬��־
            nbiot_status_reset();
        }     
    }
   
    return NBIOT_OK;    
}

/**
  * @brief  nbiot���ݷ���  
  */
static int  __nbiot_uart_data_tx (void *p_arg, uint8_t *pData, uint16_t size, uint32_t Timeout)
{  
    int ret = 0;
    
    nbiot_handle_t  nbiot_handle = (nbiot_handle_t)p_arg;
    
    uart_handle_t uart_handle = nbiot_handle->p_uart_dev; 
    
    ret = uart_data_tx_poll(uart_handle, pData, size, Timeout); 

    return ret;    
}

/**
  * @brief  nbiot���ݽ���   
  */
static int  __nbiot_uart_data_rx (void *p_arg, uint8_t *pData, uint16_t size, uint32_t Timeout)
{  
    int ret = 0;
    
    //��ʱ��������ʹ��
    (void)Timeout;
    
    nbiot_handle_t  nbiot_handle = (nbiot_handle_t)p_arg;
    
    uart_handle_t uart_handle = nbiot_handle->p_uart_dev;

    uart_ring_buf_read(uart_handle, pData, size);    
    
    return ret;    
}


/**
  * @brief nbiot atָ���ʼ��
  * @param  at_cmd           : ����ATָ��
  * @param  argument         : ָ���Ӧ�Ĳ���
  * @param  property         : ָ�����
  * @param  at_cmd_time_out  : ָ��ĳ�ʱʱ��
  * @retval None
  */
void nbiot_at_cmd_param_init (at_cmdhandle cmd_handle,
                              const char *at_cmd,
                              char *argument,
                              cmd_property_t property,
                              uint32_t at_cmd_time_out)
{
    if (cmd_handle == NULL)
    {
        return;
    }
    cmd_handle->cmd_try     = CMD_TRY_TIMES;
    cmd_handle->property    = property;
    cmd_handle->cmd_action  = ACTION_OK_AND_NEXT | ACTION_ERROR_AND_TRY;
    cmd_handle->p_atcmd_arg = argument;
    cmd_handle->p_expectres = NULL;
    cmd_handle->have_tried  = 0;
    cmd_handle->max_timeout = at_cmd_time_out;
    cmd_handle->p_atcmd     = at_cmd;
}

/**
  * @brief ����nbiot atָ����ַ��������Ӧ�ĳ���
  * @param  cmd_handle  : ATָ��ľ��
  * @retval ���ɺ�AT�ܹ�����Ч����  
  */
static int cmd_generate(at_cmdhandle cmd_handle)
{
    int cmdLen = 0;
    
    if (cmd_handle == NULL)
    {
        return cmdLen;
    }
    memset(g_nbiot_send_desc.buf,0,NBIOT_SEND_BUF_MAX_LEN);
    g_nbiot_send_desc.len = 0;

    if(cmd_handle->property == CMD_TEST)
    {
        cmdLen = snprintf(g_nbiot_send_desc.buf,NBIOT_SEND_BUF_MAX_LEN,
                          "%s=?\r\n",
                          cmd_handle->p_atcmd);
    }    
    else if(cmd_handle->property == CMD_READ)
    {
        cmdLen = snprintf(g_nbiot_send_desc.buf,NBIOT_SEND_BUF_MAX_LEN,
                          "%s?\r\n",
                          cmd_handle->p_atcmd);
    }
    else if(cmd_handle->property == CMD_EXCUTE)
    {
        cmdLen = snprintf(g_nbiot_send_desc.buf,NBIOT_SEND_BUF_MAX_LEN,
                          "%s\r\n",
                          cmd_handle->p_atcmd);    
    }

    else if(cmd_handle->property == CMD_SET)
    {
        cmdLen = snprintf(g_nbiot_send_desc.buf,NBIOT_SEND_BUF_MAX_LEN,
                          "%s=%s\r\n",
                          cmd_handle->p_atcmd,cmd_handle->p_atcmd_arg);    
    }
    
    //cmdlen����Ч�����ݳ��ȣ��������ַ���������Ƿ�
    g_nbiot_send_desc.len = cmdLen;
    
    return cmdLen;
}

/**
  * @brief ͨ��ATָ�����Ӧ���ݣ��ж�atָ���Ƿ��ͳɹ�
  * @param  buf  :  ָ��Ļ������ݵĻ���
  * @retval ����ֵΪ ACTION_OK_EXIT ��һ��ö������
  */
static int8_t at_cmd_result_parse (char *buf)
{
    int8_t result = -1;
  
  
    //ȷ���յ��������Ƿ�Ϊ������Ӧ�Ĳ���(+) 
    char *p_colon = strchr(buf,'+');
    
    //ȷ���յ��������Ƿ�Ϊ������Ӧ�Ĳ���(:)
    char *p_colon_temp = strchr(buf,':');
       
    if(g_at_cmd.p_expectres == NULL)
    {
        if (strstr(buf,"OK"))
        {
            result = AT_CMD_RESULT_OK;
        }
        else if (strstr(buf,"ERROR"))
        {
            result = AT_CMD_RESULT_ERROR;
          
        } else if ((strstr(buf,g_at_cmd.p_atcmd)) || ((p_colon && p_colon_temp))) {
          
            NBIOT_DEBUG_INFO("p_colon %s  p_colon_temp %s \r\n", p_colon, p_colon_temp);
            //ǰ�߳���������Ļ��� || ���߳���OK֮ǰ���������
            result = AT_CMD_RESULT_CONTINUE;
          
        } else {
          
           //����
           result = AT_CMD_RESULT_RANDOM_CODE;
        }
    }
    else
    {
        if (strstr(buf,"OK"))
        {
            //��õ�������ֵһ��
            if(strstr(buf,g_at_cmd.p_expectres))
            {
                result = AT_CMD_RESULT_OK;
            }
            else
            {
                result = AT_CMD_RESULT_ERROR;
            }
            
        }
        else if(strstr(buf,"ERROR"))
        {
            //ERROR
            result = AT_CMD_RESULT_ERROR;
        }
         
        else if (strstr(buf, g_at_cmd.p_atcmd) || ((p_colon && p_colon_temp))) {
          
             //ǰ�߳���������Ļ��� || ���߳���OK֮ǰ���������
             result = AT_CMD_RESULT_CONTINUE;
          
        } else {
          
             //����
             result = AT_CMD_RESULT_RANDOM_CODE;      
          
        }    
    }

    return result;
}



/**
  * @brief ���ݽ��յ�������nbiot����������Ӧ��һ���¼�֪ͨ
  * @param  nbiot_handle  : ָ��nbiot�豸�����ָ��.
  * @param  buf           : �������ݵĻ���
  * @retval NBIOT_OK �ɹ�  
  */
static uint8_t nbiot_event_notify (nbiot_handle_t nbiot_handle, char *buf)
{
    char *target_pos_start = NULL;
         
    if((target_pos_start = strstr(buf, "+CEREG:")) != NULL)
    {
        char *p_colon = strchr(target_pos_start,',');
      
        if (p_colon)
        {
            p_colon = p_colon + 1;
            
            //���������ֱ�Ӹ�������ע��״̬����Ϣ�����ַ�����ʾ�ģ�Ҫת��������        
            g_nbiot_connect_status.register_status = (*p_colon - '0');
            p_colon  = strstr(buf, "OK");
        }        
        else 
        {
           p_colon = strchr(target_pos_start,':');
          
            if (p_colon)
            {
                p_colon = p_colon + 1;
                
                //���������ֱ�Ӹ�������ע��״̬����Ϣ�����ַ�����ʾ�ģ�Ҫת��������        
                g_nbiot_connect_status.register_status = (*p_colon - '0');
            }
        }
                       
        if (p_colon !=NULL)
        {                      
            nbiot_event_set(nbiot_handle, NBIOT_REG_STA_EVENT);  
        } 
        else
        {
             //����յ��ظ���������AT������Ӧ������
             nbiot_event_set(nbiot_handle, NBIOT_RECV_EVENT);           
        }          
    }    
    else if ((target_pos_start = strstr(buf,"+QLWEVTIND:")) != NULL)
    {
        //�յ��������˷���NCDP״̬����
        char *p_colon = strchr(target_pos_start, ':');
              
        //�õ�NCDP��ǰ��״̬
        if (p_colon)
        {
            p_colon++;
            g_nbiot_connect_status.m2m_status = strtoul(p_colon,0,10);
        } 
                              
        nbiot_event_set(nbiot_handle, NBIOT_NCDP_STATUS_EVENT);  
        nbiot_status_set(NBIOT_NCDP_STATUS, NBIOT_SUB_NCDP_STATUS);          
    }
    
    else if ((target_pos_start = strstr(buf,"+CSCON:")) != NULL)
    {
        //�յ��������˷���NCDP״̬����
        char *p_colon = strchr(target_pos_start, ':');
        nbiot_status_sm_t sm_status = {0,0};
              
        //�õ�NCDP��ǰ��״̬
        if (p_colon)
        {
            p_colon++;
            g_nbiot_connect_status.cscon_status = strtoul(p_colon,0,10);
        } 
                              
        nbiot_event_set(nbiot_handle, NBIOT_CSON_STATUS_EVENT);
        nbiot_status_get(&sm_status);

        if ((sm_status.main_status != NBIOT_INIT) && (sm_status.main_status != NBIOT_INFO)) 
        {          
             nbiot_status_set(NBIOT_CSCON_STATUS, NBIOT_SUB_CSCON_STATUS);       
        }          
    } 

    else if ((target_pos_start = strstr(buf,"REGISTERNOTIFY")) != NULL)
    {
                              
        nbiot_event_set(nbiot_handle, NBIOT_OTHER_EVENT);      
    }     

    else if ((target_pos_start = strstr(buf,"+NNMI:")) != NULL)
    {
        //�յ��������˷���NCDP����
        char *p_colon = strchr(target_pos_start, ':');
             
        //�õ���Ч���ݵ���ʼ��ַ
        if (p_colon)
        {
            p_colon =  p_colon + 8;
            g_nbiot_connect_status.data_offest = p_colon;
                     
        } 
                              
        nbiot_event_set(nbiot_handle, NBIOT_NCDP_RECV_EVENT);  
        nbiot_status_set(NBIOT_NCDP_RECV, NBIOT_SUB_NCDP_RECV);           
    }    
    else if((target_pos_start = strstr(buf,"+CLMOBSERVE")) != NULL)
    {
        nbiot_event_set(nbiot_handle, NBIOT_LWM2M_RECV_EVENT);          
    } 

    //�յ�MQTT���ݰ�    
    else if ((target_pos_start = strstr(buf,"+CMQPUB")) != NULL)
    {        
        nbiot_event_set(nbiot_handle, NBIOT_MQTT_RECV_EVENT);  
    }    
    else 
    {
        //����յ��ظ���������AT������Ӧ������
        nbiot_event_set(nbiot_handle, NBIOT_RECV_EVENT);  
    }
    
    return 0;
}

/**
  * @brief �ж��Ƿ������һ��ATָ��
  * @param  type          : �������ͣ�ֵ��Ϊö�ٵ��е�ֵ
  * @retval FALSE ָ�����̽���  NBIOT_ERROR ����ʧ��  
  */
static uint8_t at_cmd_next (void)
{ 
    if (g_nbiot_sm_status.main_status == NBIOT_INIT)
    {
        g_nbiot_sm_status.sub_status++;
      
        if (g_nbiot_sm_status.sub_status == NBIOT_SUB_END)
        {
            return FALSE;
        }

        switch(g_nbiot_sm_status.sub_status)
        {
          
        case NBIOT_SUB_SYNC:
            
            break;
        
         case NBIOT_SUB_QLED:
          
            nbiot_at_cmd_param_init(&g_at_cmd, AT_QLEDMODE, "1", CMD_SET, 500);
          
            break;       
        
        case NBIOT_SUB_CMEE:
          
            nbiot_at_cmd_param_init(&g_at_cmd, AT_CMEE, "1", CMD_SET, 500);
            break;        
        
        case NBIOT_SUB_BAND:
          
            nbiot_at_cmd_param_init(&g_at_cmd, AT_NBAND, "5", CMD_SET, 500);
            g_at_cmd.cmd_action  = ACTION_OK_AND_NEXT | ACTION_ERROR_BUT_NEXT;
          
            break;
        
        case NBIOT_SUB_QREGSWT:
          
            nbiot_at_cmd_param_init(&g_at_cmd, AT_QREGSWT, "0", CMD_SET, 500);
          
            break;        
        
        case NBIOT_SUB_ATI:
          
            nbiot_at_cmd_param_init(&g_at_cmd, AT_ATI, NULL, CMD_EXCUTE, 3000);
            g_at_cmd.p_expectres = "Quectel";     //���������ظ���Ϣ�����ָ��ִ�����
                                                  //û������������Ϣƥ�䣬����Ϊ����
                                                  //�����г�����           
            break;
                      
        //��ѯNB��״̬�Ƿ�׼����    
        case NBIOT_SUB_CPIN:
          {
            nbiot_at_cmd_param_init(&g_at_cmd, AT_NCCID, NULL, CMD_READ, 3000);
            g_at_cmd.p_expectres = "+NCCID:"; //���������ظ���Ϣ�����ָ��ִ�����
                                              //û������������Ϣƥ�䣬����Ϊ����
                                              //�����г�����              
          }
          break;
                    
          
        //��ѯ��Ƶģ���ź�����   
        case NBIOT_SUB_CSQ:
          {
             nbiot_at_cmd_param_init(&g_at_cmd, AT_CSQ, NULL, CMD_EXCUTE, 3000);
          }
          break;

        //ʹ��ģ����Ƶ�ź�,��Ӧ�ȴ����ʱ��Ϊ6S      
        case NBIOT_SUB_CFUN:                                   
          {
            nbiot_at_cmd_param_init(&g_at_cmd, AT_CFUN,"1",CMD_SET, 6000);
          }
          
          break;
          
        // ʹ��nbiot����ע��   
        case NBIOT_SUB_CEREG:
          {
              nbiot_at_cmd_param_init(&g_at_cmd, AT_CEREG, "1", CMD_SET, 500);
              if (g_nbiot_connect_status.register_status != 1)
              {
                  g_at_cmd.cmd_action  = ACTION_OK_WAIT | ACTION_ERROR_AND_TRY; 
              }                           
          }
          break; 

        // ʹ��CSCONT��ʾ 
        case NBIOT_SUB_CSCON:
          {
            nbiot_at_cmd_param_init(&g_at_cmd, AT_CSCON, "1", CMD_SET, 500); 
              
          }
          break;            
          
                 
        //��ѯPDP������Ϣ          
        case NBIOT_SUB_CIPCA_QUERY:
          {
            nbiot_at_cmd_param_init(&g_at_cmd, AT_CIPCA, NULL, CMD_READ, 500);

          }
          break;          
               
          
        //ʹ�����總��,�����Ӧʱ��Ϊ1s,������������Ϊ3s     
        case NBIOT_SUB_CGATT:
          {
            nbiot_at_cmd_param_init(&g_at_cmd, AT_CGATT, "1", CMD_SET, 3000);            
          }
          break;
          
          
        //��ѯģ�������״̬��Ϣ     
        case NBIOT_SUB_NUESTATS:
          {
            nbiot_at_cmd_param_init(&g_at_cmd, AT_NUESTATS, NULL, CMD_EXCUTE, 500);
          }
          break;
          
        //��ѯ�����APN��Ϣ��IP��ַ     
        case NBIOT_SUB_CGPADDR:
          {
            nbiot_at_cmd_param_init(&g_at_cmd, AT_CGPADDR, NULL, CMD_EXCUTE, 500);
               
          }
          break; 

        case NBIOT_SUB_NNMI:
          {
            nbiot_at_cmd_param_init(&g_at_cmd, AT_NNMI, "1", CMD_SET, 500);
               
          }            
          break;  


//        case NBIOT_SUB_NSMI:
//          {
//            nbiot_at_cmd_param_init(&g_at_cmd, AT_NSMI, "1", CMD_SET, 500);
//               
//          }            
//          break;          
          
          
        //��ѯ���總����Ϣ,�����Ӧʱ�䲻��       
        case NBIOT_SUB_CGATT_QUERY:
          {
            nbiot_at_cmd_param_init(&g_at_cmd, AT_CGATT, NULL, CMD_READ, 3000);
            
            //���������ظ���Ϣ�����ָ��ִ�����
            //û������������Ϣƥ�䣬����Ϊ����                                             
            //�����г�����               
            g_at_cmd.p_expectres = "CGATT:1";     
          }
          break;
          
                                      
        //��ѯnbiot�����Ƿ�ע��,�����Ӧʱ�䲻��       
        case NBIOT_SUB_CEREG_QUERY:
          {
            nbiot_at_cmd_param_init(&g_at_cmd, AT_CEREG, NULL, CMD_READ, 500);             

          }
          break;
                    
        default: 
          
          //ǿ�Ʊ�ʾ�ӽ��̽���
          g_nbiot_sm_status.sub_status = NBIOT_SUB_END;
        
          return FALSE;
                   
         }
    }
    
    else if (g_nbiot_sm_status.main_status == NBIOT_INFO)
    {
        g_nbiot_sm_status.sub_status++;
      
        if (g_nbiot_sm_status.sub_status == NBIOT_SUB_END)
        {
            return FALSE;
        }
        
        switch(g_nbiot_sm_status.sub_status)
        {
          
        case  NBIOT_SUB_CGMI:
          
          {
            nbiot_at_cmd_param_init(&g_at_cmd,AT_CGMI,NULL,CMD_EXCUTE,3000);
          }
          break;

                   
        case NBIOT_SUB_CGMM:
          {
            nbiot_at_cmd_param_init(&g_at_cmd,AT_CGMM,NULL,CMD_EXCUTE,3000);
          }
          break;
          
        case NBIOT_SUB_CGMR:
          {
            nbiot_at_cmd_param_init(&g_at_cmd,AT_CGMR,NULL,CMD_EXCUTE,3000);
          }
          break;
          
        case NBIOT_SUB_CIMI:
          {
            nbiot_at_cmd_param_init(&g_at_cmd,AT_CIMI,NULL,CMD_EXCUTE,3000);
          }
          break;

        case NBIOT_SUB_CGSN:
          {
            nbiot_at_cmd_param_init(&g_at_cmd, AT_CGSN, "1", CMD_SET, 3000);
            
            //���������ظ���ϢΪ��
            //���ָ��ִ�����,û������������Ϣƥ��                                            
            //����Ϊ���������г�����               
            g_at_cmd.p_expectres = "CGSN";
          }
          break;             
          
          
        case NBIOT_SUB_NBAND:
          {
            nbiot_at_cmd_param_init(&g_at_cmd,AT_NBAND,NULL,CMD_READ,3000);
            //���������ظ���ϢΪ��
            //���ָ��ִ�����,û������������Ϣƥ��                                            
            //����Ϊ���������г�����               
            g_at_cmd.p_expectres = "NBAND";
          }
          break;
          
        default: 
          
          //ǿ�Ʊ�ʾ�ӽ��̽���
          g_nbiot_sm_status.sub_status = NBIOT_SUB_END;
        
          return FALSE;          
        
        }
    }
    
    else if (g_nbiot_sm_status.main_status == NBIOT_SIGNAL)
    {
        
        g_nbiot_sm_status.sub_status = NBIOT_SUB_END;
        return FALSE;
    }
    
    else if (g_nbiot_sm_status.main_status == NBIOT_CSCON)
    {
        
        g_nbiot_sm_status.sub_status = NBIOT_SUB_END;
        return FALSE;
    }    
    else if (g_nbiot_sm_status.main_status == NBIOT_RESET)
    {
        
        g_nbiot_sm_status.sub_status = NBIOT_SUB_END;
        return FALSE;
    }
    else if (g_nbiot_sm_status.main_status == NBIOT_NCONFIG)
    {
        
        g_nbiot_sm_status.sub_status = NBIOT_SUB_END;
        return FALSE;
    }     
    

    else if (g_nbiot_sm_status.main_status == NBIOT_NCDP_SERVER) 
    {
        g_nbiot_sm_status.sub_status++;
        
        switch (g_nbiot_sm_status.sub_status)
        {
                        
        case NBIOT_SUB_NCDP_CONNECT:          
          {
                                              
            //�����Ӧʱ��Ϊ300ms,������������Ϊ500ms                                    
            nbiot_at_cmd_param_init(&g_at_cmd, AT_QLWSREGIND, "0", CMD_SET, 500);
                                        
          }
          break;

                                     
        default: 
          
          //ǿ�Ʊ�ʾ�ӽ��̽���
          g_nbiot_sm_status.sub_status = NBIOT_SUB_END;
        
          return FALSE;          
        
        }      
          
    }
   
     else if (g_nbiot_sm_status.main_status == NBIOT_NCDP_SEND) 
    {
      
        g_nbiot_sm_status.sub_status = NBIOT_SUB_END;
      
        return FALSE;    
      
    } 
    
    else if (g_nbiot_sm_status.main_status == NBIOT_NCDP_CL) 
    {
      
        g_nbiot_sm_status.sub_status = NBIOT_SUB_END;
      
        return FALSE;     
    } 
       
    else if (g_nbiot_sm_status.main_status == NBIOT_NONE)   
    {  //��ֹ�����ط�
       return FALSE; 
    }   
    else 
    {
      
    }
    
    return TRUE;
}


/**
  * @brief ������Ϣ,�ص��û�ע������Ļص�����
  * @param  nbiot_handle  : ָ��nbiot�豸�����ָ��.
  * @param  buf           : ָ����Ӧ���ݵĶ�άָ�룬��Ԫ�ص�ֵΪһ��һά���ַ�������
  * @retval NBIOT_OK �ɹ�  
  */
static void nbiot_msg_send (nbiot_handle_t nbiot_handle, char**buf, int8_t is_ok)
{
    if (nbiot_handle == NULL)
    {
        return;
    }

     //�����ʱ����������
    if ((is_ok == NBIOT_ERROR_RETRY) || 
        (is_ok == NBIOT_ERROR_TIMEOUT)) {
        
         nbiot_handle->nbiot_cb(nbiot_handle->p_arg, (nbiot_msg_id_t)NBIOT_MSG_CMD_RETRY, strlen(buf[0]), buf[0]);  
        
         return;      
    }

    //����������ָ��ִ��  
    else if (is_ok == NBIOT_ERROR_NEXT)
    {

        nbiot_handle->nbiot_cb(nbiot_handle->p_arg, (nbiot_msg_id_t)NBIOT_MSG_CMD_NEXT, strlen(buf[0]), buf[0]);  
        
        return;
    }    
           
    //�������ϱ�������ִ��ʧ��
    else if(is_ok == FALSE)
    {
        nbiot_handle->nbiot_cb(nbiot_handle->p_arg, (nbiot_msg_id_t)NBIOT_MSG_CMD_FAIL, strlen(buf[0]), buf[0]);
        //��λ״̬��־
        nbiot_status_reset(); 
        return;
    }

    else if (g_nbiot_sm_status.main_status == NBIOT_INIT)
    {
        switch(g_nbiot_sm_status.sub_status)
        {
            
        case NBIOT_SUB_SYNC:
            
            break;
        
         case NBIOT_SUB_QLED:
                   
          
            break;          

        case NBIOT_SUB_CMEE:
          
            break;    

        case NBIOT_SUB_ATI:
          
            //�õ�ģ�������
            memcpy(g_firmware_info.name, buf[1], strlen(buf[1])); 
            
            break;

        //��ѯNB��״̬�Ƿ�׼����    
        case NBIOT_SUB_CPIN:
        {
            break; 
        }
                         
        case NBIOT_SUB_CSQ:
        {
            char *p_colon = strchr(buf[0],':');
          
            if (p_colon != NULL) 
            {            
                p_colon++;
                //ת����10��������
                uint8_t lqi =strtoul(p_colon,0, 10);
                //����ȡ��ÿ����ֵ��Ӧ��dbm��Χ
                int8_t rssi = -110 + (lqi << 1);
                uint8_t len = snprintf(buf[0],10,"%d",rssi);
                *(buf[0]+len) = 0;
                
                g_nbiot_connect_status.rssi = rssi;
                
                nbiot_handle->nbiot_cb(nbiot_handle->p_arg,(nbiot_msg_id_t)NBIOT_MSG_CSQ,len,buf[0]);
              
            }           
          
            break;
        }

        case NBIOT_SUB_QREGSWT:
            
            break;        
                  
        case NBIOT_SUB_CFUN:
            
            break;
        
        case NBIOT_SUB_CSCON:
            
            break;        


        case NBIOT_SUB_CEREG:
           
            NBIOT_DEBUG_INFO("reg status=%d\r\n", g_nbiot_connect_status.register_status);  

        
            break;
                
        case NBIOT_SUB_CIPCA_QUERY:
        {
            char *p_colon = strchr(buf[0],':');
                        
            if (p_colon != NULL) 
            {                
                p_colon++;
                
                //ת����10��������,�õ���ǰ������cid
                g_nbiot_connect_status.cid =strtoul(p_colon,0, 10);
            }
            
            break;
         }
               
        case NBIOT_SUB_CGATT:
            
            break;


        case NBIOT_SUB_CGATT_QUERY:
            
            break;

        case NBIOT_SUB_CGPADDR:
            
            break;  


//        case NBIOT_SUB_NNMI:
//            
//            break;
//        
//            
//        case NBIOT_SUB_NSMI:

//            break;
        
           
        case NBIOT_SUB_END:
            
            nbiot_handle->nbiot_cb(nbiot_handle->p_arg, (nbiot_msg_id_t)NBIOT_MSG_INIT,1,"OK");

            break;

        default:
          
            break;
        }
    }
    else if(g_nbiot_sm_status.main_status == NBIOT_NCONFIG)  
    {          
        if (g_nbiot_sm_status.sub_status == NBIOT_SUB_END)
        {
            nbiot_handle->nbiot_cb(nbiot_handle->p_arg, (nbiot_msg_id_t)NBIOT_MSG_NCONFIG,1,"OK"); 
        }        

    }

    else if(g_nbiot_sm_status.main_status == NBIOT_RESET)  
    {
         
        if (g_nbiot_sm_status.sub_status == NBIOT_SUB_END)
        {
            nbiot_handle->nbiot_cb(nbiot_handle->p_arg, (nbiot_msg_id_t)NBIOT_MSG_RESET,1,"OK"); 
        }        
    }    
    
    else if(g_nbiot_sm_status.main_status == NBIOT_INFO)
    {
        switch(g_nbiot_sm_status.sub_status)
        {
            
            //��ѯ����ע��״̬    
            case NBIOT_SUB_CEREG_QUERY:

                nbiot_handle->nbiot_cb(nbiot_handle->p_arg, (nbiot_msg_id_t)NBIOT_MSG_REG, 1, (char *)&g_nbiot_connect_status.register_status);

                break;        
                
                         
            case NBIOT_SUB_CGMI:
            {
                nbiot_handle->nbiot_cb(nbiot_handle->p_arg, (nbiot_msg_id_t)NBIOT_MSG_MID, strlen(buf[0]), buf[0]); 
                break;
            }
                        
            case NBIOT_SUB_CGMM:
            {
                nbiot_handle->nbiot_cb(nbiot_handle->p_arg, (nbiot_msg_id_t)NBIOT_MSG_MMODEL,strlen(buf[0]),buf[0]);  
                break;
            }
              
              
            case NBIOT_SUB_CGMR:
            {

                nbiot_handle->nbiot_cb(nbiot_handle->p_arg, (nbiot_msg_id_t)NBIOT_MSG_MREV,strlen(buf[0]),buf[0]);  
                break;            
            }
                                 
            case NBIOT_SUB_CIMI:
            {
                memcpy(g_firmware_info.IMSI,buf[0],15);
                g_firmware_info.IMSI[15] = 0;
                nbiot_handle->nbiot_cb(nbiot_handle->p_arg, (nbiot_msg_id_t)NBIOT_MSG_IMSI,strlen(buf[0]),buf[0]);
                break;
            }
                        
            case NBIOT_SUB_CGSN:
            {
                char *p_colon = strchr(buf[0],':');
                
                if (p_colon)
                {
                    p_colon = p_colon + 1;
                    memcpy(g_firmware_info.IMEI ,p_colon,15);
                    g_firmware_info.IMEI[15] = 0;
                    nbiot_handle->nbiot_cb(nbiot_handle->p_arg, (nbiot_msg_id_t)NBIOT_MSG_IMEI,15,(char*)g_firmware_info.IMEI);
                } 
                break;
            }
                                       
            case NBIOT_SUB_NBAND:
            {
                char *p_colon = strchr(buf[0],':');
                char *pFreq = NULL;
                if  (p_colon)
                {
                    p_colon++;
                    uint8_t hz_id = strtoul(p_colon,0,10);
                    if(hz_id == BAND_850MHZ_ID)
                    {
                        //850MHZ
                        pFreq = BAND_850MHZ_STR;
                    }
                    else if(hz_id == BAND_900MHZ_ID)
                    {
                        //900MHZ
                        pFreq = BAND_900MHZ_STR;
                    }
                    else if(hz_id == BAND_800MHZ_ID)
                    {
                        //800MHZ 
                        pFreq = BAND_800MHZ_STR;
                    }
                    else 
                    {
                        //700MHZ
                        pFreq = BAND_700MHZ_STR;
                    }
                    
                    nbiot_handle->nbiot_cb(nbiot_handle->p_arg, (nbiot_msg_id_t)NBIOT_MSG_BAND,strlen(pFreq),pFreq);
                }
                
                break;
            }
              
            case NBIOT_SUB_END:
            {
                nbiot_handle->nbiot_cb(nbiot_handle->p_arg, (nbiot_msg_id_t)NBIOT_MSG_INFO, 1, "OK");
                break;
            }
            
            default:
               break;
              
        }
    }
    else if(g_nbiot_sm_status.main_status == NBIOT_SIGNAL)
    {
        switch(g_nbiot_sm_status.sub_status) 
        { 
            case NBIOT_SUB_CSQ:
            {
                char *p_colon = strchr(buf[0],':');
              
                if (p_colon != NULL) 
                {            
                    p_colon++;
                    //ת����10��������
                    uint8_t lqi =strtoul(p_colon,0, 10);
                    //����ȡ��ÿ����ֵ��Ӧ��dbm��Χ
                    int8_t rssi = -110 + (lqi << 1);
                    uint8_t len = snprintf(buf[0],10,"%d",rssi);
                    *(buf[0]+len) = 0;
                    
                    g_nbiot_connect_status.rssi = rssi;
                    
                    nbiot_handle->nbiot_cb(nbiot_handle->p_arg,(nbiot_msg_id_t)NBIOT_MSG_CSQ,len,buf[0]);
                  
                }  
              
                break;
            } 
              
            case NBIOT_SUB_END:
            {
                nbiot_handle->nbiot_cb(nbiot_handle->p_arg,(nbiot_msg_id_t)NBIOT_MSG_SIGNAL,1,"OK");
                break;
            }

            default:
              
                break;
        }

    }
    
    else if(g_nbiot_sm_status.main_status == NBIOT_CSCON)
    {
        switch(g_nbiot_sm_status.sub_status) 
        { 
            case NBIOT_SUB_CSCON_QUERY:
            {
                char *p_colon = strchr(buf[0],',');
              
                if (p_colon != NULL) 
                {            
                    p_colon++;
                    //ת����10��������
                    uint8_t active_status =strtoul(p_colon,0, 10);
     
                    
                    g_nbiot_connect_status.cscon_status = active_status;
                    
                    nbiot_handle->nbiot_cb(nbiot_handle->p_arg,(nbiot_msg_id_t)NBIOT_MSG_CSCON, 1, (char *)&g_nbiot_connect_status.cscon_status);
                  
                }  
              
                break;
            } 
              
            case NBIOT_SUB_END:
            {
                nbiot_handle->nbiot_cb(nbiot_handle->p_arg,(nbiot_msg_id_t)NBIOT_MSG_CSCON, 1, (char *)&g_nbiot_connect_status.cscon_status);
                break;
            }

            default:
              
                break;
        }

    }    
    
    else if(g_nbiot_sm_status.main_status == NBIOT_NCDP_SERVER)
    {
        if (g_nbiot_sm_status.sub_status == NBIOT_SUB_END)
        { 
            g_nbiot_connect_status.connect_status = 1;         
            nbiot_handle->nbiot_cb(nbiot_handle->p_arg, (nbiot_msg_id_t)NBIOT_MSG_NCDP_SERVER, 1, "OK");
        }
    }  

    else if(g_nbiot_sm_status.main_status == NBIOT_NCDP_SEND)
    {
        if (g_nbiot_sm_status.sub_status == NBIOT_SUB_END)
        {
            char *p_buf_tmep = g_nbiot_send_desc.buf;
          
            nbiot_handle->nbiot_cb(nbiot_handle->p_arg, (nbiot_msg_id_t)NBIOT_MSG_NCDP_SEND, strlen(p_buf_tmep), p_buf_tmep);
        }
    }
    else if(g_nbiot_sm_status.main_status == NBIOT_NCDP_RECV)
    {
        if (g_nbiot_sm_status.sub_status == NBIOT_SUB_NCDP_RECV)
        {
            char *data_buf = g_nbiot_connect_status.data_offest; 
                   
            nbiot_handle->nbiot_cb(nbiot_handle->p_arg,(nbiot_msg_id_t)NBIOT_MSG_NCDP_RECV,strlen(data_buf),data_buf);
                       
        }
    }

    else if(g_nbiot_sm_status.main_status == NBIOT_NCDP_STATUS)
    {
        if (g_nbiot_sm_status.sub_status == NBIOT_SUB_NCDP_STATUS)
        {                      
            nbiot_handle->nbiot_cb(nbiot_handle->p_arg,(nbiot_msg_id_t)NBIOT_MSG_NCDP_STATUS, 1, (char *)&g_nbiot_connect_status.m2m_status);
          

                   
        }
    }

    else if(g_nbiot_sm_status.main_status == NBIOT_CSCON_STATUS)
    {
        if (g_nbiot_sm_status.sub_status == NBIOT_SUB_CSCON_STATUS)
        {                      
            nbiot_handle->nbiot_cb(nbiot_handle->p_arg, (nbiot_msg_id_t)NBIOT_MSG_CSCON_STATUS, 1, (char *)&g_nbiot_connect_status.cscon_status);
            //��λ״̬��־
            nbiot_status_reset();        
        }
    }    

    else if(g_nbiot_sm_status.main_status == NBIOT_NCDP_CL)
    {
        if (g_nbiot_sm_status.sub_status == NBIOT_SUB_END)
        {
             g_nbiot_connect_status.connect_status = 0;  
          
             char *p_buf_tmep = NULL;
                                        
             if (g_nbiot_connect_status.connect_type == NBIOT_NCDP)
             {
                 p_buf_tmep = "ncdp close";
             }         
            
             nbiot_handle->nbiot_cb(nbiot_handle->p_arg,(nbiot_msg_id_t)NBIOT_MSG_NCDP_CLOSE, strlen(p_buf_tmep), p_buf_tmep);
        }
    }
    else
    {

    }    
}

/**
  * @brief  ָ����Ӧ�����������������������״̬���ԣ�������״̬��ִ��.
  * @param  nbiot_handle  : ָ��nbiot�豸�����ָ��.
  * @param  cmd_response  : ָ����Ӧ�Ľ����ָ����Ӧ�ɹ��������ֵΪTrue
  * @retval ���� AT_CMD_OK ��һ��ĺ�ֵ
  */
static uint8_t nbiot_response_handle (nbiot_handle_t nbiot_handle, uint8_t cmd_response)
{
    uint8_t next_cmd = AT_CMD_OK;
      
    if (cmd_response)
    {
        if (g_at_cmd.cmd_action & ACTION_OK_AND_NEXT)
        {
            next_cmd = AT_CMD_NEXT;
        }        
        else if (g_at_cmd.cmd_action & ACTION_OK_WAIT)   
        {
            //��������ִ�гɹ����ȴ�
            next_cmd = AT_CMD_WAIT;             
        }
        else  
        {
            //��������ִ�гɹ����˳��ô�״̬���������������������
            g_at_cmd.cmd_action = ACTION_OK_EXIT;             
        }
    }
    else
    {
        if(g_at_cmd.cmd_action & ACTION_ERROR_AND_TRY)
        {
            g_at_cmd.have_tried++;

            if (g_at_cmd.have_tried < g_at_cmd.cmd_try)
            {
                //�����ط�������
                nbiot_at_cmd_send(nbiot_handle, &g_at_cmd);
            }
            else
            {
                  
               //���Դﵽ���������ñ��������ԣ���������ִ�д�����˳��ô�״̬���������������������
                g_at_cmd.cmd_action = ACTION_ERROR_EXIT;
                
            }
        }        
        else if (g_at_cmd.cmd_action & ACTION_OK_WAIT)   
        {
            //��������ִ�д��󽫵ȴ�
            next_cmd = AT_CMD_WAIT;             
        }
                
        else if (!(g_at_cmd.cmd_action & ACTION_ERROR_EXIT))  
        {
            //����ִ�д�������������ִ����һ������
            next_cmd = TRUE;
        }
        else 
        {
            //nerver reach here  
        }
    }
    
    return next_cmd;
}

/**
  * @brief  ��nbiotģ�鷢��ATָ��.
  * @param  nbiot_handle  : ָ��nbiot�豸�����ָ��.
  * @param  cmd_handle    : ��Ҫ����ָ����Ϣ���
  * @retval ���� 0 �����ͳɹ�
  */
int nbiot_at_cmd_send(nbiot_handle_t nbiot_handle, at_cmdhandle cmd_handle)
{
    int strLen = 0;
    
    int ret = 0;
        
    if (nbiot_handle == NULL || cmd_handle == NULL)
    {
       return NBIOT_ERROR;
    }
    
    //������������ݼ�����    
    strLen = cmd_generate(cmd_handle);

    //�ص�����ͺ���
    ret = nbiot_handle->p_drv_funcs->nbiot_send_data(nbiot_handle, 
                                                     (uint8_t*)g_nbiot_send_desc.buf, 
                                                     strLen,                                                    
                                                     cmd_handle->max_timeout);   
    return ret;
}

/**
  * @brief  ��������nbiotģ�������.
  * @param  nbiot_handle  : ָ��nbiot�豸�����ָ��.
  * @param  pData    : ��Ҫ������Ϣ������ָ��
  * @param  size     : ��Ҫ������Ϣ�������ĳ���
  * @param  Timeout  : ���ý��յĳ�ʱֵ
  * @retval ���� 0 �����ͳɹ�
  */
static int nbiot_data_recv(nbiot_handle_t nbiot_handle, uint8_t *pData, uint16_t size, uint32_t Timeout)
{   
    int ret = 0;
        
    if (nbiot_handle == NULL)
    {
       return NBIOT_ERROR;
    }
    
    //�ص��������ݺ��� 
    ret = nbiot_handle->p_drv_funcs->nbiot_recv_data(nbiot_handle, pData, size, Timeout);      
    
    return ret;
}


/**
  * @brief  nbiotģ���豸ʵ����ʼ�� .
  * @param  nbiot_handle  : ָ��nbiot�豸�����ָ��.
  * @param  cmd_handle    : ��Ҫ����ָ����Ϣ���
  * @retval ���� nbiotģ���豸�����ָ�� 
  */
nbiot_handle_t nbiot_dev_init(uart_handle_t nbiot_handle)
{
     //����豸�ṹ��
     g_nbiot_dev.p_uart_dev       = nbiot_handle;
     g_nbiot_dev.p_drv_funcs      = &drv_funcs; 

     g_nbiot_dev.p_nbiot_cmd      = &g_at_cmd;    
     g_nbiot_dev.p_socket_info    = g_socket_info;
     g_nbiot_dev.p_firmware_info  = &g_firmware_info;
     g_nbiot_dev.p_sm_status      = &g_nbiot_sm_status;
     g_nbiot_dev.p_connect_status = &g_nbiot_connect_status;
  
     g_nbiot_dev.frame_format     = 0;  
    
     //ע�ᴮ���շ��¼��ص�����
     uart_event_registercb(nbiot_handle, __uart_event_cb_handle, &g_nbiot_dev);     
    
     return &g_nbiot_dev;    
}

/**
  * @brief  ע��nbiotģ���¼��ص�����.
  * @param  cb     : ģ���豸�ص�.
  * @param  p_arg  : ģ���豸�ص���������
  * @retval ���� nbiotģ���豸�����ָ�� 
  */
void nbiot_event_registercb (nbiot_handle_t nbiot_handle, nbiot_cb cb, void *p_arg)
{  
    if(cb != 0)
    {
        nbiot_handle->nbiot_cb  = (nbiot_cb)cb;
        nbiot_handle->p_arg     = p_arg;
    }
}








