#ifndef _ATK_BC28_H
#define _ATK_BC28_H
#include "atk_sys.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "atk_nbiot_usart.h"
//////////////////////////////////////////////////////////////////////////////////    
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F429������
//����1��ʼ��         
//����ԭ��@ALIENTEK
//������̳:www.openedv.csom
//�޸�����:2015/6/23
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
//********************************************************************************
//V1.0 ��һ��Ӧ�� 
//////////////////////////////////////////////////////////////////////////////////

 /**
  * @brief ʹ�ö�Ӧiotƽ̨�ĺ꿪��
  */
#define CTNB_IOT                    1    //����iotƽ̨
//#define HUAWEI_IOT                  1    //��Ϊiotƽ̨
//#define EASY_IOT                    1    //Easyiotƽ̨

/**
  * @brief ��ʼ��������ע���������
  */
#define AT_NCONFIG     "AT+NCONFIG"    //ʹ��/�����Զ���������
#define AT_QLEDMODE    "AT+QLEDMODE"   //ʹ��/����LED
#define AT_QREGSWT     "AT+QREGSWT"    //IOTƽ̨ע��ģʽѡ��
#define AT_NRB         "AT+NRB"        //�������� 
#define AT_SYNC        "AT"
#define AT_QLEDMODE    "AT+QLEDMODE"
#define AT_CMEE        "AT+CMEE"
#define AT_NBAND       "AT+NBAND"
#define AT_ATI         "ATI"
#define AT_NCCID       "AT+NCCID"     //��ѯSIM����ID
#define AT_CSQ         "AT+CSQ"
#define AT_CFUN        "AT+CFUN"
#define AT_NCSEARFCN   "AT+NCSEARFCN"  //���Ƶ�� 

#define AT_CEREG       "AT+CEREG"      //ע������
#define AT_CIPCA       "AT+CIPCA"       //PDP���������� 
#define AT_CGATT       "AT+CGATT"
#define AT_NUESTATS    "AT+NUESTATS"
#define AT_CGPADDR     "AT+CGPADDR"
#define AT_NUESTATS    "AT+NUESTATS"
#define AT_CGPADDR     "AT+CGPADDR"
#define AT_NSMI        "AT+NSMI"    
#define AT_NNMI        "AT+NNMI" 

/**
  * @brief ��Ϣ��ѯ�������
  */
#define AT_CGMI        "AT+CGMI"        //��ȡ����ID����
#define AT_CGMM        "AT+CGMM"        //��ȡģ��ID����
#define AT_CGMR        "AT+CGMR"        //��ȡ����汾���� 
#define AT_CIMI        "AT+CIMI"        //��ȡ�����ƶ��û����ID���� 
#define AT_CGSN        "AT+CGSN"        //��ȡ��Ʒ���к�ID����
#define AT_NBAND       "AT+NBAND"       //��ȡƵ����Ϣ

/**
  * @brief �״̬����
  */
#define AT_CSCON       "AT+CSCON"      //��ȡ����״̬, Ϊ0����IDLE״̬ 

/**
  * @brief TCP/UDP�������
  */
#define AT_CSOC        "AT+CSOC"
#define AT_CSOSEND     "AT+CSOSEND"
#define AT_CSOCL       "AT+CSOCL" 


/**
  * @brief CoAP�������
  */
#define AT_CCOAPSTA    "AT+CCOAPSTA"   //����Զ�� CoAP ����
#define AT_CCOAPNEW    "AT+CCOAPNEW"   //�����ͻ���ʵ��
#define AT_CCOAPSEND   "AT+CCOAPSEND"  //����16��������
#define AT_CCOAPDEL    "AT+CCOAPDEL"   //�ͷŲ�ɾ���ͻ���ʵ��


/**
  * @brief ����(��Ϊ)iotƽ̨�������������
  */
#define AT_NCDP          "AT+NCDP"         //����Զ�˷�����
#define AT_QLWULDATA     "AT+QLWULDATA"    //����16��������
#define AT_QLWULDATAEX   "AT+QLWULDATAEX"  //����16��������,��ģʽ
#define AT_QLWSREGIND    "AT+QLWSREGIND"   //����/ɾ��iotƽ̨


/**
  * @brief �͹����������
  */
#define AT_CLAC        "AT+CLAC"
#define AT_CGPADDR     "AT+CGPADDR"
#define AT_CGDCONT     "AT+CGDCONT"
#define AT_CCLK        "AT+CCLK"
#define AT_CPSMS       "AT+CPSMS"
#define AT_CEDRXS      "AT+CEDRXS"
#define AT_CEER        "AT+CEER"
#define AT_CEDRXRDP    "AT+CEDRXRDP"
#define AT_CTZR        "AT+CTZR"

/**
  * @brief Ping����
  */    
#define  AT_CIPPING      "AT+CIPPING"     //Ping����


/**
  * @brief ATָ���һЩ���Բ���
  */
#define CMD_TRY_TIMES           10
#define CMD_READ_ARGUMENT       "?"
#define CMD_TEST_ARGUMENT       "=?"
#define CMD_OK_RES              "OK"


/**
  * @brief ����iotƽ̨������
  */
#if CTNB_IOT
#define REMOTE_SERVER_IP        "180.101.147.115"
#define REMOTE_COAP_PORT        "5683"
#define REMOTE_UDP_PORT         "6000"
#define REMOTE_TCP_PORT         "5683"

#define REMOTE_COAP_INFO        "180.101.147.115,5683"

/**
  * @brief ��Ϊiotƽ̨������
  */
#elif HUAWEI_IOT

#define REMOTE_SERVER_IP        "139.159.140.34"
#define REMOTE_COAP_PORT        "5683"
#define REMOTE_UDP_PORT         "6000"
#define REMOTE_TCP_PORT         "5683"

#define REMOTE_COAP_INFO        "139.159.140.34,5683"

/**
  * @brief easyiotƽ̨������
  */
#elif EASY_IOT

#define REMOTE_SERVER_IP        "117.60.157.137"
#define REMOTE_COAP_PORT        "5683"
#define REMOTE_UDP_PORT         "6000"
#define REMOTE_TCP_PORT         "5683"

#define REMOTE_COAP_INFO        "117.60.157.137,5683"

#else


#endif

/**
  * @brief ��������
  */
#define BAND_850MHZ_ID           5
#define BAND_850MHZ_STR          "850"

#define BAND_900MHZ_ID           8
#define BAND_900MHZ_STR          "900"

#define BAND_800MHZ_ID           20
#define BAND_800MHZ_STR          "800"

#define BAND_700MHZ_ID           28
#define BAND_700MHZ_STR          "700"



/**
  * @brief ATָ����Ӧ������������
  */
#define AT_CMD_RESPONSE_PAR_NUM_MAX   16


/**
 * @brief ATָ������ö��
 */
typedef enum
{
   CMD_TEST,         //����TEST����
   CMD_READ,         //����READ����
   CMD_SET,          //����SET����
   CMD_EXCUTE        //����EXCUTE����
}cmd_property_t;


/**
 * @brief ATָ�����Ϊö��
 */
typedef enum
{
    ACTION_OK_EXIT        = 0X01,              //����ִ�гɹ����˳� 
    ACTION_OK_AND_NEXT    = 0X02,              //����ִ�гɹ���ִ����һ��ָ��      
    ACTION_ERROR_BUT_NEXT = 0X04,              //����ִ�д��������������ִ����һ��ָ��
    ACTION_ERROR_AND_TRY  = 0X08,              //����ִ�д������г���
    ACTION_ERROR_EXIT     = 0X10,              //����ִ�д�����˳� 
    ACTION_OK_WAIT        = 0X20               //����ִ�гɹ��󽫵ȴ�        
}cmd_action_t;


/**
 * @brief ATָ��ṹ����
 */
typedef struct at_cmd_info
{
    const char*     p_atcmd;       // ATָ��
    cmd_property_t  property;      // ָ�ǰ����(TEST,READ,SET,EXCUTE)
    char*           p_atcmd_arg;   // ָ�����
    char*           p_expectres;   // �����õ��ظ�
    unsigned char   cmd_try;       // �����Դ���
    unsigned char   have_tried;    // �Ѿ����γ��ԵĴ���
    uint8_t         cmd_action;    // ATָ����Ϊ
    uint32_t        max_timeout;   // ���ʱʱ��
}at_cmd_info_t;


/**
 * @brief����AT cmd�ṹָ������
 */
typedef at_cmd_info_t *at_cmdhandle;


/**
 * @brief nbiotͨ�ô�����붨��
 */
#define   NBIOT_OK                  0
#define   NBIOT_ERROR              -1
#define   NBIOT_NOTSUPPORT         -2


/**
 * @brief ָ��ִ������
 */
#define   AT_CMD_OK                NBIOT_OK
#define   AT_CMD_NEXT              1
#define   AT_CMD_WAIT              2


/**
 * @brief ָ��ִ�н��
 */
#define   AT_CMD_RESULT_OK            NBIOT_OK
#define   AT_CMD_RESULT_ERROR           -3
#define   AT_CMD_RESULT_CONTINUE        -4
#define   AT_CMD_RESULT_RANDOM_CODE     -5

/**
 * @brief ָ�������
 */
#define   NBIOT_ERROR_TIMEOUT           -6
#define   NBIOT_ERROR_RETRY             -7
#define   NBIOT_ERROR_NEXT              -8
#define   NBIOT_ERROR_CONTINUE          -9


/**
 * @brief nbiot ATָ�ǰ��״̬����
 */
typedef enum nbiot_main_status
{
    NBIOT_NONE= 0,
    NBIOT_INIT,         // ��ʼ������
    NBIOT_INFO,                     // ��ȡ NB ģ�鳧�̼��̼���Ƶ�ε���Ϣ
    NBIOT_SIGNAL,       // ��ȡ�ź�����
    NBIOT_CSCON,        // ��ȡģ��Ļ״̬
    NBIOT_TCPUDP_CR,    // ���� TCP/UDP
    NBIOT_TCPUDP_CL,    // �ر� TCP/UDP
    NBIOT_TCPUDP_SEND,  // �����Ѿ�������TCP/UDP��������
    NBIOT_TCPUDP_RECV,  // TCP/UDP������Ϣ 
    NBIOT_SOCKET_ERR,   // SOCKET���ӷ�������
    NBIOT_CoAP_SEVER,   // CoAPԶ�̷���������
    NBIOT_CoAP_CLIENT,  // CoAP�ͻ������ӵ�ַ����
    NBIOT_CoAP_SEND,    // CoAP������Ϣ
    NBIOT_CoAP_RECV,    // CoAP������Ϣ
    NBIOT_CoAP_CL,      // �ر�CoAP
    NBIOT_NCDP_SERVER,  // NCDP�ͻ������ӵ�ַ����
    NBIOT_NCDP_SEND,    // NCDP������Ϣ
    NBIOT_NCDP_RECV,    // NCDP������Ϣ
    NBIOT_NCDP_STATUS,  // NCDP����״̬��Ϣ
    NBIOT_NCDP_CL,      // �ر�NCDP 
    NBIOT_CSCON_STATUS, // ���ӣ����״̬
    NBIOT_PSM,          // PSM
    NBIOT_EDRX,         // eDRX 
    NBIOT_SLEEP,        // SLEEP      
    NBIOT_RESET,        // ��λNB
    NBIOT_NCONFIG,      // �Զ���������
    NBIOT_END    
}nbiot_main_status_t;


/**
 * @brief nbiot ATָ�ǰ��״̬����
 */
typedef enum nbiot_sub_status
{
    NBIOT_SUB_NONE,
    NBIOT_SUB_SYNC,
    NBIOT_SUB_QLED,  

    NBIOT_SUB_CMEE, 
    NBIOT_SUB_CFUN_DISABLE,  
    NBIOT_SUB_NCSEARFCN,  
    NBIOT_SUB_BAND, 
    NBIOT_SUB_QREGSWT,     
    NBIOT_SUB_CFUN,
    NBIOT_SUB_CSCON,
    NBIOT_SUB_CGATT,  
    NBIOT_SUB_CEREG,   
    NBIOT_SUB_ATI,
    NBIOT_SUB_CPIN,
    NBIOT_SUB_CSQ,
    NBIOT_SUB_CIPCA_QUERY,    
    NBIOT_SUB_NUESTATS,    
    NBIOT_SUB_CGPADDR,
    NBIOT_SUB_NNMI,
//    NBIOT_SUB_NSMI,    
    NBIOT_SUB_CGATT_QUERY,  
    NBIOT_SUB_CEREG_QUERY,
  
    NBIOT_SUB_CGMI,
    NBIOT_SUB_CGMM,
    NBIOT_SUB_CGMR,
    NBIOT_SUB_CIMI,
    NBIOT_SUB_CGSN,  
    NBIOT_SUB_NBAND,
    
    NBIOT_SUB_CSCON_QUERY,    

    NBIOT_SUB_TCPUDP_CR,
    NBIOT_SUB_TCPUDP_CONNECT, 
    NBIOT_SUB_TCPUDP_SEND,
    NBIOT_SUB_TCPUDP_RECV,  // TCP/UDP������Ϣ 
    NBIOT_SUB_SOCKET_ERR,   // SOCKET���ӷ�������    
    NBIOT_SUB_TCPUDP_CL,
    
    NBIOT_SUB_CoAP_SEVER,   // CoAPԶ�̷���������
    NBIOT_SUB_CoAP_CLIENT,  // CoAP�ͻ������ӵ�ַ����
    NBIOT_SUB_CoAP_SEND,    // ������Ϣ
    NBIOT_SUB_CoAP_RECV,    // CoAP������Ϣ
    NBIOT_SUB_CoAP_CL,      // �ر�CoAP  

    NBIOT_SUB_NCDP_SERVER,  // NCDP��������ַ����
    NBIOT_SUB_NCDP_CONNECT, // ����IoTƽ̨����
    NBIOT_SUB_NCDP_SEND,    // NCDP������Ϣ
    NBIOT_SUB_NCDP_RECV,    // NCDP������Ϣ
    NBIOT_SUB_NCDP_STATUS,  // NCDP����״̬��Ϣ
    NBIOT_SUB_NCDP_CL,      // �ر�NCDP 
    NBIOT_SUB_CSCON_STATUS, // ���ӣ����״̬
    NBIOT_SUB_PSM,          // PSM
    NBIOT_SUB_EDRX,         // eDRX 
    NBIOT_SUB_SLEEP,        // SLEEP 
    NBIOT_SUB_RESET,        // ��λNB
    NBIOT_SUB_NCONFIG,      // �Զ���������
    NBIOT_SUB_END       
}nbiot_sub_status_t;


/**
 * @brief nbiot״̬��Ϣ, ����״̬��
 */
typedef struct nbiot_status_sm
{
    int   main_status;         //�����������׶�
    int   sub_status;          //���������ӽ׶Σ�����״̬Ƕ��     
}nbiot_status_sm_t;


/**
 * @brief nbiot ����״̬��Ϣ,����ָʾʹ������Э���������
 */
typedef struct nbiot_status_connect {
    uint8_t                connect_status;      //���ӵ�״̬
    uint8_t                connect_type;        //���ӵ�����
    int8_t                 connect_id;          //���ӵ�id
    int8_t                 m2m_status;          //m2m����״̬
    int8_t                 cscon_status;        //cscon_status����״̬    
    int8_t                 rssi;                //�źŵ�����
    int8_t                 cid;                 //���ӵ�cid
    uint8_t                register_status;     //����ע��
    uint16_t               data_len;            //��ʾ���ݳ���  
    char                  *data_offest;         //���ݻ�������ʼ��ַ����ƫ�Ƶ�ַ  
}nbiot_status_connect_t;

typedef struct nbiot_socket_info {
    uint8_t                socket_type;         //ָʾsocket_type������
    int8_t                 socket_id;           //ָʾ��Ӧ��socket id
    int8_t                 socket_errcode;      //ָʾ��Ӧ��socket������ 
    char                  *data_offest;         //���ݻ�������ʼ��ַ����ƫ�Ƶ�ַ
    uint16_t               data_len;            //��ʾ���ݳ���   
}nbiot_socket_info_t;

/**
 * @brief ���ӵ�Э������
 */
typedef enum nbiot_connect_type {
   NBIOT_NOCON,
   NBIOT_TCP  = 1,         
   NBIOT_UDP,         
   NBIOT_HTTP,        
   NBIOT_COAP,        
   NBIOT_MQTT,
   NBIOT_NCDP, 
}nbiot_connect_type_t;


/**
 * @brief NBIOT�̼���Ϣ
 */
typedef struct nbiot_firmware_info
{   char         name[32];
    uint8_t      IMSI[16];
    uint8_t      IMEI[16];      
}nbiot_firmware_info_t;


/**
 * @brief �͹�����Ϣ
 */
typedef enum nbiot_lpower_type {
   PSM  = 1,
   EDRX,         
   SLEEP, 
}nbiot_lpower_type_t;

/**
 * @brief NBIOT�͹�����Ϣ�ṹ��
 */
typedef struct nbiot_lpower_info_t
{   nbiot_lpower_type_t lpower_type;
    char time[4]; 
}nbiot_lpower_info_t;

/**
 * @brief �շ����ݵ���󳤶�
 */
#define NBIOT_RECV_BUF_MAX_LEN    (RING_BUF_LEN + 1)
#define NBIOT_SEND_BUF_MAX_LEN    (RING_BUF_LEN + 1)


/**
 * @brief ���ջ���ṹ�嶨��
 */
typedef struct nbiot_recv
{
    char      buf[NBIOT_RECV_BUF_MAX_LEN];    //�������ݻ�����
    uint16_t  len;                              //��Ч���ݳ���
}nbiot_recv_t;

/**
 * @brief ���ͻ���ṹ�嶨��
 */
typedef struct nbiot_send
{
    char      buf[NBIOT_SEND_BUF_MAX_LEN];    //�������ݻ�����
    uint16_t  len;                             //��Ч���ݳ���
    
}nbiot_send_t;

/**
 * @brief nbiotģ�����������ṹ�嶨��
 */
struct nbiot_drv_funcs {
    
    //nbiot��������
    int (*nbiot_send_data) (void *p_arg, uint8_t *pData, uint16_t size, uint32_t Timeout);
    
    //nbiot��������
    int (*nbiot_recv_data) (void *p_arg, uint8_t *pData, uint16_t size, uint32_t Timeout);    
};


/**
 * @brief nbiotģ���¼�����
 */
#define    NBIOT_NONE_EVENT          0x0000           //û���¼�����
#define    NBIOT_RECV_EVENT          0x0001           //�յ���Ӧ�����¼�(����ͺ�ģ���Ӧ�¼���
#define    NBIOT_TIMEOUT_EVENT       0x0002           //�����ʱ���������ʱ�¼����ͺ�ò�����Ӧ��������ʱ�¼�
#define    NBIOT_REG_STA_EVENT       0x0004           //NBIOT���總���¼�
#define    NBIOT_TCP_RECV_EVENT      0x0008           //TCP�����¼�
#define    NBIOT_UDP_RECV_EVENT      0x0010           //UDP�����¼�
#define    NBIOT_COAP_RECV_EVENT     0X0020           //COAP�����¼�
#define    NBIOT_MQTT_RECV_EVENT     0X0040           //MQTT�����¼�
#define    NBIOT_LWM2M_RECV_EVENT    0X0080           //LWM2M�����¼�
#define    NBIOT_SOCKET_ERR_EVENT    0x0100           //SOCKET���������¼�
#define    NBIOT_NCDP_RECV_EVENT     0X0200           //NCDP�����¼�
#define    NBIOT_NCDP_STATUS_EVENT   0X0400           //NCDP״̬�¼�
#define    NBIOT_CSON_STATUS_EVENT   0X0800           //���ӣ����״̬�¼�
#define    NBIOT_ENTER_LPOWER_EVENT  0X1000           //����͹����¼�
#define    NBIOT_EXIT_LPOWER_EVENT   0X2000           //�˳��͹����¼�
#define    NBIOT_REBOOT_EVENT        0X4000           //�����¼�
#define    NBIOT_OTHER_EVENT         0X8000           //�����¼�
                                    
/**
 * @brief ����NBIOT�¼��ص�����ָ��
 */
typedef void (*nbiot_cb)(void *p_arg, int, int ,char*);


/**
 * @brief nbiot�豸�ṹ��
 */
typedef struct nbiot_dev_info
{ 
    GPIO_TypeDef  *GPIO_VEN;     
    int ven_pin;  
  
    GPIO_TypeDef  *GPIO_RST;   
    int rst_pin; 
  
}nbiot_dev_info_t;


/**
 * @brief nbiot�豸�ṹ��
 */
typedef struct nbiot_dev
{     
    struct nbiot_drv_funcs   *p_drv_funcs;
  
    //�豸��Ϣ�ṹ��
    struct nbiot_dev_info    *p_dev_info;  
    
    //�����Ĵ����豸
    uart_dev_t               *p_uart_dev;
    
    //nbiot�豸�¼��ص�����
    nbiot_cb                  nbiot_cb;  
    
    //nbiot�豸�¼��ص���������
    void                     *p_arg;

    //�¼����
    volatile int              nbiot_event;
    
    //nbiotָ��ִ��״̬��Ϣ
    at_cmd_info_t            *p_nbiot_cmd; 

    //nbiotָ��ִ��״̬��Ϣ
    nbiot_socket_info_t      *p_socket_info;    

    //nbiot sm״̬��Ϣ
    nbiot_status_sm_t        *p_sm_status;

    //nbiot ����״̬��Ϣ
    nbiot_status_connect_t   *p_connect_status;
  
    //nbiot�̼���Ϣ
    nbiot_firmware_info_t   *p_firmware_info; 

    //֡��ʽ����ʱ��֡����ILDE��֡
    uint8_t                   frame_format;  
               
}nbiot_dev_t;


/**
 * @brief nbiotģ���豸���
 */
typedef nbiot_dev_t *nbiot_handle_t;


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
                              uint32_t at_cmd_time_out);
                              
/**
  * @brief  ��nbiotģ�鷢��ATָ��.
  * @param  nbiot_handle  : ָ��nbiot�豸�����ָ��.
  * @param  cmd_handle    : ��Ҫ����ָ����Ϣ���
  * @note   ���øú���ǰ��                              
  * @retval ���� 0 �����ͳɹ�
  */
int nbiot_at_cmd_send(nbiot_handle_t nbiot_handle, at_cmdhandle cmd_handle);                              


/**
  * @brief ����nbiot�¼�  
  * @param  nbiot_handle  : ָ��nbiot�豸�����ָ��.
  * @param  nbiot_event   : �¼�����.
  * @retval None  
  */
void nbiot_event_set (nbiot_handle_t nbiot_handle, int nbiot_event);

/**
  * @brief  �жϵ�ǰnbiot�¼��Ƿ���  
  * @param  nbiot_handle  : ָ��nbiot�豸�����ָ��.
  * @param  nbiot_event   : �¼�����.
  * @retval ��0����ǰ�¼��Ѿ�����  
  */
int nbiot_event_get (nbiot_handle_t nbiot_handle, int nbiot_event);


/**
  * @brief ���nbiot�¼�  
  * @param  nbiot_handle  : ָ��nbiot�豸�����ָ��.
  * @param  nbiot_event   : �¼�����.
  * @retval None  
  */
void nbiot_event_clr (nbiot_handle_t nbiot_handle, int nbiot_event);

/**
  * @brief  nbiotģ���豸ʵ����ʼ�� .
  * @param  nbiot_handle  : ָ��nbiot�豸�����ָ��.
  * @param  cmd_handle    : ��Ҫ����ָ����Ϣ���
  * @retval ���� nbiotģ���豸�����ָ�� 
  */
nbiot_handle_t nbiot_dev_init(uart_handle_t nbiot_handle);

/**
  * @brief  ע��nbiotģ���¼��ص�����.
  * @param  cb     : ģ���豸�ص�.
  * @param  p_arg  : ģ���豸�ص���������
  * @retval ���� nbiotģ���豸�����ָ�� 
  */
void nbiot_event_registercb (nbiot_handle_t nbiot_handle, nbiot_cb cb, void *p_arg);

/**
  * @brief nbiot�¼�������
  * @param  nbiot_handle  : ָ��nbiot�豸�����ָ��.
  * @retval NBIOT_OK �ɹ�  
  */
int nbiot_event_poll (nbiot_handle_t nbiot_handle);
                                                                                                    
#endif /* ATK_BC28_H */
