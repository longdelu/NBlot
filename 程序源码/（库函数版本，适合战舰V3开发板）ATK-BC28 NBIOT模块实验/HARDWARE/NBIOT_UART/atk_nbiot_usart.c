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
 * @brief  ���ڳ�ʼ��
 * @param  bound : ������.
 * @retval �����豸�����ָ��
 */
uart_dev_t *atk_nbiot_uart_init (u32 bound)
{   
    int err = 0;
  
     //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    //ʹ��USART3��GPIOBʱ��  
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
    USART_DeInit(USART3);  //��λ����3  
  
    //USART3_TX   GPIOB10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    //�����������
    GPIO_Init(GPIOB, &GPIO_InitStructure);
   
    //USART3_RX      GPIOB11��ʼ��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOB, &GPIO_InitStructure);                

    //Usart3 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;//��ռ���ȼ�1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;        //�����ȼ�2
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            //IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);    //����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
    //USART ��ʼ������
    USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;    //�շ�ģʽ

    USART_Init(USART3, &USART_InitStructure);     //��ʼ������3
  
    //��ʼ���ճ�ʱ����������ַ�֮��Ľ���ʱ�䳬��10msû�е�����������Ϊ�������
    atk_soft_timer_init(&uart_dev.uart_rx_timer, __lpuart_rx_timeout_cb, &uart_dev, 10, 0);     
    
    //��ʼ�����ջ��λ�����
    err = atk_ring_buf_init(&g_uart_ring_buf);
    uart_dev.p_huart    = USART3;
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
     
    //���rx�жϱ��
    USART_ClearITPendingBit(uart_dev.p_huart, USART_IT_RXNE);
      
    //���ilde�жϱ��
    USART_GetITStatus(uart_dev.p_huart, USART_IT_IDLE);
    USART_ReceiveData(uart_dev.p_huart);

    //���ܴ��ڷ����뷢������ж�
    USART_ITConfig(uart_dev.p_huart, USART_IT_TXE, DISABLE);
    USART_ITConfig(uart_dev.p_huart, USART_IT_TC,  DISABLE);    
     
    USART_ITConfig(uart_dev.p_huart, USART_IT_RXNE, ENABLE);  //�������ڽ����ж�
    USART_ITConfig(uart_dev.p_huart, USART_IT_IDLE, ENABLE);  //���������ж� 
        
    //ʹ�ܴ���
    USART_Cmd(uart_dev.p_huart, ENABLE);                    
       
    return &uart_dev;   
}

 /**
  * @brief  ���ڽ��ճ�ʱ�ص�����
  */
static void __lpuart_rx_timeout_cb (void *p_arg)
{
    
    uart_dev_t *p_lpuart_dev  = (uart_dev_t *)p_arg;
    
    USART_TypeDef *p_huart = p_lpuart_dev->p_huart;

#if !UART_ANY_DATA_LEN_RECV 
 
    /* Disable the UART Parity Error Interrupt and RXNE interrupts */

    CLEAR_BIT(p_huart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE));
    
    /* Disable the UART Error Interrupt: (Frame error, noise error, overrun error) */
    CLEAR_BIT(p_huart->Instance->CR3, USART_CR3_EIE);     
    
#endif //endif  UART_ANY_DATA_LEN_RECV
    
            
    //�������жϱ��
    USART_GetITStatus(p_huart, USART_IT_ORE);
    USART_ReceiveData(p_huart);
                                  
    //���֡�����жϱ��
    USART_GetITStatus(p_huart, USART_IT_FE);
    USART_ReceiveData(p_huart);
                                       
    //���У���жϱ��
    USART_GetITStatus(p_huart, USART_IT_PE);
    USART_ReceiveData(p_huart);
    
    //��������жϱ��
    USART_GetITStatus(p_huart, USART_IT_NE);
    USART_ReceiveData(p_huart);    
                                              
    //���rx�жϱ��
    USART_ClearITPendingBit(p_huart, USART_IT_RXNE);
    
    //���ilde�жϱ��
    USART_GetITStatus(p_huart, USART_IT_IDLE);
    USART_ReceiveData(p_huart);

    //���¿��������ж� 
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);  //�������ڽ����ж�
    
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
    
    USART_TypeDef *p_huart = p_lpuart_dev->p_huart;
    
    /* Disable the UART Transmit Data Register Empty Interrupt */
    CLEAR_BIT(p_huart->CR1, USART_CR1_TXEIE);
    
    /* Disable the UART Transmit Complete Interrupt */
    CLEAR_BIT(p_huart->CR1, USART_CR1_TCIE);          

        
    //���ô��ڷ��ͳ�ʱ�¼�
    uart_event_set(p_lpuart_dev, UART_TX_TIMEOUT_EVENT);    
}

#ifdef UART_ANY_DATA_LEN_RECV  

int __uart_ilde_is_enable(USART_TypeDef *p_huart)
{
  
     return (p_huart->CR1 & 0x00000010);
}

 /**
  * @brief  �����жϴ�����
  */
void USART3_IRQHandler(void)                    
{ 
    
    if ((USART_GetITStatus(uart_dev.p_huart, USART_IT_RXNE) != RESET))  
    { 
      
        //������д�뻷�λ�����
        atk_ring_buf_write(&g_uart_ring_buf, uart_dev.p_huart->DR);  
                      
        //�յ�һ�����ݣ����ó�ʱΪ500ms 
        atk_soft_timer_timeout_change(&uart_dev.uart_rx_timer, 500);
               
        // �յ����ݱ����������ڳ�ʱʱ����������ɣ�ֹͣ���ͳ�ʱ  
        atk_soft_timer_stop(&uart_dev.uart_tx_timer);
                                   
    }
    
    //ilde�жϲ���������ǰ����֡���ս���
    if ((USART_GetITStatus(uart_dev.p_huart, USART_IT_IDLE)!=RESET))  
    {        
        //����ǿ����ж�һ��ʼ��ʹ�ܵ�
        if (__uart_ilde_is_enable(uart_dev.p_huart)!=RESET) 
        {
           
            //�����ڳ�ʱʱ����������ɣ�ֹͣ���ճ�ʱ  
            atk_soft_timer_stop(&uart_dev.uart_rx_timer);
           
            //���ô��ڽ�������¼�
            uart_event_set(&uart_dev, UART_RX_EVENT); 
         
            #if !UART_ANY_DATA_LEN_RECV 
             
                /* Disable the UART Parity Error Interrupt and RXNE interrupts */ 
                CLEAR_BIT(nbiot_uart.Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE));
              
                /* Disable the UART Error Interrupt: (Frame error, noise error, overrun error) */
                CLEAR_BIT(nbiot_uart.Instance->CR3, USART_CR3_EIE);     
                
            #endif //endif    UART_ANY_DATA_LEN_RECV
                                            
            //�������жϱ��
            USART_GetITStatus(uart_dev.p_huart, USART_IT_ORE);
            USART_ReceiveData(uart_dev.p_huart);
                                          
            //���֡�����жϱ��
            USART_GetITStatus(uart_dev.p_huart, USART_IT_FE);
            USART_ReceiveData(uart_dev.p_huart);
                                               
            //���У���жϱ��
            USART_GetITStatus(uart_dev.p_huart, USART_IT_PE);
            USART_ReceiveData(uart_dev.p_huart);
            
            //��������жϱ��
            USART_GetITStatus(uart_dev.p_huart, USART_IT_NE);
            USART_ReceiveData(uart_dev.p_huart);    
                                                      
            //���rx�жϱ��
            USART_ClearITPendingBit(uart_dev.p_huart, USART_IT_RXNE);
            

            //���¿��������ж� 
            USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);  //�������ڽ����ж�              
         
         
        } 

        //���ilde�жϱ��
        USART_GetITStatus(uart_dev.p_huart, USART_IT_IDLE);
        USART_ReceiveData(uart_dev.p_huart);                              
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
    uint32_t i = 0;
        
    if(size == 0 || pData == NULL)
    {
       return -1;
    }
           
    //��ʼ��ʱ����
    atk_soft_timer_init(&uart_handle->uart_tx_timer, __lpuart_tx_timeout_cb, uart_handle, Timeout, 0); 
    
    //���÷��ͳ�ʱ��Ӧ�¼�
    atk_soft_timer_start(&uart_handle->uart_tx_timer); 
    
    for (i = 0; i < size; i ++)
    {  
         //�ȴ���һ�����ݷ������ 
         while((uart_handle->p_huart->SR&0X40)==0);
         //��������
         USART_SendData(uart_handle->p_huart, pData[i]);
    } 

    //���ô��ڷ�������¼�
    uart_event_set(&uart_dev, UART_TX_EVENT);
    
    return 0;    
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






