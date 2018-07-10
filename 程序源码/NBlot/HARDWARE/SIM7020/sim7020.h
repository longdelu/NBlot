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
#define  AT_NMGS      "AT+NMGS";
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




//定义收发数据缓冲区长度
#define NB_UART_RECE_BUF_MAX_LEN    512
#define NB_UART_SEND_BUF_MAX_LEN    512

typedef enum sim7020state
{
    SIM7020NONE,
    SIM7020INIT,       // 初始化操作
    SIM7020MODULE,     // 获取 NB 模块厂商及固件，频段等信息
    SIM7020SIGN,
    SIM7020UDP_CR,     // 创建 UDP
    SIM7020UDP_CL,     // 关闭 UDP
    SIM7020TCP_CR,     // 创建 TCP
    SIM7020TCP_CL,     // 关闭 TCP
    SIM7020UDP_ST,     // 利用已经创建的 UDP 发送数据
    SIM7020UDP_RE,     // UDP 接收信息
    SIM7020CoAP_SEVER, // CoAP 远程地址设置与获取
    SIM7020CoAP_ST,    // 利用 CoAP 发送消息
    SIM7020CoAP_RE,    // CoAP 返回信息
    SIM7020RESET,      // 复位 NB
    SIM7020END    
}sim7020state_t;

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


// 定义存储SIM7020的状态信息
typedef struct sim020_status
{
    uint8_t      connect_status;
    uint8_t      register_status;
    uint8_t      IMSI[16];
    uint8_t      IMEI[16];
    uint8_t      socket_id;     //指示相应的socket id
    uint16_t     data_len;      //提示数据长度
}sim020_status_t;


//sim7020驱动函数结构体
struct sim7020_drv_funcs {
    
    //sim7020发送数据
    int (*sim7020_send_data) (UART_HandleTypeDef *p_huart, uint8_t *pData, uint16_t size, uint32_t Timeout);
    //sim7020接收数据
    int (*sim7020_recv_data) (UART_HandleTypeDef *p_huart, uint8_t *pData, uint16_t size, uint32_t Timeout);    
};

//定义串口事件回调函数指针
typedef void (*sim7020_cb)(void *p_arg);

//sim7020设备结构体
typedef struct sim7020_dev
{     
    struct sim7020_drv_funcs *p_funcs;
    
    uart_dev_t               *p_uart_dev;
    
    //sim7020设备事件回调函数
    sim7020_cb  sim7020_cb;  
    
    void       *p_arg;   
          
}sim7020_dev_t;

//sim7020设备句柄
typedef sim7020_dev_t *sim7020_handle_t;

#endif
