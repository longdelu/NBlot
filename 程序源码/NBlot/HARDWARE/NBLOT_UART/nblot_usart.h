#ifndef _NBLOT_USART_H
#define _NBLOT_USART_H
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

#define UART_NONE_EVENT              0X0000
#define UART_TX_EVENT                0X0001
#define UART_RX_EVENT                0X0002
#define UART_TX_TIMEOUT_EVENT        0X0004
#define UART_RX_TIMEOUT_EVENT        0X0008

                               
#define RING_BUFF_LEN            512         //定义环形缓冲区最大接收字节数

//
typedef struct atk_ring_buf_t
{
    uint32_t head;           
    uint32_t tail;
    uint32_t lenght;
    uint8_t  ring_buff[RING_BUFF_LEN];    
    
}atk_ring_buf_t;


extern atk_ring_buf_t g_uart_ring_buff;      //创建一个ringBuff的缓冲区 

//定义串口事件回调函数指针
typedef void (*uart_cb)(void *p_arg);

//串口设备结构体
typedef struct uart_dev
{  
    UART_HandleTypeDef *p_huart; 
    
    struct atk_soft_timer uart_rx_timer;  
    struct atk_soft_timer uart_tx_timer;
    
    /* 串口事件回调函数 */
    uart_cb  uart_cb;
    
    void     *p_arg;   

    int       uart_event;  

    uint32_t  bound;    
    
}uart_dev_t;

#define EN_LPUART1_RX             1           //使能（1）/禁止（0）串口接收

//uart设备句柄
typedef uart_dev_t *uart_handle_t;
          
extern UART_HandleTypeDef hlpuart1;            //UART句柄
                   
//初始化IO LPUART1
//bound:波特率
uart_dev_t *lpuart1_init (u32 bound);


//轮询发送串口数据
int uart_data_tx_poll(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t size, uint32_t Timeout);

//轮询接收串口数据
int uart_data_rx_poll(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t size, uint32_t Timeout);


//中断接收串口数据
int uart_data_rx_int(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t size, uint32_t Timeout);

//中断发送串口数据， 保留使用
int uart_data_tx_int(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t size, uint32_t Timeout);


//注册串口事件回调函数
void lpuart_event_registercb(uart_cb cb, void *p_arg);


//设置串口事件
void lpuart_event_set (int uart_event);

//获取串口事件
int lpuart_event_get (int uart_event);

//清除串口事件
void lpuart_event_clr (int uart_event);


//轮询串口事件
void uart_event_poll(uart_handle_t uart_handle);

#endif
