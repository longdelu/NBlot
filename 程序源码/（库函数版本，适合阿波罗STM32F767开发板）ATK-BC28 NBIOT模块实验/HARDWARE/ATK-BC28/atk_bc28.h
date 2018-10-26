#ifndef _ATK_BC28_H
#define _ATK_BC28_H
#include "atk_sys.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "atk_nbiot_usart.h"
//////////////////////////////////////////////////////////////////////////////////    
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F429开发板
//串口1初始化         
//正点原子@ALIENTEK
//技术论坛:www.openedv.csom
//修改日期:2015/6/23
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved
//********************************************************************************
//V1.0 第一次应用 
//////////////////////////////////////////////////////////////////////////////////

 /**
  * @brief 使用对应iot平台的宏开关
  */
#define CTNB_IOT                    1    //电信iot平台
//#define HUAWEI_IOT                  1    //华为iot平台
//#define EASY_IOT                    1    //Easyiot平台

/**
  * @brief 初始化及网络注册相关命令
  */
#define AT_NCONFIG     "AT+NCONFIG"    //使能/禁能自动入网命令
#define AT_QLEDMODE    "AT+QLEDMODE"   //使能/禁能LED
#define AT_QREGSWT     "AT+QREGSWT"    //IOT平台注册模式选择
#define AT_NRB         "AT+NRB"        //重启命令 
#define AT_SYNC        "AT"
#define AT_QLEDMODE    "AT+QLEDMODE"
#define AT_CMEE        "AT+CMEE"
#define AT_NBAND       "AT+NBAND"
#define AT_ATI         "ATI"
#define AT_NCCID       "AT+NCCID"     //查询SIM卡的ID
#define AT_CSQ         "AT+CSQ"
#define AT_CFUN        "AT+CFUN"
#define AT_NCSEARFCN   "AT+NCSEARFCN"  //清除频点 

#define AT_CEREG       "AT+CEREG"      //注册网络
#define AT_CIPCA       "AT+CIPCA"       //PDP上下文设置 
#define AT_CGATT       "AT+CGATT"
#define AT_NUESTATS    "AT+NUESTATS"
#define AT_CGPADDR     "AT+CGPADDR"
#define AT_NUESTATS    "AT+NUESTATS"
#define AT_CGPADDR     "AT+CGPADDR"
#define AT_NSMI        "AT+NSMI"    
#define AT_NNMI        "AT+NNMI" 

/**
  * @brief 信息查询相关命令
  */
#define AT_CGMI        "AT+CGMI"        //获取产商ID命令
#define AT_CGMM        "AT+CGMM"        //获取模型ID命令
#define AT_CGMR        "AT+CGMR"        //获取软件版本命令 
#define AT_CIMI        "AT+CIMI"        //获取国际移动用户身份ID命令 
#define AT_CGSN        "AT+CGSN"        //获取产品序列号ID命令
#define AT_NBAND       "AT+NBAND"       //获取频率信息

/**
  * @brief 活动状态命令
  */
#define AT_CSCON       "AT+CSCON"      //获取连接状态, 为0则是IDLE状态 

/**
  * @brief TCP/UDP相关命令
  */
#define AT_CSOC        "AT+CSOC"
#define AT_CSOSEND     "AT+CSOSEND"
#define AT_CSOCL       "AT+CSOCL" 


/**
  * @brief CoAP相关命令
  */
#define AT_CCOAPSTA    "AT+CCOAPSTA"   //连接远端 CoAP 服务
#define AT_CCOAPNEW    "AT+CCOAPNEW"   //创建客户端实例
#define AT_CCOAPSEND   "AT+CCOAPSEND"  //发送16进制数据
#define AT_CCOAPDEL    "AT+CCOAPDEL"   //释放并删除客户端实例


/**
  * @brief 电信(华为)iot平台连接相关命令令
  */
#define AT_NCDP          "AT+NCDP"         //更新远端服务器
#define AT_QLWULDATA     "AT+QLWULDATA"    //发送16进制数据
#define AT_QLWULDATAEX   "AT+QLWULDATAEX"  //发送16进制数据,待模式
#define AT_QLWSREGIND    "AT+QLWSREGIND"   //连接/删除iot平台


/**
  * @brief 低功耗相关命令
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
  * @brief Ping命令
  */    
#define  AT_CIPPING      "AT+CIPPING"     //Ping命令


/**
  * @brief AT指令的一些属性参数
  */
#define CMD_TRY_TIMES           10
#define CMD_READ_ARGUMENT       "?"
#define CMD_TEST_ARGUMENT       "=?"
#define CMD_OK_RES              "OK"


/**
  * @brief 电信iot平台服务器
  */
#if CTNB_IOT
#define REMOTE_SERVER_IP        "180.101.147.115"
#define REMOTE_COAP_PORT        "5683"
#define REMOTE_UDP_PORT         "6000"
#define REMOTE_TCP_PORT         "5683"

#define REMOTE_COAP_INFO        "180.101.147.115,5683"

/**
  * @brief 华为iot平台服务器
  */
#elif HUAWEI_IOT

#define REMOTE_SERVER_IP        "139.159.140.34"
#define REMOTE_COAP_PORT        "5683"
#define REMOTE_UDP_PORT         "6000"
#define REMOTE_TCP_PORT         "5683"

#define REMOTE_COAP_INFO        "139.159.140.34,5683"

/**
  * @brief easyiot平台服务器
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
  * @brief 工作带宽
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
  * @brief AT指令响应的最大参数个数
  */
#define AT_CMD_RESPONSE_PAR_NUM_MAX   16


/**
 * @brief AT指令属性枚举
 */
typedef enum
{
   CMD_TEST,         //命令TEST操作
   CMD_READ,         //命令READ操作
   CMD_SET,          //命令SET操作
   CMD_EXCUTE        //命令EXCUTE操作
}cmd_property_t;


/**
 * @brief AT指令动作行为枚举
 */
typedef enum
{
    ACTION_OK_EXIT        = 0X01,              //命令执行成功后将退出 
    ACTION_OK_AND_NEXT    = 0X02,              //命令执行成功后将执行下一条指令      
    ACTION_ERROR_BUT_NEXT = 0X04,              //命令执行错误后跳过该命令执行下一条指令
    ACTION_ERROR_AND_TRY  = 0X08,              //命令执行错误后进行尝试
    ACTION_ERROR_EXIT     = 0X10,              //命令执行错误后将退出 
    ACTION_OK_WAIT        = 0X20               //命令执行成功后将等待        
}cmd_action_t;


/**
 * @brief AT指令结构类型
 */
typedef struct at_cmd_info
{
    const char*     p_atcmd;       // AT指令
    cmd_property_t  property;      // 指令当前属性(TEST,READ,SET,EXCUTE)
    char*           p_atcmd_arg;   // 指令参数
    char*           p_expectres;   // 期望得到回复
    unsigned char   cmd_try;       // 出错尝试次数
    unsigned char   have_tried;    // 已经出次尝试的次数
    uint8_t         cmd_action;    // AT指令行为
    uint32_t        max_timeout;   // 最大超时时间
}at_cmd_info_t;


/**
 * @brief声明AT cmd结构指针类型
 */
typedef at_cmd_info_t *at_cmdhandle;


/**
 * @brief nbiot通用错误代码定义
 */
#define   NBIOT_OK                  0
#define   NBIOT_ERROR              -1
#define   NBIOT_NOTSUPPORT         -2


/**
 * @brief 指令执行流程
 */
#define   AT_CMD_OK                NBIOT_OK
#define   AT_CMD_NEXT              1
#define   AT_CMD_WAIT              2


/**
 * @brief 指令执行结果
 */
#define   AT_CMD_RESULT_OK            NBIOT_OK
#define   AT_CMD_RESULT_ERROR           -3
#define   AT_CMD_RESULT_CONTINUE        -4
#define   AT_CMD_RESULT_RANDOM_CODE     -5

/**
 * @brief 指令出错处理
 */
#define   NBIOT_ERROR_TIMEOUT           -6
#define   NBIOT_ERROR_RETRY             -7
#define   NBIOT_ERROR_NEXT              -8
#define   NBIOT_ERROR_CONTINUE          -9


/**
 * @brief nbiot AT指令当前主状态定义
 */
typedef enum nbiot_main_status
{
    NBIOT_NONE= 0,
    NBIOT_INIT,         // 初始化操作
    NBIOT_INFO,                     // 获取 NB 模块厂商及固件，频段等信息
    NBIOT_SIGNAL,       // 获取信号质量
    NBIOT_CSCON,        // 获取模块的活动状态
    NBIOT_TCPUDP_CR,    // 创建 TCP/UDP
    NBIOT_TCPUDP_CL,    // 关闭 TCP/UDP
    NBIOT_TCPUDP_SEND,  // 利用已经创建的TCP/UDP发送数据
    NBIOT_TCPUDP_RECV,  // TCP/UDP接收信息 
    NBIOT_SOCKET_ERR,   // SOCKET连接发生错误
    NBIOT_CoAP_SEVER,   // CoAP远程服务器设置
    NBIOT_CoAP_CLIENT,  // CoAP客户端连接地址设置
    NBIOT_CoAP_SEND,    // CoAP发送消息
    NBIOT_CoAP_RECV,    // CoAP返回信息
    NBIOT_CoAP_CL,      // 关闭CoAP
    NBIOT_NCDP_SERVER,  // NCDP客户端连接地址设置
    NBIOT_NCDP_SEND,    // NCDP发送消息
    NBIOT_NCDP_RECV,    // NCDP返回信息
    NBIOT_NCDP_STATUS,  // NCDP返回状态消息
    NBIOT_NCDP_CL,      // 关闭NCDP 
    NBIOT_CSCON_STATUS, // 连接（活动）状态
    NBIOT_PSM,          // PSM
    NBIOT_EDRX,         // eDRX 
    NBIOT_SLEEP,        // SLEEP      
    NBIOT_RESET,        // 复位NB
    NBIOT_NCONFIG,      // 自动入网设置
    NBIOT_END    
}nbiot_main_status_t;


/**
 * @brief nbiot AT指令当前子状态定义
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
    NBIOT_SUB_TCPUDP_RECV,  // TCP/UDP接收信息 
    NBIOT_SUB_SOCKET_ERR,   // SOCKET连接发生错误    
    NBIOT_SUB_TCPUDP_CL,
    
    NBIOT_SUB_CoAP_SEVER,   // CoAP远程服务器设置
    NBIOT_SUB_CoAP_CLIENT,  // CoAP客户端连接地址设置
    NBIOT_SUB_CoAP_SEND,    // 发送消息
    NBIOT_SUB_CoAP_RECV,    // CoAP返回信息
    NBIOT_SUB_CoAP_CL,      // 关闭CoAP  

    NBIOT_SUB_NCDP_SERVER,  // NCDP服务器地址设置
    NBIOT_SUB_NCDP_CONNECT, // 连接IoT平台设置
    NBIOT_SUB_NCDP_SEND,    // NCDP发送消息
    NBIOT_SUB_NCDP_RECV,    // NCDP返回信息
    NBIOT_SUB_NCDP_STATUS,  // NCDP返回状态消息
    NBIOT_SUB_NCDP_CL,      // 关闭NCDP 
    NBIOT_SUB_CSCON_STATUS, // 连接（活动）状态
    NBIOT_SUB_PSM,          // PSM
    NBIOT_SUB_EDRX,         // eDRX 
    NBIOT_SUB_SLEEP,        // SLEEP 
    NBIOT_SUB_RESET,        // 复位NB
    NBIOT_SUB_NCONFIG,      // 自动入网设置
    NBIOT_SUB_END       
}nbiot_sub_status_t;


/**
 * @brief nbiot状态信息, 用于状态机
 */
typedef struct nbiot_status_sm
{
    int   main_status;         //命令运行主阶段
    int   sub_status;          //命令运行子阶段，用于状态嵌套     
}nbiot_status_sm_t;


/**
 * @brief nbiot 连接状态信息,用于指示使用哪种协议进行连接
 */
typedef struct nbiot_status_connect {
    uint8_t                connect_status;      //连接的状态
    uint8_t                connect_type;        //连接的类型
    int8_t                 connect_id;          //连接的id
    int8_t                 m2m_status;          //m2m连接状态
    int8_t                 cscon_status;        //cscon_status连接状态    
    int8_t                 rssi;                //信号的质量
    int8_t                 cid;                 //连接的cid
    uint8_t                register_status;     //网络注册
    uint16_t               data_len;            //提示数据长度  
    char                  *data_offest;         //数据缓冲区起始地址所在偏移地址  
}nbiot_status_connect_t;

typedef struct nbiot_socket_info {
    uint8_t                socket_type;         //指示socket_type的类型
    int8_t                 socket_id;           //指示相应的socket id
    int8_t                 socket_errcode;      //指示相应的socket错误码 
    char                  *data_offest;         //数据缓冲区起始地址所在偏移地址
    uint16_t               data_len;            //提示数据长度   
}nbiot_socket_info_t;

/**
 * @brief 连接的协议类型
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
 * @brief NBIOT固件信息
 */
typedef struct nbiot_firmware_info
{   char         name[32];
    uint8_t      IMSI[16];
    uint8_t      IMEI[16];      
}nbiot_firmware_info_t;


/**
 * @brief 低功耗信息
 */
typedef enum nbiot_lpower_type {
   PSM  = 1,
   EDRX,         
   SLEEP, 
}nbiot_lpower_type_t;

/**
 * @brief NBIOT低功耗信息结构体
 */
typedef struct nbiot_lpower_info_t
{   nbiot_lpower_type_t lpower_type;
    char time[4]; 
}nbiot_lpower_info_t;

/**
 * @brief 收发数据的最大长度
 */
#define NBIOT_RECV_BUF_MAX_LEN    (RING_BUF_LEN + 1)
#define NBIOT_SEND_BUF_MAX_LEN    (RING_BUF_LEN + 1)


/**
 * @brief 接收缓存结构体定义
 */
typedef struct nbiot_recv
{
    char      buf[NBIOT_RECV_BUF_MAX_LEN];    //接收数据缓冲区
    uint16_t  len;                              //有效数据长度
}nbiot_recv_t;

/**
 * @brief 发送缓存结构体定义
 */
typedef struct nbiot_send
{
    char      buf[NBIOT_SEND_BUF_MAX_LEN];    //发送数据缓冲区
    uint16_t  len;                             //有效数据长度
    
}nbiot_send_t;

/**
 * @brief nbiot模块驱动函数结构体定义
 */
struct nbiot_drv_funcs {
    
    //nbiot发送数据
    int (*nbiot_send_data) (void *p_arg, uint8_t *pData, uint16_t size, uint32_t Timeout);
    
    //nbiot接收数据
    int (*nbiot_recv_data) (void *p_arg, uint8_t *pData, uint16_t size, uint32_t Timeout);    
};


/**
 * @brief nbiot模块事件类型
 */
#define    NBIOT_NONE_EVENT          0x0000           //没有事件发生
#define    NBIOT_RECV_EVENT          0x0001           //收到回应数据事件(命令发送后模组回应事件）
#define    NBIOT_TIMEOUT_EVENT       0x0002           //命令发送时出错产生超时事件或发送后得不到回应，产生超时事件
#define    NBIOT_REG_STA_EVENT       0x0004           //NBIOT网络附着事件
#define    NBIOT_TCP_RECV_EVENT      0x0008           //TCP接收事件
#define    NBIOT_UDP_RECV_EVENT      0x0010           //UDP接收事件
#define    NBIOT_COAP_RECV_EVENT     0X0020           //COAP接收事件
#define    NBIOT_MQTT_RECV_EVENT     0X0040           //MQTT接收事件
#define    NBIOT_LWM2M_RECV_EVENT    0X0080           //LWM2M接收事件
#define    NBIOT_SOCKET_ERR_EVENT    0x0100           //SOCKET发生错误事件
#define    NBIOT_NCDP_RECV_EVENT     0X0200           //NCDP接收事件
#define    NBIOT_NCDP_STATUS_EVENT   0X0400           //NCDP状态事件
#define    NBIOT_CSON_STATUS_EVENT   0X0800           //连接（活动）状态事件
#define    NBIOT_ENTER_LPOWER_EVENT  0X1000           //进入低功耗事件
#define    NBIOT_EXIT_LPOWER_EVENT   0X2000           //退出低功耗事件
#define    NBIOT_REBOOT_EVENT        0X4000           //重启事件
#define    NBIOT_OTHER_EVENT         0X8000           //其它事件
                                    
/**
 * @brief 定义NBIOT事件回调函数指针
 */
typedef void (*nbiot_cb)(void *p_arg, int, int ,char*);


/**
 * @brief nbiot设备结构体
 */
typedef struct nbiot_dev_info
{ 
    GPIO_TypeDef  *GPIO_VEN;     
    int ven_pin;  
  
    GPIO_TypeDef  *GPIO_RST;   
    int rst_pin; 
  
}nbiot_dev_info_t;


/**
 * @brief nbiot设备结构体
 */
typedef struct nbiot_dev
{     
    struct nbiot_drv_funcs   *p_drv_funcs;
  
    //设备信息结构体
    struct nbiot_dev_info    *p_dev_info;  
    
    //关联的串口设备
    uart_dev_t               *p_uart_dev;
    
    //nbiot设备事件回调函数
    nbiot_cb                  nbiot_cb;  
    
    //nbiot设备事件回调函数参数
    void                     *p_arg;

    //事件标记
    volatile int              nbiot_event;
    
    //nbiot指令执行状态信息
    at_cmd_info_t            *p_nbiot_cmd; 

    //nbiot指令执行状态信息
    nbiot_socket_info_t      *p_socket_info;    

    //nbiot sm状态信息
    nbiot_status_sm_t        *p_sm_status;

    //nbiot 连接状态信息
    nbiot_status_connect_t   *p_connect_status;
  
    //nbiot固件信息
    nbiot_firmware_info_t   *p_firmware_info; 

    //帧格式，超时成帧或者ILDE成帧
    uint8_t                   frame_format;  
               
}nbiot_dev_t;


/**
 * @brief nbiot模块设备句柄
 */
typedef nbiot_dev_t *nbiot_handle_t;


/**
  * @brief nbiot at指令初始化
  * @param  at_cmd           : 具体AT指令
  * @param  argument         : 指令对应的参数
  * @param  property         : 指令参数
  * @param  at_cmd_time_out  : 指令的超时时间
  * @retval None
  */
void nbiot_at_cmd_param_init (at_cmdhandle cmd_handle,
                              const char *at_cmd,
                              char *argument,
                              cmd_property_t property,
                              uint32_t at_cmd_time_out);
                              
/**
  * @brief  给nbiot模块发送AT指令.
  * @param  nbiot_handle  : 指向nbiot设备句柄的指针.
  * @param  cmd_handle    : 将要发送指令信息句柄
  * @note   调用该函数前先                              
  * @retval 返回 0 代表发送成功
  */
int nbiot_at_cmd_send(nbiot_handle_t nbiot_handle, at_cmdhandle cmd_handle);                              


/**
  * @brief 设置nbiot事件  
  * @param  nbiot_handle  : 指向nbiot设备句柄的指针.
  * @param  nbiot_event   : 事件类型.
  * @retval None  
  */
void nbiot_event_set (nbiot_handle_t nbiot_handle, int nbiot_event);

/**
  * @brief  判断当前nbiot事件是否发生  
  * @param  nbiot_handle  : 指向nbiot设备句柄的指针.
  * @param  nbiot_event   : 事件类型.
  * @retval 非0代表当前事件已经发生  
  */
int nbiot_event_get (nbiot_handle_t nbiot_handle, int nbiot_event);


/**
  * @brief 清除nbiot事件  
  * @param  nbiot_handle  : 指向nbiot设备句柄的指针.
  * @param  nbiot_event   : 事件类型.
  * @retval None  
  */
void nbiot_event_clr (nbiot_handle_t nbiot_handle, int nbiot_event);

/**
  * @brief  nbiot模块设备实例初始化 .
  * @param  nbiot_handle  : 指向nbiot设备句柄的指针.
  * @param  cmd_handle    : 将要发送指令信息句柄
  * @retval 返回 nbiot模块设备句柄的指针 
  */
nbiot_handle_t nbiot_dev_init(uart_handle_t nbiot_handle);

/**
  * @brief  注册nbiot模块事件回调函数.
  * @param  cb     : 模块设备回调.
  * @param  p_arg  : 模块设备回调函数参数
  * @retval 返回 nbiot模块设备句柄的指针 
  */
void nbiot_event_registercb (nbiot_handle_t nbiot_handle, nbiot_cb cb, void *p_arg);

/**
  * @brief nbiot事件处理函数
  * @param  nbiot_handle  : 指向nbiot设备句柄的指针.
  * @retval NBIOT_OK 成功  
  */
int nbiot_event_poll (nbiot_handle_t nbiot_handle);
                                                                                                    
#endif /* ATK_BC28_H */
