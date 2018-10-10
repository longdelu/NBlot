#include "atk_nbiot_usart.h"
#include "atk_delay.h"
#include "stdio.h"

//////////////////////////////////////////////////////////////////////////////////     
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F429������
//����1��ʼ��           
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2015/9/7
//�汾��V1.5
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//********************************************************************************

/**
  * @brief  UART�豸�ṹ�����
  */
static uart_dev_t uart_dev;


/**
  * @brief  ����һ��uart ringbuff �Ļ�����
  */
static atk_ring_buf_t g_uart_ring_buf;  

/**
  * @brief  HAL�⴮���豸�ṹ��
  */
static UART_HandleTypeDef nbiot_uart;

/**
  * @brief  ������ǰ����
  */
static void __lpuart_rx_timeout_cb (void *p_arg);


/**
  * @brief  ע�ᴮ���¼��ص�����
  * @param  uart_handle : ָ�򴮿��豸�����ָ��.
  * @param  cb          : �ص�����.
  * @param  p_arg       : �ص���������
  * @retval None
  */
void uart_event_registercb(uart_handle_t uart_handle, uart_cb cb, void *p_arg)
{  
    if(cb != 0)
    {
        uart_handle->uart_cb  = cb;
        uart_handle->p_arg    = p_arg;
    }
}


/**
  * @brief  ���ô����¼�
  * @param  uart_handle : ָ�򴮿��豸�����ָ��.
  * @param  uart_event  : �����¼�.
  * @retval None
  */
void uart_event_set (uart_handle_t uart_handle, int uart_event)
{ 
    uart_handle->uart_event |= uart_event;   
}


/**
  * @brief  �жϵ�ǰ�Ĵ����¼��Ƿ���
  * @param  uart_handle : ָ�򴮿��豸�����ָ��.
  * @param  uart_event  : �����¼�.
  * @retval 0 û�д����¼�����  ��0 ��ǰ�����¼��Ѿ�����
  */
int uart_event_get (uart_handle_t uart_handle, int uart_event)
{ 
    return (uart_handle->uart_event & uart_event); 
}


/**
  * @brief  ��������¼�
  * @param  uart_handle : ָ�򴮿��豸�����ָ��.
  * @param  uart_event  : �����¼�.
  * @retval None
  */
void uart_event_clr (uart_handle_t uart_handle, int uart_event)
{ 
    uart_handle->uart_event &= ~uart_event;
}

 /**
  * @brief  ��UART���ջ������ȡָ�����ȵ����ݣ����ͷ�ռ�õĿռ�
  * @param  uart_handle : ָ�򴮿��豸�����ָ��.
  * @param  data        : ���ݻ������׵�ַ.
  * @param  len         : ��ȡ���ݵĳ���
  * @retval 0 ��ȡ�������ݳɹ� -1 ��ȡ��������
  */
int uart_ring_buf_read(uart_handle_t uart_handle, uint8_t *data, int len)
{
    int ret = 0;
    
    ret = atk_ring_buf_size_read(uart_handle->p_uart_ring_buff, data, len);
     
    return ret;
}


 /**
  * @brief  ��ȡ���ڻ��λ���������Ч���ݵĸ���
  * @param  uart_handle : ָ�򴮿��豸�����ָ��.
  * @retval ������ʵ����Ч����
  */
int uart_ring_buf_avail_len(uart_handle_t uart_handle)
{
    return atk_ring_buf_avail_len(uart_handle->p_uart_ring_buff);
}

 /**
  * @brief  д��uart���ջ�����ָ�����ȵ����ݣ���ռ�õĿռ�
  * @param  uart_handle : ָ�򴮿��豸�����ָ��.
  * @param  data        : ���ݻ������׵�ַ.
  * @param  len         : д�����ݵĳ���
  * @retval 0 д�봮�����ݳɹ� -1 д�봮������
  */
int uart_ring_buf_write(uart_handle_t uart_handle, uint8_t *data, int len)
{
    int ret = 0;
    
    ret = atk_ring_buf_size_write(uart_handle->p_uart_ring_buff, data, len);
     
    return ret;
}


 /**
  * @brief  UART�ײ��ʼ����ʱ��ʹ�ܣ��������ã��ж�����
  * @param  huart       :  HAL�⴮���豸�ṹ��
  * @param  data        :  ���ݻ������׵�ַ.
  * @param  len         :  д�����ݵĳ���
  * @retval None
  */
void HAL_USART3_MspInit(UART_HandleTypeDef *huart)
{
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_Initure;
    
    if(huart->Instance==USART3)//����Ǵ���1�����д���1 MSP��ʼ��
    {
        __HAL_RCC_USART3_CLK_ENABLE();
        
        // GPIO Ports Clock Enable 
        __HAL_RCC_GPIOB_CLK_ENABLE();
       
        GPIO_Initure.Pin=GPIO_PIN_10|GPIO_PIN_11;                //PB10��PB11
        GPIO_Initure.Mode=GPIO_MODE_AF_PP;                       //�����������
        GPIO_Initure.Pull=GPIO_PULLUP;                           //����
        GPIO_Initure.Speed=GPIO_SPEED_FAST;                      //����
        GPIO_Initure.Alternate=GPIO_AF7_USART3;                  //����ΪUSART3
        HAL_GPIO_Init(GPIOB,&GPIO_Initure);                      //��ʼ��PB10,��PB11              
    }

}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
static void _Error_Handler(char *file, int line)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    while(1)
    {
      
    }
    
    /* USER CODE END Error_Handler_Debug */
}


/**
 * @brief  ���ڳ�ʼ��
 * @param  bound : ������.
 * @retval �����豸�����ָ��
 */
uart_dev_t *atk_nbiot_uart_init (u32 bound)
{      
    int err = 0;
    
    nbiot_uart.Instance = USART3;
    nbiot_uart.Init.BaudRate = bound;
    nbiot_uart.Init.WordLength = UART_WORDLENGTH_8B;
    nbiot_uart.Init.StopBits = UART_STOPBITS_1;
    nbiot_uart.Init.Parity = UART_PARITY_NONE;
    nbiot_uart.Init.Mode = UART_MODE_TX_RX;
    nbiot_uart.Init.HwFlowCtl = UART_HWCONTROL_NONE; 

    HAL_USART3_MspInit(&nbiot_uart); 
    
       
    if (HAL_UART_Init(&nbiot_uart) != HAL_OK)
    {
       _Error_Handler(__FILE__, __LINE__);
    }
    
    //��ʼ���ճ�ʱ����������ַ�֮��Ľ���ʱ�䳬��10msû�е�����������Ϊ�������
    atk_soft_timer_init(&uart_dev.uart_rx_timer, __lpuart_rx_timeout_cb, &uart_dev, 10, 0);     
    
    //��ʼ�����ջ��λ�����
    err = atk_ring_buf_init(&g_uart_ring_buf);
   
    //�����������豸�ṹ��    
    uart_dev.p_huart    = &nbiot_uart; 
    uart_dev.uart_event = UART_NONE_EVENT;   
    uart_dev.bound      = bound; 
    
    if (err == 0) 
    {
        uart_dev.p_uart_ring_buff  = &g_uart_ring_buf;           
    }
    else
    {
        uart_dev.p_uart_ring_buff  = NULL;   
    }


    //���ܴ���
     __HAL_UART_DISABLE(&nbiot_uart);    

    __HAL_UART_DISABLE_IT(&nbiot_uart, UART_IT_TXE);  //��ֹ�����ж� 
    __HAL_UART_DISABLE_IT(&nbiot_uart, UART_IT_TC);   //���ܷ�������ж�   
    
    //���rx�жϱ��
    __HAL_UART_CLEAR_FLAG(&nbiot_uart, UART_FLAG_RXNE);
      
    //���ilde�жϱ��
    __HAL_UART_CLEAR_IDLEFLAG(&nbiot_uart); 
     
    __HAL_UART_ENABLE_IT(&nbiot_uart, UART_IT_RXNE);    //���������ж�      
    __HAL_UART_ENABLE_IT(&nbiot_uart, UART_IT_IDLE);    //���������ж� 
    

    HAL_NVIC_EnableIRQ(USART3_IRQn);                //ʹ��USART3�ж�ͨ��
    HAL_NVIC_SetPriority(USART3_IRQn,3,3);          //��ռ���ȼ�3�������ȼ�3
    
    //ʹ�ܴ���
     __HAL_UART_ENABLE(&nbiot_uart);   
       
    return  &uart_dev;   
}

 /**
  * @brief  ���ڽ��ճ�ʱ�ص�����
  */
static void __lpuart_rx_timeout_cb (void *p_arg)
{
    
    uart_dev_t *p_lpuart_dev  = (uart_dev_t *)p_arg;
    
    UART_HandleTypeDef *p_huart = p_lpuart_dev->p_huart;

#if !UART_ANY_DATA_LEN_RECV 
 
    /* Disable the UART Parity Error Interrupt and RXNE interrupts */
#if defined(USART_CR1_FIFOEN)
    CLEAR_BIT(p_huart->Instance->CR1, (USART_CR1_RXNEIE_RXFNEIE | USART_CR1_PEIE));
#else
    CLEAR_BIT(p_huart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE));
#endif //endif  USART_CR1_FIFOEN
    
    /* Disable the UART Error Interrupt: (Frame error, noise error, overrun error) */
    CLEAR_BIT(p_huart->Instance->CR3, USART_CR3_EIE);     
    
#endif //endif    UART_ANY_DATA_LEN_RECV
    
    //reset the p_huart->RxState
    p_huart->State= HAL_UART_STATE_READY; 

    //����������
    READ_REG(p_huart->Instance->DR);
             
    //�������жϱ��
    __HAL_UART_CLEAR_OREFLAG(p_huart); 
                                  
    //���֡�����жϱ��
    __HAL_UART_CLEAR_FEFLAG(p_huart); 
    
    //���NE�����жϱ��
    __HAL_UART_CLEAR_NEFLAG(p_huart);
                                       
    //���У���жϱ��
    __HAL_UART_CLEAR_PEFLAG(p_huart); 
                                              
    //��������ж�
    __HAL_UART_CLEAR_FLAG(&nbiot_uart, UART_FLAG_RXNE);
    
    //���ilde�жϱ��
    __HAL_UART_CLEAR_IDLEFLAG(&nbiot_uart); 

    //���¿��������ж� 
    __HAL_UART_ENABLE_IT(p_huart, UART_IT_RXNE);  
    
    printf("rx timer happen\r\n");    
          
    //���ô��ڽ��ճ�ʱ�¼�
    uart_event_set(p_lpuart_dev, UART_RX_TIMEOUT_EVENT);    
}


 /**
  * @brief  ���ڷ��ͳ�ʱ�ص�����
  */
static void __lpuart_tx_timeout_cb (void *p_arg)
{
    
    uart_dev_t *p_lpuart_dev  = (uart_dev_t *)p_arg;
    
    UART_HandleTypeDef *p_huart = p_lpuart_dev->p_huart;
    
      /* Disable the UART Transmit Data Register Empty Interrupt */
#if defined(USART_CR1_FIFOEN)
      CLEAR_BIT(p_huart->Instance->CR1, USART_CR1_TXEIE_TXFNFIE);
#else
      CLEAR_BIT(p_huart->Instance->CR1, USART_CR1_TXEIE);
#endif
    
    /* Disable the UART Transmit Complete Interrupt */
    CLEAR_BIT(p_huart->Instance->CR1, USART_CR1_TCIE);          

    //reset the p_huart->RxState
    p_huart->State= HAL_UART_STATE_READY;  
        
    //���ô��ڷ��ͳ�ʱ�¼�
    uart_event_set(p_lpuart_dev, UART_TX_TIMEOUT_EVENT);    
}

#ifdef UART_ANY_DATA_LEN_RECV  

 /**
  * @brief  �����жϴ�����
  */
void USART3_IRQHandler(void)                    
{ 
    
    if ((__HAL_UART_GET_FLAG(&nbiot_uart, UART_FLAG_RXNE) != RESET))  
    {      
        //������д�뻷�λ�����
        atk_ring_buf_write(&g_uart_ring_buf, nbiot_uart.Instance->DR);  
                      
        //�յ�һ�����ݣ����ó�ʱΪ500ms 
        atk_soft_timer_timeout_change(&uart_dev.uart_rx_timer, 500);
               
        // �յ����ݱ����������ڳ�ʱʱ����������ɣ�ֹͣ���ͳ�ʱ  
        atk_soft_timer_stop(&uart_dev.uart_tx_timer);                                   
    }
    
    //ilde�жϲ���������ǰ����֡���ս���
    if ((__HAL_UART_GET_FLAG(&nbiot_uart, UART_FLAG_IDLE)!=RESET))  
    {        
        //����ǿ����ж�һ��ʼ��ʹ�ܵ�
        if (__HAL_UART_GET_IT_SOURCE(&nbiot_uart, UART_IT_IDLE)!=RESET) 
        {
           
           //�����ڳ�ʱʱ����������ɣ�ֹͣ���ճ�ʱ  
           atk_soft_timer_stop(&uart_dev.uart_rx_timer);
           
           //���ô��ڽ�������¼�
           uart_event_set(&uart_dev, UART_RX_EVENT); 
         
          #if !UART_ANY_DATA_LEN_RECV 
           
              /* Disable the UART Parity Error Interrupt and RXNE interrupts */
          #if defined(USART_CR1_FIFOEN)
              CLEAR_BIT(nbiot_uart.Instance->CR1, (USART_CR1_RXNEIE_RXFNEIE | USART_CR1_PEIE));
          #else
              CLEAR_BIT(nbiot_uart.Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE));
          #endif //endif  USART_CR1_FIFOEN
              
              /* Disable the UART Error Interrupt: (Frame error, noise error, overrun error) */
              CLEAR_BIT(nbiot_uart.Instance->CR3, USART_CR3_EIE);     
              
          #endif //endif    UART_ANY_DATA_LEN_RECV
              
              //reset the p_huart->RxState
              nbiot_uart.State= HAL_UART_STATE_READY; 
                           
              //���NE�����жϱ��
              __HAL_UART_CLEAR_NEFLAG(&nbiot_uart);
                                                                 
              //�������жϱ��
              __HAL_UART_CLEAR_OREFLAG(&nbiot_uart); 
                                          
              //���֡�����жϱ��
              __HAL_UART_CLEAR_FEFLAG(&nbiot_uart); 
                                                  
              //���У���жϱ��
              __HAL_UART_CLEAR_PEFLAG(&nbiot_uart); 
                                                      
              //��������ж�
              __HAL_UART_CLEAR_FLAG(&nbiot_uart, UART_FLAG_RXNE);
            
              //���¿��������ж� 
              __HAL_UART_ENABLE_IT(&nbiot_uart, UART_IT_RXNE);                

        } 

        //���ilde�жϱ��
        __HAL_UART_CLEAR_IDLEFLAG(&nbiot_uart);     
                            
    }      
}   

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART3)//����Ǵ���3
    {
       /* �����ڳ�ʱʱ����������ɣ�ֹͣ��ʱ */  
       atk_soft_timer_stop(&uart_dev.uart_tx_timer);
       
       //���ô��ڷ�������¼�
       uart_event_set(&uart_dev, UART_TX_EVENT);              
    }
}

#else 


#endif   //end if UART_ANY_DATA_LEN_RECV 


 /**
  * @brief  ��ѯ���ʹ�������
  * @param  uart_handle : ָ�򴮿��豸�����ָ��.
  * @param  pData       : ���ݻ������׵�ַ.
  * @param  size        : �������ݵĳ���
  * @retval 0 �жϷ������ݳɹ�
  */
int uart_data_tx_poll(uart_handle_t uart_handle, uint8_t *pData,uint16_t size, uint32_t Timeout)
{   
    int ret = 0;
    
    int tx_int_flag = 0;
    
    if(size == 0 || pData == NULL)
    {
       return -1;
    }
    
    tx_int_flag = __HAL_UART_GET_IT_SOURCE(uart_handle->p_huart, UART_IT_TXE);
    
    
    if (tx_int_flag) {
    
        __HAL_UART_DISABLE_IT(uart_handle->p_huart, UART_IT_TXE);    //���ܷ����ж�  
    }    
        
    /* �������ó�ʱΪ Timeout ms */ 
    ret = HAL_UART_Transmit(uart_handle->p_huart, pData, size, Timeout);
    
    
    if (ret == HAL_TIMEOUT) {
      
        //���ô��ڷ��ͳ�ʱ�¼�
        uart_event_set(uart_handle, UART_TX_TIMEOUT_EVENT); 
      
        return ret;
        
    } else if (ret == HAL_OK) {
        
        //���ô��ڷ�������¼�
        uart_event_set(uart_handle, UART_TX_EVENT);         
    }   
    
    if (tx_int_flag) {
    
        __HAL_UART_ENABLE_IT(uart_handle->p_huart, UART_IT_TXE);    //ʹ�ܷ����ж�     
    }
    
    
    /* ��ʼ��ʱ���� */
    atk_soft_timer_init(&uart_handle->uart_tx_timer, __lpuart_tx_timeout_cb, uart_handle, Timeout, 0); 
    atk_soft_timer_start(&uart_handle->uart_tx_timer);    
    
    return  ret;
}


 /**
  * @brief  ��ѯ���մ������ݣ�����ʹ�ã�
  * @param  uart_handle : ָ�򴮿��豸�����ָ��.
  * @param  pData       : ���ݻ������׵�ַ.
  * @param  size        : �������ݵĳ���
  * @retval 0 �������ݳɹ�
  */
int uart_data_rx_poll(uart_handle_t uart_handle, uint8_t *pData, uint16_t size, uint32_t Timeout)
{
    int ret = 0;
    
    int rx_int_flag = 0;
    
    if(size == 0 || pData == NULL)
    {
       return -1;
    }
       
    rx_int_flag = __HAL_UART_GET_IT_SOURCE(uart_handle->p_huart, UART_IT_RXNE);
    
    
    if (rx_int_flag) {
    
        __HAL_UART_DISABLE_IT(uart_handle->p_huart, UART_IT_RXNE);    //��ֹ�����ж�
    }
    
    /* �������ó�ʱΪ Timeout ms */
    ret = HAL_UART_Receive(uart_handle->p_huart, pData, size, Timeout);
    
    if (ret == HAL_TIMEOUT) {     
        //���ô��ڽ��ճ�ʱ�¼�
        uart_event_set(uart_handle, UART_RX_TIMEOUT_EVENT); 
        
    } else if (ret == HAL_OK) {
        
        //���ô��ڽ�������¼�
        uart_event_set(uart_handle, UART_RX_EVENT);         
    }
        
    if (rx_int_flag) {
        __HAL_UART_ENABLE_IT(uart_handle->p_huart, UART_IT_RXNE);    //���������ж�
    }
    
    return ret;
}
 

 /**
  * @brief  �жϽ��մ������ݣ�����ʹ�ã�
  * @param  uart_handle : ָ�򴮿��豸�����ָ��.
  * @param  pData       : ���ݻ������׵�ַ.
  * @param  size        : �������ݵĳ���
  * @retval 0 �������ݳɹ�
  */
int uart_data_rx_int(uart_handle_t uart_handle, uint8_t *pData, uint16_t size, uint32_t Timeout)
{
    int ret = 0;
    
    if(size == 0 || pData == NULL)
    {
       return - 1;
    }
         
    /* �������ó�ʱΪ HAL_MAX_DELAY ms */
    ret = HAL_UART_Receive_IT(uart_handle->p_huart, pData, size);
    
    /* ��ʼ��ʱ���� */
    atk_soft_timer_init(&uart_handle->uart_rx_timer, __lpuart_rx_timeout_cb, uart_handle, Timeout, 0); 
    atk_soft_timer_start(&uart_handle->uart_rx_timer);
             
    /* ͬ����ʱ��ʱ�� */
    while (HAL_UART_GetState(uart_handle->p_huart) != HAL_UART_STATE_READY);//�ȴ�����
               
    return ret;
}



 /**
  * @brief  �жϷ��ʹ������ݣ� ����ʹ��
  * @param  uart_handle : ָ�򴮿��豸�����ָ��.
  * @param  pData       : ���ݻ������׵�ַ.
  * @param  size        : �������ݵĳ���
  * @retval 0 �жϷ������ݳɹ�
  */
int uart_data_tx_int(uart_handle_t uart_handle, uint8_t *pData, uint16_t size, uint32_t Timeout)
{
    int ret = 0;
    
    if(size == 0 || pData == NULL)
    {
       return -1;
    }
    
    ret = HAL_UART_Transmit_IT(uart_handle->p_huart, pData, size);
    
    /* ��ʼ��ʱ���� */
    atk_soft_timer_init(&uart_handle->uart_tx_timer, __lpuart_tx_timeout_cb, uart_handle, Timeout, 0); 
    atk_soft_timer_start(&uart_handle->uart_tx_timer);
           
    /* ͬ����ʱ��ʱ�� */    
    while (HAL_UART_GetState(uart_handle->p_huart) != HAL_UART_STATE_READY);//�ȴ�����    
    return ret;
}

 /**
  * @brief  �����¼���ѯ
  * @param  uart_handle : ָ�򴮿��豸�����ָ��.
  * @retval None
  */
void uart_event_poll(uart_handle_t uart_handle)
{ 
    //�ص�ע������Ĵ����¼������� 
    uart_dev.uart_cb(uart_handle->p_arg);
    
}






