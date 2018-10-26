#ifndef _NBIOT_USART_H
#define _NBIOT_USART_H

#include "atk_sys.h"
#include "stdio.h"    
#include "atk_soft_timer.h"
#include "atk_ring_buf.h"

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

/**
 * @brief  串口任意数据长度收发使能宏，一般需要开启
 */
#define UART_ANY_DATA_LEN_RECV       1

/**
 * @brief  定义串口事件回调函数指针
 */
#define UART_NONE_EVENT              0X0000
#define UART_TX_EVENT                0X0001
#define UART_RX_EVENT                0X0002
#define UART_TX_TIMEOUT_EVENT        0X0004
#define UART_RX_TIMEOUT_EVENT        0X0008


/**
 * @brief  定义串口事件回调函数指针
 */
typedef void (*uart_cb)(void *p_arg);

 /**
  * @brief  串口设备结构体
  */
typedef struct uart_dev
{  
    //串口设备结构体指针
    LPUART_Type *p_huart; 
  
    //定义收发数据超时软定时器
    struct atk_soft_timer uart_rx_timer;  
    struct atk_soft_timer uart_tx_timer;
    
    //环形缓冲区结构体指针
    atk_ring_buf_t *p_uart_ring_buff;  
    
    //事件回调函数
    uart_cb         uart_cb;
    
    //回调函数
    void           *p_arg;   

    //事件标记组
    volatile int    uart_event;  

    //当前使用的波特率
    uint32_t        bound;    
    
}uart_dev_t;

 /**
  * @brief  串口设备句柄类型
  */
typedef uart_dev_t *uart_handle_t;

 /**
  * @brief  从UART接收缓存里读取指定长度的数据，并释放占用的空间
  * @param  uart_handle : 指向串口设备句柄的指针.
  * @param  data        : 数据缓冲区首地址.
  * @param  len         : 读取数据的长度
  * @retval 0 读取串口数据成功 -1 读取串口数据
  */
int uart_ring_buf_read(uart_handle_t uart_handle, uint8_t *data, int len);

 /**
  * @brief  获取串口环形缓冲区中有效数据的个数
  * @param  uart_handle : 指向串口设备句柄的指针.
  * @retval 缓冲区实际有效个数
  */
int uart_ring_buf_avail_len(uart_handle_t uart_handle);

/**
  * @brief  写入uart接收缓存里指定长度的数据，并占用的空间
  * @param  uart_handle : 指向串口设备句柄的指针.
  * @param  data        : 数据缓冲区首地址.
  * @param  len         : 写入数据的长度
  * @retval 0 写入串口数据成功 -1 写入串口数据
  */
int uart_ring_buf_write(uart_handle_t uart_handle, uint8_t *data, int len);

                   
/**
 * @brief  串口初始化
 * @param  bound : 波特率.
 * @retval 串口设备句柄的指针
 */
uart_dev_t *atk_nbiot_uart_init (u32 bound);


 /**
  * @brief  轮询发送串口数据
  * @param  uart_handle : 指向串口设备句柄的指针.
  * @param  pData       : 数据缓冲区首地址.
  * @param  size        : 发送数据的长度
  * @retval 0 中断发送数据成功
  */
int uart_data_tx_poll(uart_handle_t uart_handle, uint8_t *pData,uint16_t size, uint32_t Timeout);



 /**
  * @brief  轮询接收串口数据（保留使用）
  * @param  uart_handle : 指向串口设备句柄的指针.
  * @param  pData       : 数据缓冲区首地址.
  * @param  size        : 接收数据的长度
  * @retval 0 接收数据成功
  */
int uart_data_rx_poll(uart_handle_t uart_handle, uint8_t *pData, uint16_t size, uint32_t Timeout);



 /**
  * @brief  中断接收串口数据（保留使用）
  * @param  uart_handle : 指向串口设备句柄的指针.
  * @param  pData       : 数据缓冲区首地址.
  * @param  size        : 接收数据的长度
  * @retval 0 接收数据成功
  */
int uart_data_rx_int(uart_handle_t uart_handle, uint8_t *pData, uint16_t size, uint32_t Timeout);

 /**
  * @brief  中断发送串口数据， 保留使用
  * @param  uart_handle : 指向串口设备句柄的指针.
  * @param  pData       : 数据缓冲区首地址.
  * @param  size        : 发送数据的长度
  * @retval 0 中断发送数据成功
  */
int uart_data_tx_int(uart_handle_t uart_handle, uint8_t *pData, uint16_t size, uint32_t Timeout);


/**
  * @brief  注册串口事件回调函数
  * @param  uart_handle : 指向串口设备句柄的指针.
  * @param  cb          : 回调函数.
  * @param  p_arg       : 回调函数参数
  * @retval None
  */
void uart_event_registercb(uart_handle_t uart_handle, uart_cb cb, void *p_arg);


/**
  * @brief  设置串口事件
  * @param  uart_handle : 指向串口设备句柄的指针.
  * @param  uart_event  : 串口事件.
  * @retval None
  */
void uart_event_set (uart_handle_t uart_handle,int uart_event);

/**
  * @brief  判断当前的串口事件是否发生
  * @param  uart_handle : 指向串口设备句柄的指针.
  * @param  uart_event  : 串口事件.
  * @retval 0 没有串口事件发生  非0 当前串口事件已经发生
  */
int uart_event_get (uart_handle_t uart_handle, int uart_event);

/**
  * @brief  清除串口事件
  * @param  uart_handle : 指向串口设备句柄的指针.
  * @param  uart_event  : 串口事件.
  * @retval None
  */
void uart_event_clr (uart_handle_t uart_handle, int uart_event);

 /**
  * @brief  串口事件轮询
  * @param  uart_handle : 指向串口设备句柄的指针.
  * @retval None
  */
void uart_event_poll(uart_handle_t uart_handle);

#endif
