#ifndef _SIM7020_H
#define _SIM7020_H
#include "sys.h"
#include "stdio.h"	
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


#endif
