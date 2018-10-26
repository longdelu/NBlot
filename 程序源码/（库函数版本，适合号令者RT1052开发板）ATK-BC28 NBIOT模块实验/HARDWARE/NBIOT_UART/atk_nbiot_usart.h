#ifndef _NBIOT_USART_H
#define _NBIOT_USART_H

#include "atk_sys.h"
#include "stdio.h"    
#include "atk_soft_timer.h"
#include "atk_ring_buf.h"

//////////////////////////////////////////////////////////////////////////////////     
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F429������
//����1��ʼ��           
//����ԭ��@ALIENTEK
//������̳:www.openedv.csom
//�޸�����:2015/6/23
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
//********************************************************************************
//V1.0�޸�˵�� 
//////////////////////////////////////////////////////////////////////////////////  

/**
 * @brief  �����������ݳ����շ�ʹ�ܺ꣬һ����Ҫ����
 */
#define UART_ANY_DATA_LEN_RECV       1

/**
 * @brief  ���崮���¼��ص�����ָ��
 */
#define UART_NONE_EVENT              0X0000
#define UART_TX_EVENT                0X0001
#define UART_RX_EVENT                0X0002
#define UART_TX_TIMEOUT_EVENT        0X0004
#define UART_RX_TIMEOUT_EVENT        0X0008


/**
 * @brief  ���崮���¼��ص�����ָ��
 */
typedef void (*uart_cb)(void *p_arg);

 /**
  * @brief  �����豸�ṹ��
  */
typedef struct uart_dev
{  
    //�����豸�ṹ��ָ��
    LPUART_Type *p_huart; 
  
    //�����շ����ݳ�ʱ��ʱ��
    struct atk_soft_timer uart_rx_timer;  
    struct atk_soft_timer uart_tx_timer;
    
    //���λ������ṹ��ָ��
    atk_ring_buf_t *p_uart_ring_buff;  
    
    //�¼��ص�����
    uart_cb         uart_cb;
    
    //�ص�����
    void           *p_arg;   

    //�¼������
    volatile int    uart_event;  

    //��ǰʹ�õĲ�����
    uint32_t        bound;    
    
}uart_dev_t;

 /**
  * @brief  �����豸�������
  */
typedef uart_dev_t *uart_handle_t;

 /**
  * @brief  ��UART���ջ������ȡָ�����ȵ����ݣ����ͷ�ռ�õĿռ�
  * @param  uart_handle : ָ�򴮿��豸�����ָ��.
  * @param  data        : ���ݻ������׵�ַ.
  * @param  len         : ��ȡ���ݵĳ���
  * @retval 0 ��ȡ�������ݳɹ� -1 ��ȡ��������
  */
int uart_ring_buf_read(uart_handle_t uart_handle, uint8_t *data, int len);

 /**
  * @brief  ��ȡ���ڻ��λ���������Ч���ݵĸ���
  * @param  uart_handle : ָ�򴮿��豸�����ָ��.
  * @retval ������ʵ����Ч����
  */
int uart_ring_buf_avail_len(uart_handle_t uart_handle);

/**
  * @brief  д��uart���ջ�����ָ�����ȵ����ݣ���ռ�õĿռ�
  * @param  uart_handle : ָ�򴮿��豸�����ָ��.
  * @param  data        : ���ݻ������׵�ַ.
  * @param  len         : д�����ݵĳ���
  * @retval 0 д�봮�����ݳɹ� -1 д�봮������
  */
int uart_ring_buf_write(uart_handle_t uart_handle, uint8_t *data, int len);

                   
/**
 * @brief  ���ڳ�ʼ��
 * @param  bound : ������.
 * @retval �����豸�����ָ��
 */
uart_dev_t *atk_nbiot_uart_init (u32 bound);


 /**
  * @brief  ��ѯ���ʹ�������
  * @param  uart_handle : ָ�򴮿��豸�����ָ��.
  * @param  pData       : ���ݻ������׵�ַ.
  * @param  size        : �������ݵĳ���
  * @retval 0 �жϷ������ݳɹ�
  */
int uart_data_tx_poll(uart_handle_t uart_handle, uint8_t *pData,uint16_t size, uint32_t Timeout);



 /**
  * @brief  ��ѯ���մ������ݣ�����ʹ�ã�
  * @param  uart_handle : ָ�򴮿��豸�����ָ��.
  * @param  pData       : ���ݻ������׵�ַ.
  * @param  size        : �������ݵĳ���
  * @retval 0 �������ݳɹ�
  */
int uart_data_rx_poll(uart_handle_t uart_handle, uint8_t *pData, uint16_t size, uint32_t Timeout);



 /**
  * @brief  �жϽ��մ������ݣ�����ʹ�ã�
  * @param  uart_handle : ָ�򴮿��豸�����ָ��.
  * @param  pData       : ���ݻ������׵�ַ.
  * @param  size        : �������ݵĳ���
  * @retval 0 �������ݳɹ�
  */
int uart_data_rx_int(uart_handle_t uart_handle, uint8_t *pData, uint16_t size, uint32_t Timeout);

 /**
  * @brief  �жϷ��ʹ������ݣ� ����ʹ��
  * @param  uart_handle : ָ�򴮿��豸�����ָ��.
  * @param  pData       : ���ݻ������׵�ַ.
  * @param  size        : �������ݵĳ���
  * @retval 0 �жϷ������ݳɹ�
  */
int uart_data_tx_int(uart_handle_t uart_handle, uint8_t *pData, uint16_t size, uint32_t Timeout);


/**
  * @brief  ע�ᴮ���¼��ص�����
  * @param  uart_handle : ָ�򴮿��豸�����ָ��.
  * @param  cb          : �ص�����.
  * @param  p_arg       : �ص���������
  * @retval None
  */
void uart_event_registercb(uart_handle_t uart_handle, uart_cb cb, void *p_arg);


/**
  * @brief  ���ô����¼�
  * @param  uart_handle : ָ�򴮿��豸�����ָ��.
  * @param  uart_event  : �����¼�.
  * @retval None
  */
void uart_event_set (uart_handle_t uart_handle,int uart_event);

/**
  * @brief  �жϵ�ǰ�Ĵ����¼��Ƿ���
  * @param  uart_handle : ָ�򴮿��豸�����ָ��.
  * @param  uart_event  : �����¼�.
  * @retval 0 û�д����¼�����  ��0 ��ǰ�����¼��Ѿ�����
  */
int uart_event_get (uart_handle_t uart_handle, int uart_event);

/**
  * @brief  ��������¼�
  * @param  uart_handle : ָ�򴮿��豸�����ָ��.
  * @param  uart_event  : �����¼�.
  * @retval None
  */
void uart_event_clr (uart_handle_t uart_handle, int uart_event);

 /**
  * @brief  �����¼���ѯ
  * @param  uart_handle : ָ�򴮿��豸�����ָ��.
  * @retval None
  */
void uart_event_poll(uart_handle_t uart_handle);

#endif
