#ifndef _USART_H
#define _USART_H
#include "sys.h"
#include "stdio.h"    
#include "fsl_common.h"
#include "fsl_iomuxc.h"
//#include "pin_mux.h"
#include "fsl_debug_console.h"
//////////////////////////////////////////////////////////////////////////////////     
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK I.MX RT1052开发板
//串口1初始化           
//正点原子@ALIENTEK
//技术论坛:www.openedv.csom
//修改日期:2017/12/10
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved
//********************************************************************************
//V1.0修改说明 
//////////////////////////////////////////////////////////////////////////////////     

//跟NXP官方例程里提供的跟debug console有关的宏定义
#define BOARD_DEBUG_UART_TYPE         DEBUG_CONSOLE_DEVICE_TYPE_LPUART
#define BOARD_DEBUG_UART_BASEADDR     (uint32_t) LPUART1

#define LPUART_REC_LEN          200      //定义最大接收字节数 200
#define EN_LPUART1_RX             1        //使能（1）/禁止（0）串口1接收
          
extern u8  LPUART_RX_BUF[LPUART_REC_LEN]; //接收缓冲,最大LPUART_REC_LEN个字节.末字节为换行符 
extern u16 LPUART_RX_STA;                 //接收状态标记    

void LPUART1_Init(u32 bound);


#endif
