#ifndef _SIM7020_H
#define _SIM7020_H
#include "sys.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nblot_usart.h"
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
//V1.0修改说明 
//////////////////////////////////////////////////////////////////////////////////  


#define    NB_SP_RECE_EVENT       0x0001           //收到串口数据
#define    NB_TIMEOUT_EVENT       0x0002           //超时事件
#define    NB_UDPRECE_EVENT       0x0004           //UDP接收事件
#define    NB_COAP_RE_EVENT       0X0008           //COAP接收事件
#define    NB_REG_STA_EVENT       0x0010           //NB IOT网络附着事件

/*
 * AT命令常量定义区域
 */
#define AT_SYNC        "AT"
#define AT_ATI         "ATI"
#define AT_CGMI        "AT+CGMI"
#define AT_CGMM        "AT+CGMM"
#define AT_CGMR        "AT+CGMR"
#define AT_CGSN        "AT+CGSN"
#define AT_CEREG       "AT+CEREG"
#define AT_CSCON       "AT+CSCON"
#define AT_CLAC        "AT+CLAC"
#define AT_CSQ         "AT+CSQ"
#define AT_CGPADDR     "AT+CGPADDR"
#define AT_COPS        "AT+COPS"
#define AT_CGATT       "AT+CGATT"
#define AT_CGACT       "AT+CGACT"
#define AT_CIMI        "AT+CIMI"
#define AT_CGDCONT     "AT+CGDCONT"
#define AT_CFUN        "AT+CFUN"
#define AT_CMEE        "AT+CMEE"
#define AT_CCLK        "AT+CCLK";
#define AT_CPSMS       "AT+CPSMS";
#define AT_CEDRXS      "AT+CEDRXS";
#define AT_CEER        "AT+CEER";
#define AT_CEDRXRDP    "AT+CEDRXRDP";
#define AT_CTZR        "AT+CTZR";

/* ETSI Commands* (127.005)  <Under development> */
/*
#define AT_CSMS       "AT+CSMS";
#define AT_CNMA       "AT+CNMA";
#define AT_CSCA       "AT+CSCA";
#define AT_CMGS       "AT+CMGS";
#define AT_CMGC       "AT+CMGC";
#define AT_CSODCP     "AT+CSODCP";
#define AT_CRTDCP     "AT+CRTDCP";
*/
#define AT_NMGS       "AT+NMGS";
#define AT_NMGR       "AT+NMGR";
#define AT_NNMI       "AT+NNMI";
#define AT_NSMI       "AT+NSMI";
#define AT_NQMGR      "AT+NQMGR";
#define AT_NQMGS      "AT+NQMGS";
#define AT_NMSTATUS   "AT+NMSTATUS";
#define AT_NRB        "AT+NRB";
#define AT_NCDP       "AT+NCDP";

#define AT_NUESTATS   "AT+NUESTATS";

#define AT_NEARFCN    "AT+NEARFCN";
#define AT_NSOCR      "AT+NSOCR";
#define AT_NSOST      "AT+NSOST";
#define AT_NSOSTF     "AT+NSOSTF";
#define AT_NSORF      "AT+NSORF";
#define AT_NSOCL      "AT+NSOCL";  
      
#define  AT_NPING          "AT+NPING";
#define  AT_NBAND          "AT+NBAND";
#define  AT_NLOGLEVEL      "AT+NLOGLEVEL";
#define  AT_NCONFIG        "AT+NCONFIG";
#define  AT_NATSPEED       "AT+NATSPEED";
#define  AT_NCCID          "AT+NCCID";
#define  AT_NFWUPD         "AT+NFWUPD";
#define  AT_NRDCTRL        "AT+NRDCTRL";
#define  AT_NCHIPINFO      "AT+NCHIPINFO";
#define  AT_NTSETID        "AT+NTSETID";

//                                                                              
//

#define CMD_TRY_TIMES           10
#define CMD_READ_ARGUMENT       "?"
#define CMD_TEST_ARGUMENT       "=?"

#define CMD_OK_RES              "OK"

#define REMOTE_SERVER_IP        "115.29.240.46"
#define REMOTE_SERVER_PORT      "6000"


#define REMOTE_COAP_INFO        "115.29.240.46,5683"

#define LOCAL_UDP_SET           "DGRAM,17,10000,1"

#define BAND_850MHZ_ID           5
#define BAND_850MHZ_STR          "850"

#define BAND_900MHZ_ID           8
#define BAND_900MHZ_STR          "900"

#define BAND_800MHZ_ID           20
#define BAND_800MHZ_STR          "800"

#define BAND_700MHZ_ID           28
#define BAND_700MHZ_STR          "700"


/*
 * cmd 属性枚举
 */
typedef enum
{
   CMD_TEST,         //命令TEST操作
   CMD_READ,         //命令READ操作
   CMD_SET,          //命令SET操作
   CMD_EXCUTE        //命令EXCUTE操作
}cmd_property_t;

/*
 * AT指令动作行为枚举
 */
typedef enum
{
  ACTION_OK,                      //命令成功执行
  ACTION_ERROR,                   //命令执行错误
    
  ACTION_ERROR_NEXT,              //命令执行错误将继续执行下一条指令
  ACTION_ERROR_TRY                //命令执行错误后进行尝试
}cmd_action_t;

//AT指令结构类型
typedef struct at_cmd_info
{
    const char*     p_atcmd;       // AT指令
    cmd_property_t  property;      // 指令当前属性(TEST,READ,SET,EXCUTE)
    char*           p_atcmd_arg;   // 指令参数
    char*           p_expectres;   // 期望得到回复
    unsigned char   cmd_try;       // 出错尝试次数
    unsigned char   have_tried;    // 已经出次尝试的次数
    cmd_action_t    cmd_action;    // AT指令行为
    uint32_t        max_timeout;   // 最大超时时间
}at_cmd_info_t;


//声明AT cmd结构指针类型
typedef at_cmd_info_t *at_cmdhandle;

#define    SIM7020_NONE_EVENT          0x0000           //没有其它事件发生
#define    SIM7020_RECV_EVENT          0x0001           //收到串口数据事件
#define    SIM7020_TIMEOUT_EVENT       0x0002           //超时事件
#define    SIM7020_REG_STA_EVENT       0x0004           //NBIOT网络附着事件
#define    SIM7020_TCP_RECV_EVENT      0x0008           //TCP接收事件
#define    SIM7020_UDP_RECV_EVENT      0x0010           //UDP接收事件
#define    SIM7020_COAP_RECV_EVENT     0X0020           //COAP接收事件


//sim7020 主状态定义，app可以使用该信息
typedef enum sim7020_main_status
{
    SIM7020_NONE,
    SIM7020_INIT,         // 初始化操作
    SIM7020_MODULE_INFO,  // 获取 NB 模块厂商及固件，频段等信息
    SIM7020_SIGN,         // 获取信号质量
    SIM7020_UDP_CR,       // 创建 UDP
    SIM7020_UDP_CL,       // 关闭 UDP
    SIM7020_TCP_CR,       // 创建 TCP
    SIM7020_TCP_CL,       // 关闭 TCP
    SIM7020_UDP_SEND,     // 利用已经创建的UDP发送数据
    SIM7020_UDP_RECV,     // UDP接收信息
    SIM7020_TCP_SEND,     // 利用已经创建的TCP发送数据
    SIM7020_TCP_RECV,     // TCP接收信息    
    SIM7020_CoAP_SEVER,   // CoAP远程地址设置与获取
    SIM7020_CoAP_SEND,    // 利用CoAP发送消息
    SIM7020_CoAP_RECV,    // CoAP返回信息
    SIM7020_RESET,        // 复位NB
    SIM7020_END
    
}sim7020_main_status_t;


//sim7020 状态信息
typedef struct sim7020_status_nest
{
    sim7020_main_status_t  main_status;         //主阶段
    int                    sub_status;          //子阶段，用于状态嵌套    
    uint8_t                connect_status;
    uint8_t                register_status;  
    uint8_t                socket_id;           //指示相应的socket id
    uint16_t               data_len;            //提示数据长度     
}sim7020_status_nest_t;

//SIM7020G固件信息
typedef struct sim020_firmware_info
{
    uint8_t      IMSI[16];
    uint8_t      IMEI[16];      
}sim020_firmware_info_t;


//定义收发数据缓冲区长度
#define NB_UART_RECE_BUF_MAX_LEN    512
#define NB_UART_SEND_BUF_MAX_LEN    512

//接收缓存空间
typedef struct sim7020_recv
{
    char      buf[NB_UART_RECE_BUF_MAX_LEN];    //接收数据缓冲区
    uint16_t  len;                              //有效数据长度
}sim7020_recv_t;

//接收缓存空间
typedef struct sim7020_send
{
    char      buf[NB_UART_SEND_BUF_MAX_LEN];    //发送数据缓冲区
    uint16_t  len;                              //有效数据长度
}sim7020_send_t;


//sim7020驱动函数结构体
struct sim7020_drv_funcs {
    
    //sim7020发送数据
    int (*sim7020_send_data) (void *p_arg, uint8_t *pData, uint16_t size, uint32_t Timeout);
    //sim7020接收数据
    int (*sim7020_recv_data) (void *p_arg, uint8_t *pData, uint16_t size, uint32_t Timeout);    
};

//定义串口事件回调函数指针
typedef void (*sim7020_cb)(void *p_arg);

//sim7020设备结构体
typedef struct sim7020_dev
{     
    struct sim7020_drv_funcs *p_drv_funcs;
    
    uart_dev_t               *p_uart_dev;
    
    //sim7020设备事件回调函数
    sim7020_cb                sim7020_cb;  
    
    void                     *p_arg;

    //事件标记
    int                       sim7020_event; 

    //sim7020状态信息
    sim7020_status_nest_t    *sim702_status;
    //sim7020固件信息
    sim020_firmware_info_t   *firmware_info;      
             
}sim7020_dev_t;

//sim7020设备句柄
typedef sim7020_dev_t *sim7020_handle_t;


//设置sim7020事件
void sim7020_event_set (int sim7020_event);

//获取sim7020事件
int sim7020_event_get (int sim7020_event);

//清除sim7020事件
void sim7020_event_clr (int sim7020_event);


//sim7020初始化 
sim7020_handle_t sim7020_init(uart_handle_t lpuart_handle);

//注册sim7020事件回调处理函数
void sim7020_event_registercb(sim7020_cb cb, void *p_arg);

//sim7020消息事件处理函数
void sim7020_app_status_poll(int *sim702_main_status);

#endif
