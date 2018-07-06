#ifndef _NB_USART_H
#define _NB_USART_H
#include "sys.h"
#include "stdio.h"	
#include "atk_soft_timer.h"

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

#define UART_TX_EVENT                0X0001
#define UART_RX_EVENT                0X0002
#define UART_TX_TIMEOUT_EVENT        0X0004
#define UART_RX_TIMEOUT_EVENT        0X0008


//定义按键回调函数指针
typedef void (*uart_cb)(u32 key_event, void *p_arg);

//串口描述结构体
typedef struct uart_dev
{  
    int     uart_event;   
    int     start_tick;   
    int     time_out;
    
    struct Timer uart_timer;
    
    /* 串口事件回调函数 */
    uart_cb  lpuart_cb;
    
    void   *p_arg;       
    
}uart_dev_t;


#define LPUSART_REC_LEN  	    512  	    //定义最大接收字节数 200
#define EN_LPUART1_RX 			1		    //使能（1）/禁止（0）串口接收
	  	
extern u8  NBLOT_RxBuffer[LPUSART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 NBLOT_USART_RX_STA;         	    //接收状态标记	
extern UART_HandleTypeDef hlpuart1;         //UART句柄

//初始化IO LPUART1
//bound:波特率
void lpuart1_init (u32 bound);


//轮询发送串口数据
int uart_data_tx_poll(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t size, uint32_t Timeout);

//轮询接收串口数据
int uart_data_rx_poll(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t size, uint32_t Timeout);


#endif
