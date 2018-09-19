#ifndef ATK_BC28_NBIOT_H
#define ATK_BC28_NBIOT_H

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

/**
  * @brief nbiot demo 应用app状态定义
  */
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



/**
  * @brief nbiot消息id, 回调函数中使用
  */
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
    NBIOT_MSG_CSCON,      //获取连接（活动）状态   

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
    
    NBIOT_MSG_CSCON_STATUS,    

    NBIOT_MSG_RESET,        //NB复位完成消息
    NBIOT_MSG_NCONFIG,      //自动入网设置完成消息

    NBIOT_MSG_CMD_RETRY,    //命令出错重试消息
    
    NBIOT_MSG_CMD_NEXT,     //命令出错后跳过出错命令执行下一条命令消息
    
    NBIOT_MSG_CMD_FAIL,     //命令出错后退出执行消息 

    NBIOT_MSG_END           
  
}nbiot_msg_id_t;

/**
  * @brief nbiot 发送消息的定义状态定义
  */
#define  NON_MESSAGE          "0X0000"
#define  NON_MESSAGE_RELEASE  "0X0001"
#define  CON_MESSAGE          "0X0100"
#define  CON_MESSAGE_RELEASE  "0X0101"

/**
  * @brief  设置nbiot模块初始化网络注册
  * @param  nbiot_handle   : 指向nbiot设备句柄的指针.
  * @retval NBIOT_OK 设置初始化成功  NBIOT_ERROR 设置初始化失败
  */
int nbiot_reg_init (nbiot_handle_t nbiot_handle);

/**
  * @brief 设置重启nbiot模块
  * @param  nbiot_handle   : 指向nbiot设备句柄的指针.
  * @retval NBIOT_OK 设置重启成功  NBIOT_ERROR 设置重启失败
  */
int nbiot_reboot(nbiot_handle_t nbiot_handle);

/**
  * @brief 使能/禁能nbiot模块自动入网
  * @param  nbiot_handle  : 指向nbiot设备句柄的指针.
  * @param  auto_flag     : 0 禁能自动入网 1 使能自动入网  
  * @retval NBIOT_OK 设置成功  NBIOT_ERROR 设置失败
  */
int nbiot_nconfig(nbiot_handle_t nbiot_handle, uint8_t auto_flag);


/**
  * @brief 获取nbiot模块的信息
  * @param  nbiot_handle  : 指向nbiot设备句柄的指针.
  * @retval NBIOT_OK 设置成功  NBIOT_ERROR 设置失败
  */
int nbiot_info_get(nbiot_handle_t nbiot_handle);

/**
  * @brief 获取nbiot模块的信号质量
  * @param  nbiot_handle  : 指向nbiot设备句柄的指针.
  * @retval NBIOT_OK 设置成功  NBIOT_ERROR 设置失败
  */
int nbiot_signal_get(nbiot_handle_t nbiot_handle);

                                                               
/**
  * @brief 更新ncdp服务器
  * @param  nbiot_handle  : 指向nbiot设备句柄的指针.
  * @param  type          : 连接类型，值仅为枚举当中的值
  * @retval NBIOT_OK 设置成功  NBIOT_ERROR 设置失败  
  */
int nbiot_ncdp_update(nbiot_handle_t nbiot_handle, nbiot_connect_type_t type);


/**
  * @brief 关闭ncdp
  * @param  nbiot_handle  : 指向nbiot设备句柄的指针.
  * @param  type          : 连接类型，值仅为枚举当中的值
  * @retval NBIOT_OK 设置成功  NBIOT_ERROR 设置失败  
  */
int nbiot_ncdp_close(nbiot_handle_t nbiot_handle, nbiot_connect_type_t type);

/**
  * @brief 以hex格式字符串发送业务逻辑数据,数据有效长度必须是偶数个大小且不能包含特殊字符
  * @param  nbiot_handle  : 指向nbiot设备句柄的指针.
  * @param  type          : 连接类型，值仅为枚举当中的值
  * @param  mode          : 一般设为为NULL即可
  * @retval NBIOT_OK 设置成功  NBIOT_ERROR 设置失败  NBIOT_NOTSUPPORT 数据有效长度不为偶数个长充 
  */
int nbiot_ncdp_send_hexstr (nbiot_handle_t nbiot_handle, 
                            int len, 
                            char *msg, 
                            nbiot_connect_type_t type, 
                            char *mode);


/**
  * @brief  缓冲区当中每两个字节重新组成一个一字节十六进制数(字符).
  * @param  p_buf     : 源数据缓冲区首地址.
  * @param  len       : 源缓冲区长度.
  * @note  调用该接口会改变源缓冲区的长度
  * @retval None
  */
void nbiot_buf2hex (char *p_buf , int len);


/**
  * @brief  缓冲区当中每两个字节重新组成一个一字节十六进制数(字符).
  * @param  src_buf   : 源数据缓冲区首地址.
  * @param  dest_buf  : 目标数据缓冲区首地址.
  * @param  len       : 源缓冲区长度.
  * @note   确保目的缓冲区长度足够容纳源缓冲区的数据,其长度至少为源缓冲区长度的一半
  * @retval None
  */
void nbiot_srcbuf2hexbuf (char *src_buf ,char *dest_buf, int len);


/**
  * @brief  缓冲区每1个字符重新组成为两个字符来显示
  * @eg     例如字符'1'为十六进制0x31,则变成两个字符"31"来表示字符'1'
  * @param  src_buf   : 源数据缓冲区首地址.
  * @param  dest_buf  : 目标数据缓冲区首地址.
  * @param  len       : 源缓冲区长度.
  * @note   确保目的缓冲区长度足够容纳源缓冲区的数据,其长度至少为源缓冲区长度的2倍
  * @retval None
  */
void nbiot_srcbuf2hexchrbuf (char *src_buf ,char *dest_buf, int len);


#endif


