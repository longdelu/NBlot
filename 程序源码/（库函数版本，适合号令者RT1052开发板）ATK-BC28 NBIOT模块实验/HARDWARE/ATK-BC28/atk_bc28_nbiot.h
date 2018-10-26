#ifndef ATK_BC28_NBIOT_H
#define ATK_BC28_NBIOT_H

#include "atk_bc28.h"
#include "atk_delay.h"

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

/**
  * @brief nbiot demo Ӧ��app״̬����
  */
typedef enum nbiot_app_status
{
    NBIOT_APP_NONE,        
    NBIOT_APP_INIT        = NBIOT_INIT,         // ��ʼ������
    NBIOT_APP_INFO        = NBIOT_INFO,         // ��ȡ NB ģ�鳧�̼��̼���Ƶ�ε���Ϣ
    NBIOT_APP_SIGNAL      = NBIOT_SIGNAL,       // ��ȡ�ź�����
    NBIOT_APP_TCPUDP_CR   = NBIOT_TCPUDP_CR,    // ���� TCP/UDP
    NBIOT_APP_TCPUDP_CL   = NBIOT_TCPUDP_CL,    // �ر� TCP/UDP
    NBIOT_APP_TCPUDP_SEND = NBIOT_TCPUDP_SEND,  // �����Ѿ�������TCP/UDP��������
    NBIOT_APP_TCPUDP_RECV = NBIOT_TCPUDP_RECV,  // TCP/UDP������Ϣ 
    NBIOT_APP_SOCKET_ERR  = NBIOT_SOCKET_ERR,   // SOCKET���ӷ�������
    NBIOT_APP_CoAP_SEVER  = NBIOT_CoAP_SEVER,   // CoAPԶ�̷���������
    NBIOT_APP_CoAP_CLIENT = NBIOT_CoAP_CLIENT,  // CoAP�ͻ������ӵ�ַ����
    NBIOT_APP_CoAP_SEND   = NBIOT_CoAP_SEND,    // CoAP������Ϣ
    NBIOT_APP_CoAP_RECV   = NBIOT_CoAP_RECV,    // CoAP������Ϣ
    NBIOT_APP_CoAP_CL     = NBIOT_CoAP_CL,      // �ر�CoAP
    NBIOT_APP_NCDP_SERVER = NBIOT_NCDP_SERVER,  // NCDP�ͻ������ӵ�ַ����
    NBIOT_APP_NCDP_SEND   = NBIOT_NCDP_SEND,    // NCDP������Ϣ
    NBIOT_APP_NCDP_RECV   = NBIOT_NCDP_RECV,    // NCDP������Ϣ
    NBIOT_APP_NCDP_STATUS = NBIOT_NCDP_STATUS,  // NCDP����״̬��Ϣ
    NBIOT_APP_NCDP_CL     = NBIOT_NCDP_CL,      // �ر�NCDP 
    NBIOT_APP_RESET       = NBIOT_RESET,        // ��λNB
    NBIOT_APP_NCONFIG     = NBIOT_NCONFIG,      // ʹ��/�����Զ���������
    NBIOT_APP_END          
}nbiot_app_status_t;



/**
  * @brief nbiot��Ϣid, �ص�������ʹ��
  */
typedef enum nbiot_msg_id
{
    NBIOT_MSG_NONE,

    NBIOT_MSG_INIT,       //��ʼ�������Ϣ

    NBIOT_MSG_INFO,       //�̼���Ϣ��ȡ�����Ϣ
    
    NBIOT_MSG_REG,        //ע����Ϣ��ȡ�����Ϣ      
  
    NBIOT_MSG_IMEI,       //�ƶ��豸������ȡ�����Ϣ   
    NBIOT_MSG_IMSI,
  
    NBIOT_MSG_MID,        //����ID��ȡ�����Ϣ
    NBIOT_MSG_MMODEL,     //ģ���ͺŻ�ȡ�����Ϣ
    NBIOT_MSG_MREV,       //����汾�Ż�ȡ�����Ϣ
    NBIOT_MSG_BAND,       //����Ƶ�λ�ȡ�����Ϣ
  
    NBIOT_MSG_CSQ,
    NBIOT_MSG_STATUS,  
    NBIOT_MSG_SIGNAL,     //�ź�ǿ�Ȼ�ȡ�����Ϣ
    NBIOT_MSG_CSCON,      //��ȡ���ӣ����״̬   

    NBIOT_MSG_TCPUDP_CREATE,
    NBIOT_MSG_TCPUDP_CLOSE,
    NBIOT_MSG_TCPUDP_SEND,
    NBIOT_MSG_TCPUDP_RECV,
    
    NBIOT_MSG_SOCKET_ERROR, //�յ�socket������Ϣ    

    NBIOT_MSG_COAP_SERVER,
    NBIOT_MSG_COAP_CLIENT,
    NBIOT_MSG_COAP_SEND,
    NBIOT_MSG_COAP_RECV,
    NBIOT_MSG_COAP_CLOSE,
    
    NBIOT_MSG_NCDP_SERVER,
    NBIOT_MSG_NCDP_SEND,
    NBIOT_MSG_NCDP_RECV,
    NBIOT_MSG_NCDP_STATUS,
    NBIOT_MSG_NCDP_CLOSE,
    
    NBIOT_MSG_CSCON_STATUS,    

    NBIOT_MSG_RESET,        //NB��λ�����Ϣ
    NBIOT_MSG_NCONFIG,      //�Զ��������������Ϣ

    NBIOT_MSG_CMD_RETRY,    //�������������Ϣ
    
    NBIOT_MSG_CMD_NEXT,     //��������������������ִ����һ��������Ϣ
    
    NBIOT_MSG_CMD_FAIL,     //���������˳�ִ����Ϣ 

    NBIOT_MSG_END           
  
}nbiot_msg_id_t;

/**
  * @brief nbiot ������Ϣ�Ķ���״̬����
  */
#define  NON_MESSAGE          "0X0000"
#define  NON_MESSAGE_RELEASE  "0X0001"
#define  CON_MESSAGE          "0X0100"
#define  CON_MESSAGE_RELEASE  "0X0101"

/**
  * @brief  ����nbiotģ���ʼ������ע��
  * @param  nbiot_handle   : ָ��nbiot�豸�����ָ��.
  * @retval NBIOT_OK ���ó�ʼ���ɹ�  NBIOT_ERROR ���ó�ʼ��ʧ��
  */
int nbiot_reg_init (nbiot_handle_t nbiot_handle);

/**
  * @brief ��������nbiotģ��
  * @param  nbiot_handle   : ָ��nbiot�豸�����ָ��.
  * @retval NBIOT_OK ���������ɹ�  NBIOT_ERROR ��������ʧ��
  */
int nbiot_reboot(nbiot_handle_t nbiot_handle);

/**
  * @brief ʹ��/����nbiotģ���Զ�����
  * @param  nbiot_handle  : ָ��nbiot�豸�����ָ��.
  * @param  auto_flag     : 0 �����Զ����� 1 ʹ���Զ�����  
  * @retval NBIOT_OK ���óɹ�  NBIOT_ERROR ����ʧ��
  */
int nbiot_nconfig(nbiot_handle_t nbiot_handle, uint8_t auto_flag);


/**
  * @brief ��ȡnbiotģ�����Ϣ
  * @param  nbiot_handle  : ָ��nbiot�豸�����ָ��.
  * @retval NBIOT_OK ���óɹ�  NBIOT_ERROR ����ʧ��
  */
int nbiot_info_get(nbiot_handle_t nbiot_handle);

/**
  * @brief ��ȡnbiotģ����ź�����
  * @param  nbiot_handle  : ָ��nbiot�豸�����ָ��.
  * @retval NBIOT_OK ���óɹ�  NBIOT_ERROR ����ʧ��
  */
int nbiot_signal_get(nbiot_handle_t nbiot_handle);

                                                               
/**
  * @brief ����ncdp������
  * @param  nbiot_handle  : ָ��nbiot�豸�����ָ��.
  * @param  type          : �������ͣ�ֵ��Ϊö�ٵ��е�ֵ
  * @retval NBIOT_OK ���óɹ�  NBIOT_ERROR ����ʧ��  
  */
int nbiot_ncdp_update(nbiot_handle_t nbiot_handle, nbiot_connect_type_t type);


/**
  * @brief �ر�ncdp
  * @param  nbiot_handle  : ָ��nbiot�豸�����ָ��.
  * @param  type          : �������ͣ�ֵ��Ϊö�ٵ��е�ֵ
  * @retval NBIOT_OK ���óɹ�  NBIOT_ERROR ����ʧ��  
  */
int nbiot_ncdp_close(nbiot_handle_t nbiot_handle, nbiot_connect_type_t type);

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
                            char *mode);


/**
  * @brief  ����������ÿ�����ֽ��������һ��һ�ֽ�ʮ��������(�ַ�).
  * @param  p_buf     : Դ���ݻ������׵�ַ.
  * @param  len       : Դ����������.
  * @note  ���øýӿڻ�ı�Դ�������ĳ���
  * @retval None
  */
void nbiot_buf2hex (char *p_buf , int len);


/**
  * @brief  ����������ÿ�����ֽ��������һ��һ�ֽ�ʮ��������(�ַ�).
  * @param  src_buf   : Դ���ݻ������׵�ַ.
  * @param  dest_buf  : Ŀ�����ݻ������׵�ַ.
  * @param  len       : Դ����������.
  * @note   ȷ��Ŀ�Ļ����������㹻����Դ������������,�䳤������ΪԴ���������ȵ�һ��
  * @retval None
  */
void nbiot_srcbuf2hexbuf (char *src_buf ,char *dest_buf, int len);


/**
  * @brief  ������ÿ1���ַ��������Ϊ�����ַ�����ʾ
  * @eg     �����ַ�'1'Ϊʮ������0x31,���������ַ�"31"����ʾ�ַ�'1'
  * @param  src_buf   : Դ���ݻ������׵�ַ.
  * @param  dest_buf  : Ŀ�����ݻ������׵�ַ.
  * @param  len       : Դ����������.
  * @note   ȷ��Ŀ�Ļ����������㹻����Դ������������,�䳤������ΪԴ���������ȵ�2��
  * @retval None
  */
void nbiot_srcbuf2hexchrbuf (char *src_buf ,char *dest_buf, int len);


#endif


