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
  * @brief  HAL库串口设备结构体
  */
static UART_HandleTypeDef nbiot_uart;

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
  * @brief  UART底层初始化，时钟使能，引脚配置，中断配置
  * @param  huart       :  HAL库串口设备结构体
  * @param  data        :  数据缓冲区首地址.
  * @param  len         :  写入数据的长度
  * @retval None
  */
void HAL_USART3_MspInit(UART_HandleTypeDef *huart)
{
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_Initure;
    
    if(huart->Instance==USART3)//如果是串口1，进行串口1 MSP初始化
    {
        __HAL_RCC_USART3_CLK_ENABLE();
        
        // GPIO Ports Clock Enable 
        __HAL_RCC_GPIOB_CLK_ENABLE();
       
        GPIO_Initure.Pin=GPIO_PIN_10|GPIO_PIN_11;                //PB10、PB11
        GPIO_Initure.Mode=GPIO_MODE_AF_PP;                       //复用推挽输出
        GPIO_Initure.Pull=GPIO_PULLUP;                           //上拉
        GPIO_Initure.Speed=GPIO_SPEED_FAST;                      //高速
        GPIO_Initure.Alternate=GPIO_AF7_USART3;                  //复用为USART3
        HAL_GPIO_Init(GPIOB,&GPIO_Initure);                      //初始化PB10,和PB11              
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
 * @brief  串口初始化
 * @param  bound : 波特率.
 * @retval 串口设备句柄的指针
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
    
    //初始接收超时，如果两个字符之间的接收时间超过10ms没有到来，可以认为接收完成
    atk_soft_timer_init(&uart_dev.uart_rx_timer, __lpuart_rx_timeout_cb, &uart_dev, 10, 0);     
    
    //初始化接收环形缓冲区
    err = atk_ring_buf_init(&g_uart_ring_buf);
   
    //填充这个串口设备结构体    
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


    //禁能串口
     __HAL_UART_DISABLE(&nbiot_uart);    

    __HAL_UART_DISABLE_IT(&nbiot_uart, UART_IT_TXE);  //禁止发送中断 
    __HAL_UART_DISABLE_IT(&nbiot_uart, UART_IT_TC);   //禁能发送完成中断   
    
    //清除rx中断标记
    __HAL_UART_CLEAR_FLAG(&nbiot_uart, UART_FLAG_RXNE);
      
    //清除ilde中断标记
    __HAL_UART_CLEAR_IDLEFLAG(&nbiot_uart); 
     
    __HAL_UART_ENABLE_IT(&nbiot_uart, UART_IT_RXNE);    //开启接收中断      
    __HAL_UART_ENABLE_IT(&nbiot_uart, UART_IT_IDLE);    //开启空闲中断 
    

    HAL_NVIC_EnableIRQ(USART3_IRQn);                //使能USART3中断通道
    HAL_NVIC_SetPriority(USART3_IRQn,3,3);          //抢占优先级3，子优先级3
    
    //使能串口
     __HAL_UART_ENABLE(&nbiot_uart);   
       
    return  &uart_dev;   
}

 /**
  * @brief  串口接收超时回调函数
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

    //清垃圾数据
    READ_REG(p_huart->Instance->DR);
             
    //清除溢出中断标记
    __HAL_UART_CLEAR_OREFLAG(p_huart); 
                                  
    //清除帧错误中断标记
    __HAL_UART_CLEAR_FEFLAG(p_huart); 
    
    //清除NE错误中断标记
    __HAL_UART_CLEAR_NEFLAG(p_huart);
                                       
    //清除校验中断标记
    __HAL_UART_CLEAR_PEFLAG(p_huart); 
                                              
    //清除接收中断
    __HAL_UART_CLEAR_FLAG(&nbiot_uart, UART_FLAG_RXNE);
    
    //清除ilde中断标记
    __HAL_UART_CLEAR_IDLEFLAG(&nbiot_uart); 

    //重新开启接收中断 
    __HAL_UART_ENABLE_IT(p_huart, UART_IT_RXNE);  
    
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
        
    //设置串口发送超时事件
    uart_event_set(p_lpuart_dev, UART_TX_TIMEOUT_EVENT);    
}

#ifdef UART_ANY_DATA_LEN_RECV  

 /**
  * @brief  串口中断处理函数
  */
void USART3_IRQHandler(void)                    
{ 
    
    if ((__HAL_UART_GET_FLAG(&nbiot_uart, UART_FLAG_RXNE) != RESET))  
    {      
        //把数据写入环形缓冲区
        atk_ring_buf_write(&g_uart_ring_buf, nbiot_uart.Instance->DR);  
                      
        //收到一个数据，重置超时为500ms 
        atk_soft_timer_timeout_change(&uart_dev.uart_rx_timer, 500);
               
        // 收到数据表明，发送在超时时间内正常完成，停止发送超时  
        atk_soft_timer_stop(&uart_dev.uart_tx_timer);                                   
    }
    
    //ilde中断产生，代表当前数据帧接收结束
    if ((__HAL_UART_GET_FLAG(&nbiot_uart, UART_FLAG_IDLE)!=RESET))  
    {        
        //如果是空闲中断一开始是使能的
        if (__HAL_UART_GET_IT_SOURCE(&nbiot_uart, UART_IT_IDLE)!=RESET) 
        {
           
           //接收在超时时间内正常完成，停止接收超时  
           atk_soft_timer_stop(&uart_dev.uart_rx_timer);
           
           //设置串口接收完成事件
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
                           
              //清除NE错误中断标记
              __HAL_UART_CLEAR_NEFLAG(&nbiot_uart);
                                                                 
              //清除溢出中断标记
              __HAL_UART_CLEAR_OREFLAG(&nbiot_uart); 
                                          
              //清除帧错误中断标记
              __HAL_UART_CLEAR_FEFLAG(&nbiot_uart); 
                                                  
              //清除校验中断标记
              __HAL_UART_CLEAR_PEFLAG(&nbiot_uart); 
                                                      
              //清除接收中断
              __HAL_UART_CLEAR_FLAG(&nbiot_uart, UART_FLAG_RXNE);
            
              //重新开启接收中断 
              __HAL_UART_ENABLE_IT(&nbiot_uart, UART_IT_RXNE);                

        } 

        //清除ilde中断标记
        __HAL_UART_CLEAR_IDLEFLAG(&nbiot_uart);     
                            
    }      
}   

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART3)//如果是串口3
    {
       /* 发送在超时时间内正常完成，停止超时 */  
       atk_soft_timer_stop(&uart_dev.uart_tx_timer);
       
       //设置串口发送完成事件
       uart_event_set(&uart_dev, UART_TX_EVENT);              
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
    int ret = 0;
    
    int tx_int_flag = 0;
    
    if(size == 0 || pData == NULL)
    {
       return -1;
    }
    
    tx_int_flag = __HAL_UART_GET_IT_SOURCE(uart_handle->p_huart, UART_IT_TXE);
    
    
    if (tx_int_flag) {
    
        __HAL_UART_DISABLE_IT(uart_handle->p_huart, UART_IT_TXE);    //禁能发送中断  
    }    
        
    /* 发送设置超时为 Timeout ms */ 
    ret = HAL_UART_Transmit(uart_handle->p_huart, pData, size, Timeout);
    
    
    if (ret == HAL_TIMEOUT) {
      
        //设置串口发送超时事件
        uart_event_set(uart_handle, UART_TX_TIMEOUT_EVENT); 
      
        return ret;
        
    } else if (ret == HAL_OK) {
        
        //设置串口发送完成事件
        uart_event_set(uart_handle, UART_TX_EVENT);         
    }   
    
    if (tx_int_flag) {
    
        __HAL_UART_ENABLE_IT(uart_handle->p_huart, UART_IT_TXE);    //使能发送中断     
    }
    
    
    /* 初始超时计算 */
    atk_soft_timer_init(&uart_handle->uart_tx_timer, __lpuart_tx_timeout_cb, uart_handle, Timeout, 0); 
    atk_soft_timer_start(&uart_handle->uart_tx_timer);    
    
    return  ret;
}


 /**
  * @brief  轮询接收串口数据（保留使用）
  * @param  uart_handle : 指向串口设备句柄的指针.
  * @param  pData       : 数据缓冲区首地址.
  * @param  size        : 接收数据的长度
  * @retval 0 接收数据成功
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
    
        __HAL_UART_DISABLE_IT(uart_handle->p_huart, UART_IT_RXNE);    //禁止接收中断
    }
    
    /* 接收设置超时为 Timeout ms */
    ret = HAL_UART_Receive(uart_handle->p_huart, pData, size, Timeout);
    
    if (ret == HAL_TIMEOUT) {     
        //设置串口接收超时事件
        uart_event_set(uart_handle, UART_RX_TIMEOUT_EVENT); 
        
    } else if (ret == HAL_OK) {
        
        //设置串口接收完成事件
        uart_event_set(uart_handle, UART_RX_EVENT);         
    }
        
    if (rx_int_flag) {
        __HAL_UART_ENABLE_IT(uart_handle->p_huart, UART_IT_RXNE);    //开启接收中断
    }
    
    return ret;
}
 

 /**
  * @brief  中断接收串口数据（保留使用）
  * @param  uart_handle : 指向串口设备句柄的指针.
  * @param  pData       : 数据缓冲区首地址.
  * @param  size        : 接收数据的长度
  * @retval 0 接收数据成功
  */
int uart_data_rx_int(uart_handle_t uart_handle, uint8_t *pData, uint16_t size, uint32_t Timeout)
{
    int ret = 0;
    
    if(size == 0 || pData == NULL)
    {
       return - 1;
    }
         
    /* 发送设置超时为 HAL_MAX_DELAY ms */
    ret = HAL_UART_Receive_IT(uart_handle->p_huart, pData, size);
    
    /* 初始超时计算 */
    atk_soft_timer_init(&uart_handle->uart_rx_timer, __lpuart_rx_timeout_cb, uart_handle, Timeout, 0); 
    atk_soft_timer_start(&uart_handle->uart_rx_timer);
             
    /* 同步超时的时间 */
    while (HAL_UART_GetState(uart_handle->p_huart) != HAL_UART_STATE_READY);//等待就绪
               
    return ret;
}



 /**
  * @brief  中断发送串口数据， 保留使用
  * @param  uart_handle : 指向串口设备句柄的指针.
  * @param  pData       : 数据缓冲区首地址.
  * @param  size        : 发送数据的长度
  * @retval 0 中断发送数据成功
  */
int uart_data_tx_int(uart_handle_t uart_handle, uint8_t *pData, uint16_t size, uint32_t Timeout)
{
    int ret = 0;
    
    if(size == 0 || pData == NULL)
    {
       return -1;
    }
    
    ret = HAL_UART_Transmit_IT(uart_handle->p_huart, pData, size);
    
    /* 初始超时计算 */
    atk_soft_timer_init(&uart_handle->uart_tx_timer, __lpuart_tx_timeout_cb, uart_handle, Timeout, 0); 
    atk_soft_timer_start(&uart_handle->uart_tx_timer);
           
    /* 同步超时的时间 */    
    while (HAL_UART_GetState(uart_handle->p_huart) != HAL_UART_STATE_READY);//等待就绪    
    return ret;
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






