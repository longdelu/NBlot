
////////////////////////////////////////////////////////////////////////////////// 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F429������
//����1��ʼ��           
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2018/7/11
//�汾��V1.5
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//********************************************************************************
#include "atk_bc28_nbiot.h"
#include "atk_delay.h"

#define NBIOT_DEBUG                   
#ifdef NBIOT_DEBUG
#define NBIOT_DEBUG_INFO(...)    (int)printf(__VA_ARGS__)
#else
#define NBIOT_DEBUG_INFO(...)
#endif

static char cmd_buf_temp[NBIOT_RECV_BUF_MAX_LEN] = {0};

/**
  * @brief  ����nbiotģ���ʼ������ע��
  * @param  nbiot_handle   : ָ��nbiot�豸�����ָ��.
  * @retval NBIOT_OK ���ó�ʼ���ɹ�  NBIOT_ERROR ���ó�ʼ��ʧ��
  */
int nbiot_reg_init (nbiot_handle_t nbiot_handle)
{    
    if (nbiot_handle->p_sm_status->main_status != NBIOT_NONE)
    {
        return NBIOT_ERROR;
    }

    //���ָ��������ṹ��
    nbiot_at_cmd_param_init(nbiot_handle->p_nbiot_cmd, AT_SYNC, NULL, CMD_EXCUTE, 3000);

    //����NBIOT_INIT״̬
    nbiot_handle->p_sm_status->main_status = NBIOT_INIT;
    nbiot_handle->p_sm_status->sub_status  = NBIOT_SUB_SYNC;

    //����ָ��
    nbiot_at_cmd_send(nbiot_handle, nbiot_handle->p_nbiot_cmd);
  
    return NBIOT_OK;
}


/**
  * @brief ��������nbiotģ��
  * @param  nbiot_handle   : ָ��nbiot�豸�����ָ��.
  * @retval NBIOT_OK ���������ɹ�  NBIOT_ERROR ��������ʧ��
  */
int nbiot_reboot(nbiot_handle_t nbiot_handle)
{

    if (nbiot_handle->p_sm_status->main_status != NBIOT_NONE)
    {
        return NBIOT_ERROR;
    }
    
    //�������ʱʱ��Ϊ300ms��Ϊ�����������ﳬʱ����Ϊ500ms    
    nbiot_at_cmd_param_init(nbiot_handle->p_nbiot_cmd, AT_NRB, NULL, CMD_EXCUTE, 500);

    //����NBIOT_REBOOT״̬
    nbiot_handle->p_sm_status->main_status = NBIOT_RESET;
    nbiot_handle->p_sm_status->sub_status  = NBIOT_SUB_RESET;

    nbiot_at_cmd_send(nbiot_handle, nbiot_handle->p_nbiot_cmd);
    
    return NBIOT_OK;
}

/**
  * @brief ʹ��/����nbiotģ���Զ�����
  * @param  nbiot_handle  : ָ��nbiot�豸�����ָ��.
  * @param  auto_flag     : 0 �����Զ����� 1 ʹ���Զ�����  
  * @retval NBIOT_OK ���óɹ�  NBIOT_ERROR ����ʧ��
  */
int nbiot_nconfig(nbiot_handle_t nbiot_handle, uint8_t auto_flag)
{
    char *p_auto_nconfig = NULL;
    
    if (nbiot_handle->p_sm_status->main_status != NBIOT_NONE)
    {
        return NBIOT_ERROR;
    }
        
    if (auto_flag == 0) 
    {
        
        p_auto_nconfig = "AUTOCONNECT,FALSE";
    }    
    else 
    {
        p_auto_nconfig = "AUTOCONNECT,TRUE"; 
    }
         
    //���ָ��������ṹ�� 
    //�������ʱʱ��Ϊ300ms��Ϊ�����������ﳬʱ����Ϊ500ms      
    nbiot_at_cmd_param_init(nbiot_handle->p_nbiot_cmd, AT_NCONFIG, p_auto_nconfig, CMD_SET, 500);

    //����NBIOT_REBOOT״̬
    nbiot_handle->p_sm_status->main_status = NBIOT_NCONFIG;
    nbiot_handle->p_sm_status->sub_status  = NBIOT_SUB_NCONFIG;

    nbiot_at_cmd_send(nbiot_handle, nbiot_handle->p_nbiot_cmd);
    
    return NBIOT_OK;
}



/**
  * @brief ��ȡnbiotģ�����Ϣ
  * @param  nbiot_handle  : ָ��nbiot�豸�����ָ��.
  * @retval NBIOT_OK ���óɹ�  NBIOT_ERROR ����ʧ��
  */
int nbiot_info_get(nbiot_handle_t nbiot_handle)
{

    if (nbiot_handle->p_sm_status->main_status != NBIOT_NONE)
    {
        return NBIOT_ERROR;
    }
    
    //���ָ��������ṹ��
    nbiot_at_cmd_param_init(nbiot_handle->p_nbiot_cmd, AT_CEREG, NULL, CMD_READ, 3000);

    //����NBIOT_INFO״̬
    nbiot_handle->p_sm_status->main_status = NBIOT_INFO;
    nbiot_handle->p_sm_status->sub_status  = NBIOT_SUB_CEREG_QUERY;

    //����ָ��
    nbiot_at_cmd_send(nbiot_handle, nbiot_handle->p_nbiot_cmd);
    
    return NBIOT_OK;
}


/**
  * @brief ��ȡnbiotģ����ź�����
  * @param  nbiot_handle  : ָ��nbiot�豸�����ָ��.
  * @retval NBIOT_OK ���óɹ�  NBIOT_ERROR ����ʧ��
  */
int nbiot_signal_get(nbiot_handle_t nbiot_handle)
{

    if (nbiot_handle->p_sm_status->main_status != NBIOT_NONE)
    {
        return NBIOT_ERROR;
    }
        
    nbiot_at_cmd_param_init(nbiot_handle->p_nbiot_cmd, AT_CSQ, NULL, CMD_EXCUTE, 3000);

    //����NBIOT_SIGNAL״̬
    nbiot_handle->p_sm_status->main_status = NBIOT_SIGNAL;
    nbiot_handle->p_sm_status->sub_status  = NBIOT_SUB_CSQ;

    nbiot_at_cmd_send(nbiot_handle, nbiot_handle->p_nbiot_cmd);
    
    return NBIOT_OK;
}

/**
  * @brief ����ncdp������
  * @param  nbiot_handle  : ָ��nbiot�豸�����ָ��.
  * @param  type          : �������ͣ�ֵ��Ϊö�ٵ��е�ֵ
  * @retval NBIOT_OK ���óɹ�  NBIOT_ERROR ����ʧ��  
  */
int nbiot_ncdp_update(nbiot_handle_t nbiot_handle, nbiot_connect_type_t type)
{     
    if (nbiot_handle->p_sm_status->main_status != NBIOT_NONE)
    {
        return NBIOT_ERROR;
    }
    
    if (type != NBIOT_NCDP)
    {
        return NBIOT_NOTSUPPORT;
    }
    
    nbiot_handle->p_connect_status->connect_type = NBIOT_NCDP;

    //����ʹ��ջ�ϵ��ڴ�������ݣ�Ҫ��Ȼ�ط�������ջ�ϵ��ڴ������ͷŵ�ʱ�����   
    memset(cmd_buf_temp, 0, sizeof(cmd_buf_temp));       
                
    //��������ķ���ֵΪ��Ҫ��ʽ��д��ĳ��ȣ��������ַ������������Զ���������ַ�
    uint16_t ncdp_cn_len = snprintf(cmd_buf_temp,
                                    sizeof(cmd_buf_temp) -1,"%s,%s",                                                                                             
                                    REMOTE_SERVER_IP,
                                    REMOTE_COAP_PORT);
                                                                       
    //�����Ӧʱ��Ϊ300ms�����������ﱣ��Ϊ500ms                                           
    nbiot_at_cmd_param_init(nbiot_handle->p_nbiot_cmd, AT_NCDP, cmd_buf_temp, CMD_SET, 500);                                   
                                       
    //�������CDP������
    nbiot_handle->p_sm_status->main_status = NBIOT_NCDP_SERVER;
    nbiot_handle->p_sm_status->sub_status  = NBIOT_SUB_NCDP_SERVER;

    //����ָ��
    nbiot_at_cmd_send(nbiot_handle, nbiot_handle->p_nbiot_cmd);
    
    return NBIOT_OK;
}


/**
  * @brief �ر�ncdp
  * @param  nbiot_handle  : ָ��nbiot�豸�����ָ��.
  * @param  type          : �������ͣ�ֵ��Ϊö�ٵ��е�ֵ
  * @retval NBIOT_OK ���óɹ�  NBIOT_ERROR ����ʧ��  
  */
int nbiot_ncdp_close(nbiot_handle_t nbiot_handle, nbiot_connect_type_t type)
{  
    if (nbiot_handle->p_sm_status->main_status != NBIOT_NONE)
    {
        return NBIOT_ERROR;
    }
           
    if (type != NBIOT_NCDP)
    {
        return NBIOT_NOTSUPPORT;
    }   
                                                                                   
    nbiot_at_cmd_param_init(nbiot_handle->p_nbiot_cmd, AT_QLWSREGIND, "1", CMD_SET, 500);  

    //����ncdp�ر�״̬,�����Ӧʱ�䲻��
    nbiot_handle->p_sm_status->main_status = NBIOT_NCDP_CL;
    nbiot_handle->p_sm_status->sub_status  = NBIOT_SUB_NCDP_CL;

    nbiot_at_cmd_send(nbiot_handle, nbiot_handle->p_nbiot_cmd);
        
    return NBIOT_OK;
}

/**
  * @brief ��ѯ�״̬
  * @param  nbiot_handle  : ָ��nbiot�豸�����ָ��.
  * @retval NBIOT_OK ���óɹ�  NBIOT_ERROR ����ʧ��  
  */
int nbiot_cscon_query (nbiot_handle_t nbiot_handle)
{
  
    if (nbiot_handle->p_sm_status->main_status != NBIOT_NONE)
    {
        return NBIOT_ERROR;
    }
                                                                                              
    nbiot_at_cmd_param_init(nbiot_handle->p_nbiot_cmd, AT_CSCON, NULL, CMD_READ, 500);  

    nbiot_handle->p_sm_status->main_status = NBIOT_CSCON;
    nbiot_handle->p_sm_status->sub_status  = NBIOT_SUB_CSCON_QUERY;

    nbiot_at_cmd_send(nbiot_handle, nbiot_handle->p_nbiot_cmd);
        
    return NBIOT_OK;
}


/**
  * @brief ��hex��ʽ�ַ�������ҵ���߼�����,������Ч���ȱ�����ż������С�Ҳ��ܰ��������ַ�
  * @param  nbiot_handle  : ָ��nbiot�豸�����ָ��.
  * @param  type          : �������ͣ�ֵ��Ϊö�ٵ��е�ֵ
  * @param  mode          : һ����ΪΪNULL����
  * @retval NBIOT_OK ���óɹ�  NBIOT_ERROR ����ʧ��  NBIOT_NOTSUPPORT ������Ч���Ȳ�Ϊż�������� 
  */
int nbiot_ncdp_send_hexstr (nbiot_handle_t nbiot_handle, 
                            int len, 
                            char *msg, 
                            nbiot_connect_type_t type, 
                            char *mode)
{ 
    
    int16_t msg_len = 0;
    char buf_head[3] = {0};     
    char buf_head_tmp[5] = {0};    
    
    char *p_at_cmd = NULL;
    
    if (nbiot_handle->p_sm_status->main_status != NBIOT_NONE)
    {
        return NBIOT_ERROR;
    }
  
    //�����Ͳ�ΪNCDPʱ���򶼷��͵����ݸ�����Ϊż����ʱ
    if ((type != NBIOT_NCDP) || ((strlen(msg) % 2) != 0))
    {
        return NBIOT_NOTSUPPORT;
    } 
      
    //������ݳ���Ϊ��Ч���ݼ���ͷ��
    int16_t str_len = (NBIOT_SEND_BUF_MAX_LEN - 24) ;

    //����ʹ��ջ�ϵ��ڴ�������ݣ�Ҫ��Ȼ�ط�������ջ�ϵ��ڴ������ͷŵ�ʱ�����   
    memset(cmd_buf_temp, 0, sizeof(cmd_buf_temp));
    
    buf_head[1] =  (strlen(msg) / 2) % 256;
    buf_head[0] =  (strlen(msg) / 2) / 256;
    
    //ת�����ݸ�ʽ                     
    nbiot_srcbuf2hexchrbuf(buf_head, buf_head_tmp, 2); 
    
           
    if (mode == NULL) 
    {

        msg_len = snprintf(cmd_buf_temp,
                               str_len,
                               "%d,%s%s",
                               (strlen(msg) / 2 + 2),
                                buf_head_tmp,        
                                msg);
        
        p_at_cmd = AT_QLWULDATA;
    }
    
    else 
    {
        msg_len = snprintf(cmd_buf_temp,
                               str_len,
                               "%d,%s%s,%s",
                               (strlen(msg) / 2 + 2), 
                                buf_head_tmp,
                                msg,
                                mode); 

        p_at_cmd = AT_QLWULDATAEX;        
    }
    
    if (msg_len < 0) 
    {
      
        return NBIOT_ERROR;
    }
    
                                                          
    //����ncdp���ݷ�����������Ӧʱ�䲻��
    nbiot_at_cmd_param_init(nbiot_handle->p_nbiot_cmd, p_at_cmd, cmd_buf_temp, CMD_SET, 3000);
    
    //����ncdp���ݷ���״̬
    nbiot_handle->p_sm_status->main_status = NBIOT_NCDP_SEND;
    nbiot_handle->p_sm_status->sub_status  = NBIOT_SUB_NCDP_SEND;

    //����ָ��
    nbiot_at_cmd_send(nbiot_handle, nbiot_handle->p_nbiot_cmd);
         
    return NBIOT_OK;
}

/**
  * @brief ��1���ַ�ת��Ϊ10��������
  * @param  nbiot_handle  : ָ��nbiot�豸�����ָ��.
  * @param  chr           : �ַ�,0~9/A~F/a~F
  * @retval chr��Ӧ��10������ֵ  
  */
u8 nbiot_chr2hex (u8 chr)
{
    if(chr>='0'&&chr<='9')
    {
      return chr - '0';
    }
    else if(chr>='A'&&chr<='F')
    {
      return (chr-'A'+10);
    }
    else if(chr>='a'&&chr<='f')
    {
      return (chr-'a'+10); 
    }
    else
    {     
      return 0;
    }                
}

/**
  * @brief ��1��16��������ת��Ϊ�ַ�
  * @param  nbiot_handle  : ָ��nbiot�豸�����ָ��.
  * @param  hex           : ʮ��������
  * @retval ����ʮ����������Ӧ���ַ�  
  */
u8 nbiot_hex2chr (u8 hex)
{
    if(hex<=9)
    {
      return hex+'0';
    }
    else if(hex>=10&&hex<=15)
    {
      return (hex-10+'A'); 
    }   
    else 
    { 
      return '0';
    }              
}



/**
  * @brief  ����������ÿ�����ֽ��������һ��һ�ֽ�ʮ��������(�ַ�).
  * @param  p_buf     : Դ���ݻ������׵�ַ.
  * @param  len       : Դ����������.
  * @note  ���øýӿڻ�ı�Դ�������ĳ���
  * @retval None
  */
void nbiot_buf2hex (char *p_buf , int len)
{
    int i = 0; 
   
    char tmp  = 0;
    char tmp1 = 0;
     
    for (i = 0; i < len; i=i+2)
    {
        tmp = nbiot_chr2hex(p_buf[i]);       
        tmp1 = nbiot_chr2hex(p_buf[i + 1]);      
        p_buf[i / 2] = (tmp << 4)  |  tmp1;                    
    } 
    
    p_buf[i / 2] = 0;      
}


/**
  * @brief  ������ÿ1���ַ��������Ϊ�����ַ�����ʾ
  * @eg     �����ַ�'1'Ϊʮ������0x31,���������ַ�"31"����ʾ�ַ�'1'
  * @param  src_buf   : Դ���ݻ������׵�ַ.
  * @param  dest_buf  : Ŀ�����ݻ������׵�ַ.
  * @param  len       : Դ����������.
  * @note   ȷ��Ŀ�Ļ����������㹻����Դ������������,�䳤������ΪԴ��������Ч���ݳ��ȵ�2��
  * @retval None
  */
void nbiot_srcbuf2hexchrbuf (char *src_buf ,char *dest_buf, int len)
{
    int i = 0; 
   
    char tmp  = 0;
    char tmp1 = 0;
     
    for (i = 0; i < len; i = i+1)
    {
        tmp = ((src_buf[i] >> 4) & 0x0f);      
        tmp1 = ((src_buf[i]) & 0x0f);         
        dest_buf[2 * i] = nbiot_hex2chr(tmp); 
        dest_buf[2 * i + 1] = nbiot_hex2chr(tmp1);        
    }
    
    dest_buf[2 * i + 2] = 0;      
}


/**
  * @brief  ����������ÿ�����ֽ��������һ��һ�ֽ�ʮ��������(�ַ�).
  * @param  src_buf   : Դ���ݻ������׵�ַ.
  * @param  dest_buf  : Ŀ�����ݻ������׵�ַ.
  * @param  len       : Դ����������.
  * @note   ȷ��Ŀ�Ļ����������㹻����Դ������������,�䳤������ΪԴ��������Ч���ݳ��ȵ�һ��
  * @retval None
  */
void nbiot_srcbuf2hexbuf (char *src_buf ,char *dest_buf, int len)
{
    int i = 0; 
   
    char tmp  = 0;
    char tmp1 = 0;
    
    for (i = 0; i < len; i=i+2)
    {
        tmp = nbiot_chr2hex(src_buf[i]);       
        tmp1 = nbiot_chr2hex(src_buf[i + 1]);  
       
        dest_buf[i / 2] = (tmp << 4)  |  tmp1;                    
    }     
        
    dest_buf[i / 2] = 0;      
}


