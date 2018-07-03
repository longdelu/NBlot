#ifndef _USART_H
#define _USART_H
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
#define LPUSART_REC_LEN  			200  	  //定义最大接收字节数 200
#define EN_LPUART1_RX 			1		  //使能（1）/禁止（0）串口接收
	  	
extern u8  NBLOT_RxBuffer[LPUSART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 NBLOT_USART_RX_STA;         	  //接收状态标记	
extern UART_HandleTypeDef hlpuart1;       //UART句柄

//初始化IO LPUART1
//bound:波特率
void lpuart1_init (u32 bound);


//轮询发送串口数据
int uart_data_tx_poll(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t size);

//轮询接收串口数据
int uart_data_rx_poll(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t size);


#endif
