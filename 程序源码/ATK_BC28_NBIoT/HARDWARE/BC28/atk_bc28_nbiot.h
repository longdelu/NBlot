#ifndef _NBIOT_H
#define _NBIOT_H

#include "atk_bc28.h"
#include "atk_delay.h"

////////////////////////////////////////////////////////////////////////////////// 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F429开发板
//串口1初始化           
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2018/7/11
//版本：V1.5
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//********************************************************************************

     
     
//nbiot app状态定义
typedef enum nbiot_app_status
{
    NBIOT_APP_NONE,        
    NBIOT_APP_INIT        = NBIOT_INIT,         // 初始化操作
    NBIOT_APP_INFO        = NBIOT_INFO,         // 获取 NB 模块厂商及固件，频段等信息
    NBIOT_APP_SIGNAL      = NBIOT_SIGNAL,       // 获取信号质量
    NBIOT_APP_TCPUDP_CR   = NBIOT_TCPUDP_CR,    // 创建 TCP/UDP
    NBIOT_APP_TCPUDP_CL   = NBIOT_TCPUDP_CL,    // 关闭 TCP/UDP
    NBIOT_APP_TCPUDP_SEND = NBIOT_TCPUDP_SEND,  // 利用已经创建的TCP/UDP发送数据
    NBIOT_APP_TCPUDP_RECV = NBIOT_TCPUDP_RECV,  // TCP/UDP接收信息 
    NBIOT_APP_SOCKET_ERR  = NBIOT_SOCKET_ERR,   // SOCKET连接发生错误
    NBIOT_APP_CoAP_SEVER  = NBIOT_CoAP_SEVER,   // CoAP远程服务器设置
    NBIOT_APP_CoAP_CLIENT = NBIOT_CoAP_CLIENT,  // CoAP客户端连接地址设置
    NBIOT_APP_CoAP_SEND   = NBIOT_CoAP_SEND,    // CoAP发送消息
    NBIOT_APP_CoAP_RECV   = NBIOT_CoAP_RECV,    // CoAP返回信息
    NBIOT_APP_CoAP_CL     = NBIOT_CoAP_CL,      // 关闭CoAP
    NBIOT_APP_NCDP_SERVER = NBIOT_NCDP_SERVER,  // NCDP客户端连接地址设置
    NBIOT_APP_NCDP_SEND   = NBIOT_NCDP_SEND,    // NCDP发送消息
    NBIOT_APP_NCDP_RECV   = NBIOT_NCDP_RECV,    // NCDP返回信息
    NBIOT_APP_NCDP_STATUS = NBIOT_NCDP_STATUS,  // NCDP返回状态消息
    NBIOT_APP_NCDP_CL     = NBIOT_NCDP_CL,      // 关闭NCDP 
    NBIOT_APP_RESET       = NBIOT_RESET,        // 复位NB
    NBIOT_APP_NCONFIG     = NBIOT_NCONFIG,      // 使能/禁能自动入网设置
    NBIOT_APP_END          
}nbiot_app_status_t;


//nbiot消息id, 回调函数中使用
typedef enum nbiot_msg_id
{
    NBIOT_MSG_NONE,

    NBIOT_MSG_INIT,       //初始化完成消息

    NBIOT_MSG_INFO,       //固件信息获取完成消息
    
    NBIOT_MSG_REG,        //注册信息获取完成消息      
  
    NBIOT_MSG_IMEI,       //移动设备身份码获取完成消息   
    NBIOT_MSG_IMSI,
  
    NBIOT_MSG_MID,        //产商ID获取完成消息
    NBIOT_MSG_MMODEL,     //模块型号获取完成消息
    NBIOT_MSG_MREV,       //软件版本号获取完成消息
    NBIOT_MSG_BAND,       //工作频段获取完成消息
  
    NBIOT_MSG_CSQ,        
    NBIOT_MSG_SIGNAL,     //信号强度获取完成消息
    

    NBIOT_MSG_TCPUDP_CREATE,
    NBIOT_MSG_TCPUDP_CLOSE,
    NBIOT_MSG_TCPUDP_SEND,
    NBIOT_MSG_TCPUDP_RECV,
    
    NBIOT_MSG_SOCKET_ERROR, //收到socket错误消息    

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

    NBIOT_MSG_RESET,        //NB复位完成消息
    NBIOT_MSG_NCONFIG,      //自动入网设置完成消息

    NBIOT_MSG_CMD_RETRY,    //命令出错重试消息
    
    NBIOT_MSG_CMD_NEXT,     //命令出错后跳过出错命令执行下一条命令消息
    
    NBIOT_MSG_CMD_FAIL,     //命令出错后退出执行消息 

    NBIOT_MSG_END           
  
}nbiot_msg_id_t;

#define  NON_MESSAGE          "0X0000"
#define  NON_MESSAGE_RELEASE  "0X0001"
#define  CON_MESSAGE          "0X0100"
#define  CON_MESSAGE_RELEASE  "0X0101"

//NB模块初始化及完成网络注册
int nbiot_init (nbiot_handle_t nbiot_handle);

//重启nbiot模块
int nbiot_reboot(nbiot_handle_t nbiot_handle);

//使能/禁能自动入网
int nbiot_nconfig(nbiot_handle_t nbiot_handle, uint8_t auto_flag);

//获取NB模块的信息
int nbiot_info_get(nbiot_handle_t nbiot_handle);

//获取NB模块的信号质量
int nbiot_signal_get(nbiot_handle_t nbiot_handle);


//创建tcpudp 
int nbiot_tcpudp_create(nbiot_handle_t nbiot_handle, nbiot_connect_type_t type);


//关闭tcpudp
int nbiot_tcpudp_close(nbiot_handle_t nbiot_handle, nbiot_connect_type_t type);


//以hex数据格式发送数据
int nbiot_tcpudp_send_hex(nbiot_handle_t nbiot_handle, 
                                  int len, 
                                  char *msg, 
                                  nbiot_connect_type_t type);

//以字符串格式发送数据
int nbiot_tcpudp_send_str(nbiot_handle_t nbiot_handle, 
                                  int len, 
                                  char *msg, 
                                  nbiot_connect_type_t type);
                                  
//创建coap服务器 
int nbiot_coap_server_create(nbiot_handle_t nbiot_handle, nbiot_connect_type_t type);

//创建coap客户端
int nbiot_coap_client_create(nbiot_handle_t nbiot_handle, nbiot_connect_type_t type);


//关闭coap
int nbiot_coap_close(nbiot_handle_t nbiot_handle, nbiot_connect_type_t type);


//以hex数据格式发送coap协议数据,必须是偶数个长度
int nbiot_coap_send_hex(nbiot_handle_t nbiot_handle, int len, char *msg, nbiot_connect_type_t type);

//以字符串格式发送数据coap协议数据
int nbiot_coap_send_str(nbiot_handle_t nbiot_handle, int len, char *msg, nbiot_connect_type_t type);                                                                  

//创建ncdp客户端
int nbiot_ncdp_update(nbiot_handle_t nbiot_handle, nbiot_connect_type_t type);


//关闭ncdp
int nbiot_ncdp_close(nbiot_handle_t nbiot_handle, nbiot_connect_type_t type);


//以hex数据格式发送ncdp协议数据,必须是偶数个长度
int nbiot_ncdp_send_hex(nbiot_handle_t nbiot_handle, int len, char *msg, nbiot_connect_type_t type, char *mode);


#endif


