#ifndef _SIM7020_NBIOT_H
#define _SIM7020_NBIOT_H

#include "sim7020.h"
#include "delay.h"

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




//sim7020 app状态定义
typedef enum sim7020_app_status
{
    SIM7020_APP_NONE,        
    SIM7020_APP_NBLOT_INIT  = SIM7020_NBLOT_INIT,   // 初始化操作
    SIM7020_APP_NBLOT_INFO  = SIM7020_NBLOT_INFO,   // 获取 NB 模块厂商及固件，频段等信息
    SIM7020_APP_SIGNAL      = SIM7020_SIGNAL,       // 获取信号质量
    SIM7020_APP_TCPUDP_CR   = SIM7020_TCPUDP_CR,    // 创建 TCP/UDP
    SIM7020_APP_TCPUDP_CL   = SIM7020_TCPUDP_CL,    // 关闭 TCP/UDP
    SIM7020_APP_TCPUDP_SEND = SIM7020_TCPUDP_SEND,  // 利用已经创建的TCP/UDP发送数据
    SIM7020_APP_TCPUDP_RECV = SIM7020_TCPUDP_RECV,  // TCP/UDP接收信息 
    SIM7020_APP_SOCKET_ERR  = SIM7020_SOCKET_ERR,   // SOCKET连接发生错误
    SIM7020_APP_CoAP_SEVER  = SIM7020_CoAP_SEVER,   // CoAP远程服务器设置
    SIM7020_APP_CoAP_CLIENT = SIM7020_CoAP_CLIENT,  // CoAP客户端连接地址设置
    SIM7020_APP_CoAP_SEND   = SIM7020_CoAP_SEND,    // CoAP发送消息
    SIM7020_APP_CoAP_RECV   = SIM7020_CoAP_RECV,    // CoAP返回信息
    SIM7020_APP_CoAP_CL     = SIM7020_CoAP_CL,      // 关闭CoAP
    SIM7020_APP_CM2M_CLIENT = SIM7020_CM2M_CLIENT,  // CM2M客户端连接地址设置
    SIM7020_APP_CM2M_SEND   = SIM7020_CM2M_SEND,    // CM2M发送消息
    SIM7020_APP_CM2M_RECV   = SIM7020_CM2M_RECV,    // CM2M返回信息
    SIM7020_APP_CM2M_STATUS = SIM7020_CM2M_STATUS,  // CM2M返回状态消息
    SIM7020_APP_CM2M_CL     = SIM7020_CM2M_CL,      // 关闭CM2M 
    SIM7020_APP_RESET       = SIM7020_RESET,        // 复位NB
    SIM7020_APP_END          
}sim7020_app_status_t;


//sim7020消息id, 回调函数中使用
typedef enum sim7020_msg_id
{
    SIM7020_MSG_NONE,

    SIM7020_MSG_NBLOT_INIT,

    SIM7020_MSG_NBLOT_INFO,
    
    SIM7020_MSG_REG,
  
    SIM7020_MSG_IMEI,       //移动设备身份码   
    SIM7020_MSG_IMSI,
  
    SIM7020_MSG_MID,        //产商ID
    SIM7020_MSG_MMODEL,     //模块型号
    SIM7020_MSG_MREV,       //软件版本号
    SIM7020_MSG_BAND,       //工作频段
  
    SIM7020_MSG_CSQ,        
    SIM7020_MSG_SIGNAL,     //信号强度
    

    SIM7020_MSG_TCPUDP_CREATE,
    SIM7020_MSG_TCPUDP_CLOSE,
    SIM7020_MSG_TCPUDP_SEND,
    SIM7020_MSG_TCPUDP_RECV,
    
    SIM7020_MSG_SOCKET_ERROR, //socket错误    

    SIM7020_MSG_COAP_SERVER,
    SIM7020_MSG_COAP_CLIENT,
    SIM7020_MSG_COAP_SEND,
    SIM7020_MSG_COAP_RECV,
    SIM7020_MSG_COAP_CLOSE,
    
    SIM7020_MSG_CM2M_CLIENT,
    SIM7020_MSG_CM2M_SEND,
    SIM7020_MSG_CM2M_RECV,
    SIM7020_MSG_CM2M_STATUS,
    SIM7020_MSG_CM2M_CLOSE,   


    SIM7020_MSG_CMD_RETRY,
    
    SIM7020_MSG_CMD_NEXT,
    
    SIM7020_MSG_CMD_FAIL,    

    SIM7020_MSG_END
  
}sim7020_msg_id_t;


//sim7020 nblot初始化及完成网络注册
int sim7020_nblot_init (sim7020_handle_t sim7020_handle);

//获取NB模块的信息
int sim7020_nblot_info_get(sim7020_handle_t sim7020_handle);

//获取NB模块的信号质量
int sim7020_nblot_signal_get(sim7020_handle_t sim7020_handle);


//创建tcpudp 
int sim7020_nblot_tcpudp_create(sim7020_handle_t sim7020_handle, sim7020_connect_type_t type);


//关闭tcpudp
int sim7020_nblot_tcpudp_close(sim7020_handle_t sim7020_handle, sim7020_connect_type_t type);


//以hex数据格式发送数据
int sim7020_nblot_tcpudp_send_hex(sim7020_handle_t sim7020_handle, 
                                  int len, 
                                  char *msg, 
                                  sim7020_connect_type_t type);

//以字符串格式发送数据
int sim7020_nblot_tcpudp_send_str(sim7020_handle_t sim7020_handle, 
                                  int len, 
                                  char *msg, 
                                  sim7020_connect_type_t type);
                                  
//创建coap服务器 
int sim7020_nblot_coap_server_create(sim7020_handle_t sim7020_handle, sim7020_connect_type_t type);

//创建coap客户端
int sim7020_nblot_coap_client_create(sim7020_handle_t sim7020_handle, sim7020_connect_type_t type);


//关闭coap
int sim7020_nblot_coap_close(sim7020_handle_t sim7020_handle, sim7020_connect_type_t type);


//以hex数据格式发送coap协议数据,必须是偶数个长度
int sim7020_nblot_coap_send_hex(sim7020_handle_t sim7020_handle, int len, char *msg, sim7020_connect_type_t type);

//以字符串格式发送数据coap协议数据
int sim7020_nblot_coap_send_str(sim7020_handle_t sim7020_handle, int len, char *msg, sim7020_connect_type_t type);                                                                  

//创建cm2m客户端
int sim7020_nblot_cm2m_client_create(sim7020_handle_t sim7020_handle, sim7020_connect_type_t type);


//关闭cm2m
int sim7020_nblot_cm2m_close(sim7020_handle_t sim7020_handle, sim7020_connect_type_t type);

//以hex数据格式发送cm2m协议数据,必须是偶数个长度
int sim7020_nblot_cm2m_send_hex(sim7020_handle_t sim7020_handle, int len, char *msg, sim7020_connect_type_t type); 



#endif


