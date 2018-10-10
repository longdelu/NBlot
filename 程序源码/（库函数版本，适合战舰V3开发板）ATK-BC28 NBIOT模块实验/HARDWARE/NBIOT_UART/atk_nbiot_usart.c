#include "atk_nbiot_usart.h"
#include "atk_delay.h"
#include "stdio.h"

//////////////////////////////////////////////////////////////////////////////////     
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F429开发板
//串口1初始化           
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2015/9/7
//版本：V1.5
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//********************************************************************************

/**
  * @brief  UART设备结构体变量
  */
static uart_dev_t uart_dev;


/**
  * @brief  创建一个uart ringbuff 的缓冲区
  */
static atk_ring_buf_t g_uart_ring_buf;  


/**
  * @brief  函数提前声明
  */
static void __lpuart_rx_timeout_cb (void *p_arg);


/**
  * @brief  注册串口事件回调函数
  * @param  uart_handle : 指向串口设备句柄的指针.
  * @param  cb          : 回调函数.
  * @param  p_arg       : 回调函数参数
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
  * @brief  设置串口事件
  * @param  uart_handle : 指向串口设备句柄的指针.
  * @param  uart_event  : 串口事件.
  * @retval None
  */
void uart_event_set (uart_handle_t uart_handle, int uart_event)
{ 
    uart_handle->uart_event |= uart_event;   
}


/**
  * @brief  判断当前的串口事件是否发生
  * @param  uart_handle : 指向串口设备句柄的指针.
  * @param  uart_event  : 串口事件.
  * @retval 0 没有串口事件发生  非0 当前串口事件已经发生
  */
int uart_event_get (uart_handle_t uart_handle, int uart_event)
{ 
    return (uart_handle->uart_event & uart_event); 
}


/**
  * @brief  清除串口事件
  * @param  uart_handle : 指向串口设备句柄的指针.
  * @param  uart_event  : 串口事件.
  * @retval None
  */
void uart_event_clr (uart_handle_t uart_handle, int uart_event)
{ 
    uart_handle->uart_event &= ~uart_event;
}

 /**
  * @brief  从UART接收缓存里读取指定长度的数据，并释放占用的空间
  * @param  uart_handle : 指向串口设备句柄的指针.
  * @param  data        : 数据缓冲区首地址.
  * @param  len         : 读取数据的长度
  * @retval 0 读取串口数据成功 -1 读取串口数据
  */
int uart_ring_buf_read(uart_handle_t uart_handle, uint8_t *data, int len)
{
    int ret = 0;
    
    ret = atk_ring_buf_size_read(uart_handle->p_uart_ring_buff, data, len);
     
    return ret;
}


 /**
  * @brief  获取串口环形缓冲区中有效数据的个数
  * @param  uart_handle : 指向串口设备句柄的指针.
  * @retval 缓冲区实际有效个数
  */
int uart_ring_buf_avail_len(uart_handle_t uart_handle)
{
    return atk_ring_buf_avail_len(uart_handle->p_uart_ring_buff);
}

 /**
  * @brief  写入uart接收缓存里指定长度的数据，并占用的空间
  * @param  uart_handle : 指向串口设备句柄的指针.
  * @param  data        : 数据缓冲区首地址.
  * @param  len         : 写入数据的长度
  * @retval 0 写入串口数据成功 -1 写入串口数据
  */
int uart_ring_buf_write(uart_handle_t uart_handle, uint8_t *data, int len)
{
    int ret = 0;
    
    ret = atk_ring_buf_size_write(uart_handle->p_uart_ring_buff, data, len);
     
    return ret;
}



/**
 * @brief  串口初始化
 * @param  bound : 波特率.
 * @retval 串口设备句柄的指针
 */
uart_dev_t *atk_nbiot_uart_init (u32 bound)
{   
    int err = 0;
  
     //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    //使能USART3，GPIOB时钟  
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
    USART_DeInit(USART3);  //复位串口3  
  
    //USART3_TX   GPIOB10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    //复用推挽输出
    GPIO_Init(GPIOB, &GPIO_InitStructure);
   
    //USART3_RX      GPIOB11初始化
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);                

    //Usart3 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;//抢占优先级1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;        //子优先级2
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);    //根据指定的参数初始化VIC寄存器
  
    //USART 初始化设置
    USART_InitStructure.USART_BaudRate = bound;//串口波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;    //收发模式

    USART_Init(USART3, &USART_InitStructure);     //初始化串口3
  
    //初始接收超时，如果两个字符之间的接收时间超过10ms没有到来，可以认为接收完成
    atk_soft_timer_init(&uart_dev.uart_rx_timer, __lpuart_rx_timeout_cb, &uart_dev, 10, 0);     
    
    //初始化接收环形缓冲区
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
     
    //清除rx中断标记
    USART_ClearITPendingBit(uart_dev.p_huart, USART_IT_RXNE);
      
    //清除ilde中断标记
    USART_GetITStatus(uart_dev.p_huart, USART_IT_IDLE);
    USART_ReceiveData(uart_dev.p_huart);

    //禁能串口发送与发送完成中断
    USART_ITConfig(uart_dev.p_huart, USART_IT_TXE, DISABLE);
    USART_ITConfig(uart_dev.p_huart, USART_IT_TC,  DISABLE);    
     
    USART_ITConfig(uart_dev.p_huart, USART_IT_RXNE, ENABLE);  //开启串口接收中断
    USART_ITConfig(uart_dev.p_huart, USART_IT_IDLE, ENABLE);  //开启空闲中断 
        
    //使能串口
    USART_Cmd(uart_dev.p_huart, ENABLE);                    
       
    return &uart_dev;   
}

 /**
  * @brief  串口接收超时回调函数
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
    
            
    //清除溢出中断标记
    USART_GetITStatus(p_huart, USART_IT_ORE);
    USART_ReceiveData(p_huart);
                                  
    //清除帧错误中断标记
    USART_GetITStatus(p_huart, USART_IT_FE);
    USART_ReceiveData(p_huart);
                                       
    //清除校验中断标记
    USART_GetITStatus(p_huart, USART_IT_PE);
    USART_ReceiveData(p_huart);
    
    //清除噪声中断标记
    USART_GetITStatus(p_huart, USART_IT_NE);
    USART_ReceiveData(p_huart);    
                                              
    //清除rx中断标记
    USART_ClearITPendingBit(p_huart, USART_IT_RXNE);
    
    //清除ilde中断标记
    USART_GetITStatus(p_huart, USART_IT_IDLE);
    USART_ReceiveData(p_huart);

    //重新开启接收中断 
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);  //开启串口接收中断
    
    printf("rx timer happen\r\n");    
          
    //设置串口接收超时事件
    uart_event_set(p_lpuart_dev, UART_RX_TIMEOUT_EVENT);    
}


 /**
  * @brief  串口发送超时回调函数
  */
static void __lpuart_tx_timeout_cb (void *p_arg)
{
    
    uart_dev_t *p_lpuart_dev  = (uart_dev_t *)p_arg;
    
    USART_TypeDef *p_huart = p_lpuart_dev->p_huart;
    
    /* Disable the UART Transmit Data Register Empty Interrupt */
    CLEAR_BIT(p_huart->CR1, USART_CR1_TXEIE);
    
    /* Disable the UART Transmit Complete Interrupt */
    CLEAR_BIT(p_huart->CR1, USART_CR1_TCIE);          

        
    //设置串口发送超时事件
    uart_event_set(p_lpuart_dev, UART_TX_TIMEOUT_EVENT);    
}

#ifdef UART_ANY_DATA_LEN_RECV  

int __uart_ilde_is_enable(USART_TypeDef *p_huart)
{
  
     return (p_huart->CR1 & 0x00000010);
}

 /**
  * @brief  串口中断处理函数
  */
void USART3_IRQHandler(void)                    
{ 
    
    if ((USART_GetITStatus(uart_dev.p_huart, USART_IT_RXNE) != RESET))  
    { 
      
        //把数据写入环形缓冲区
        atk_ring_buf_write(&g_uart_ring_buf, uart_dev.p_huart->DR);  
                      
        //收到一个数据，重置超时为500ms 
        atk_soft_timer_timeout_change(&uart_dev.uart_rx_timer, 500);
               
        // 收到数据表明，发送在超时时间内正常完成，停止发送超时  
        atk_soft_timer_stop(&uart_dev.uart_tx_timer);
                                   
    }
    
    //ilde中断产生，代表当前数据帧接收结束
    if ((USART_GetITStatus(uart_dev.p_huart, USART_IT_IDLE)!=RESET))  
    {        
        //如果是空闲中断一开始是使能的
        if (__uart_ilde_is_enable(uart_dev.p_huart)!=RESET) 
        {
           
            //接收在超时时间内正常完成，停止接收超时  
            atk_soft_timer_stop(&uart_dev.uart_rx_timer);
           
            //设置串口接收完成事件
            uart_event_set(&uart_dev, UART_RX_EVENT); 
         
            #if !UART_ANY_DATA_LEN_RECV 
             
                /* Disable the UART Parity Error Interrupt and RXNE interrupts */ 
                CLEAR_BIT(nbiot_uart.Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE));
              
                /* Disable the UART Error Interrupt: (Frame error, noise error, overrun error) */
                CLEAR_BIT(nbiot_uart.Instance->CR3, USART_CR3_EIE);     
                
            #endif //endif    UART_ANY_DATA_LEN_RECV
                                            
            //清除溢出中断标记
            USART_GetITStatus(uart_dev.p_huart, USART_IT_ORE);
            USART_ReceiveData(uart_dev.p_huart);
                                          
            //清除帧错误中断标记
            USART_GetITStatus(uart_dev.p_huart, USART_IT_FE);
            USART_ReceiveData(uart_dev.p_huart);
                                               
            //清除校验中断标记
            USART_GetITStatus(uart_dev.p_huart, USART_IT_PE);
            USART_ReceiveData(uart_dev.p_huart);
            
            //清除噪声中断标记
            USART_GetITStatus(uart_dev.p_huart, USART_IT_NE);
            USART_ReceiveData(uart_dev.p_huart);    
                                                      
            //清除rx中断标记
            USART_ClearITPendingBit(uart_dev.p_huart, USART_IT_RXNE);
            

            //重新开启接收中断 
            USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);  //开启串口接收中断              
         
         
        } 

        //清除ilde中断标记
        USART_GetITStatus(uart_dev.p_huart, USART_IT_IDLE);
        USART_ReceiveData(uart_dev.p_huart);                              
    }   
}   



#else 


#endif   //end if UART_ANY_DATA_LEN_RECV 


 /**
  * @brief  轮询发送串口数据
  * @param  uart_handle : 指向串口设备句柄的指针.
  * @param  pData       : 数据缓冲区首地址.
  * @param  size        : 发送数据的长度
  * @retval 0 中断发送数据成功
  */
int uart_data_tx_poll(uart_handle_t uart_handle, uint8_t *pData,uint16_t size, uint32_t Timeout)
{     
    uint32_t i = 0;
        
    if(size == 0 || pData == NULL)
    {
       return -1;
    }
           
    //初始超时计算
    atk_soft_timer_init(&uart_handle->uart_tx_timer, __lpuart_tx_timeout_cb, uart_handle, Timeout, 0); 
    
    //设置发送超时响应事件
    atk_soft_timer_start(&uart_handle->uart_tx_timer); 
    
    for (i = 0; i < size; i ++)
    {  
         //等待上一次数据发送完成 
         while((uart_handle->p_huart->SR&0X40)==0);
         //发送数据
         USART_SendData(uart_handle->p_huart, pData[i]);
    } 

    //设置串口发送完成事件
    uart_event_set(&uart_dev, UART_TX_EVENT);
    
    return 0;    
}


 /**
  * @brief  串口事件轮询
  * @param  uart_handle : 指向串口设备句柄的指针.
  * @retval None
  */
void uart_event_poll(uart_handle_t uart_handle)
{ 
    //回调注册进来的串口事件处理函数 
    uart_dev.uart_cb(uart_handle->p_arg);
    
}






