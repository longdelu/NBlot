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

//串口任意长度设备句柄
#define UART_ANY_DATA_LEN_RECV       1


#define UART_NONE_EVENT              0X0000
#define UART_TX_EVENT                0X0001
#define UART_RX_EVENT                0X0002
#define UART_TX_TIMEOUT_EVENT        0X0004
#define UART_RX_TIMEOUT_EVENT        0X0008

//定义环形缓冲区最大接收字节数                               
#define RING_BUF_LEN                  512         

//环形缓冲区结构体
typedef struct atk_ring_buf_t
{
    uint32_t head;           
    uint32_t tail;
    uint32_t lenght;
    uint8_t  ring_buf[RING_BUF_LEN];  
    
}atk_ring_buf_t;


//定义串口事件回调函数指针
typedef void (*uart_cb)(void *p_arg);

//串口设备结构体
typedef struct uart_dev
{  
    UART_HandleTypeDef *p_huart; 
    
    struct atk_soft_timer uart_rx_timer;  
    struct atk_soft_timer uart_tx_timer;
    
    //如果不使用形形缓冲区，此值为NULL
    atk_ring_buf_t *p_uart_ring_buff;  
    
    //事件回调函数
    uart_cb  uart_cb;
    
    //回调函数
    void     *p_arg;   

    //事件标记组
    int       uart_event;  

    //当前使用的波特率
    uint32_t  bound;    
    
}uart_dev_t;

//uart设备句柄
typedef uart_dev_t *uart_handle_t;


/**
 * @brief 初始化环形缓冲区的相关信息
 */
int atk_ring_buf_init (atk_ring_buf_t *p_ring_buf);

/**
 * @brief 往环形缓冲区写数据
 */
int atk_ring_buf_write(atk_ring_buf_t *p_ring_buf, uint8_t data);


 /**
 * @brief  读取环形缓冲区的数据
 */
int atk_ring_buf_read(atk_ring_buf_t *p_ring_buf, uint8_t *data);

/**
 * @brief 环形缓冲区中有效数据的个数
 */
int atk_ring_buf_avail_len (atk_ring_buf_t *p_ring_buf);

 /**
 * @brief  从接收缓存里读取指定长度的数据，并释放占用的空间
 */
int atk_ring_buf_size_read(atk_ring_buf_t *p_ring_buf, uint8_t *data, int len);


 /**
 * @brief  写入接收缓存里读指定长度的数据，并占用的空间
 */
int atk_ring_buf_size_write(atk_ring_buf_t *p_ring_buf, uint8_t *data, int len);


 /**
 * @brief  从UART接收缓存里读取指定长度的数据，并释放占用的空间
 */
int uart_ring_buf_read(uart_handle_t uart_handle, uint8_t *data, int len);

 /**
 * @brief  获取串口环形缓冲区中有效数据的个数
 */
int uart_ring_buf_avail_len(uart_handle_t uart_handle);

 /**
 * @brief  写入uart接收缓存里读指定长度的数据，并占用的空间
 */
int uart_ring_buf_write(uart_handle_t uart_handle, uint8_t *data, int len);

                   
//初始化IO LPUART1
//bound:波特率
uart_dev_t *lpuart1_init (u32 bound);


//轮询发送串口数据
int uart_data_tx_poll(uart_handle_t uart_handle, uint8_t *pData, uint16_t size, uint32_t Timeout);

//轮询接收串口数据
int uart_data_rx_poll(uart_handle_t uart_handle, uint8_t *pData, uint16_t size, uint32_t Timeout);


//中断接收串口数据
int uart_data_rx_int(uart_handle_t uart_handle, uint8_t *pData, uint16_t size, uint32_t Timeout);

//中断发送串口数据， 保留使用
int uart_data_tx_int(uart_handle_t uart_handle, uint8_t *pData, uint16_t size, uint32_t Timeout);


//注册串口事件回调函数
void lpuart_event_registercb(uart_handle_t uart_handle, uart_cb cb, void *p_arg);


//设置串口事件
void lpuart_event_set (uart_handle_t uart_handle,int uart_event);

//获取串口事件
int lpuart_event_get (uart_handle_t uart_handle, int uart_event);

//清除串口事件
void lpuart_event_clr (uart_handle_t uart_handle, int uart_event);

//轮询串口事件
void uart_event_poll(uart_handle_t uart_handle);

#endif
