#include "nblot_usart.h"
#include "delay.h"
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

//UART设备结构体
static uart_dev_t uart_dev;

//创建一个uart ringbuff 的缓冲区      
static atk_ring_buf_t g_uart_ring_buf;  

//HAL库串口设备结构体
static UART_HandleTypeDef hlpuart1;

//函数提前声明
static void __lpuart_rx_timeout_cb (void *p_arg);

//注册串口事件回调函数
void lpuart_event_registercb(uart_handle_t uart_handle, uart_cb cb, void *p_arg)
{  
    if(cb != 0)
    {
        uart_handle->uart_cb  = cb;
        uart_handle->p_arg    = p_arg;
    }
}

//设置串口事件
void lpuart_event_set (uart_handle_t uart_handle, int uart_event)
{ 
    uart_handle->uart_event |= uart_event;   
}

//获取串口事件
int lpuart_event_get (uart_handle_t uart_handle, int uart_event)
{ 
    return (uart_handle->uart_event & uart_event); 
}

//清除串口事件
void lpuart_event_clr (uart_handle_t uart_handle, int uart_event)
{ 
    uart_handle->uart_event ^= uart_event;
}

/**
 * @brief 初始化环形缓冲区的相关信息
 */
int atk_ring_buf_init (atk_ring_buf_t *p_ring_buf)
{    
    if (p_ring_buf == NULL) {        
        return -1;       
    }
    
    p_ring_buf->head   = 0;
    p_ring_buf->tail   = 0;
    p_ring_buf->lenght = 0;
    
    return 0;
}

/**
 * @brief 往环形缓冲区写数据
 */
int atk_ring_buf_write(atk_ring_buf_t *p_ring_buf, uint8_t data)
{
    if(p_ring_buf->lenght >= RING_BUF_LEN)          //判断缓冲区是否已满
    {      
        printf("the ring buf is full\r\n");
        
        return -1;
    } 
    
    //加入临界区保护
//    INTX_DISABLE();
    
    p_ring_buf->ring_buf[p_ring_buf->tail] = data;

    //防止越界非法访问
    p_ring_buf->tail = (p_ring_buf->tail+1) % RING_BUF_LEN;
    
    p_ring_buf->lenght++;

//   INTX_ENABLE();     
    
    return 0;
}

/**
 * @brief 环形缓冲区中有效数据的个数
 */
int atk_ring_buf_avail_len (atk_ring_buf_t *p_ring_buf)
{
     return p_ring_buf->lenght;
}

 /**
 * @brief  读取环形缓冲区的数据
 */
int atk_ring_buf_read(atk_ring_buf_t *p_ring_buf, uint8_t *data)
{
   if (p_ring_buf->lenght == 0)    //判断非空
   {
       return -1;
   }
   
   //加入临界区保护
//   INTX_DISABLE();   
   
   //先进先出FIFO，从缓冲区头出
   *data = p_ring_buf->ring_buf[p_ring_buf->head];
   
   //防止越界非法访问
   p_ring_buf->head = (p_ring_buf->head+1) % RING_BUF_LEN; 
   
   p_ring_buf->lenght--;
   
//   INTX_ENABLE(); 
   
   return 0;
}

 /**
 * @brief  从接收缓存里读取指定长度的数据，并释放占用的空间
 */
int atk_ring_buf_size_read(atk_ring_buf_t *p_ring_buf, uint8_t *data, int len)
{   
    int i = 0; 
        
    if (len > atk_ring_buf_avail_len(p_ring_buf))
    {
        return -1; 
    }
    
    for (i = 0; i <  len; i++) {
        
         atk_ring_buf_read(p_ring_buf, &data[i]);
    }

    return 0;
}


 /**
 * @brief  写入接收缓存里读指定长度的数据，并占用的空间
 */
int atk_ring_buf_size_write(atk_ring_buf_t *p_ring_buf, uint8_t *data, int len)
{
    
    int i = 0; 
    
    //缓存空余个数小于写入的长度    
    if (len > (RING_BUF_LEN - p_ring_buf->lenght))
    {
        return -1; 
    }
    
    for (i = 0; i <  len; i++) {
        
         atk_ring_buf_write(p_ring_buf, data[i]);
    }

    return 0;
}



 /**
 * @brief  从UART接收缓存里读取指定长度的数据，并释放占用的空间
 */
int uart_ring_buf_read(uart_handle_t uart_handle, uint8_t *data, int len)
{
    int ret = 0;
    
    ret = atk_ring_buf_size_read(uart_handle->p_uart_ring_buff, data, len);
     
    return ret;
}

 /**
 * @brief  获取串口环形缓冲区中有效数据的个数
 */
int uart_ring_buf_avail_len(uart_handle_t uart_handle)
{
    return atk_ring_buf_avail_len(uart_handle->p_uart_ring_buff);
}

 /**
 * @brief  写入uart接收缓存里读指定长度的数据，并占用的空间
 */
int uart_ring_buf_write(uart_handle_t uart_handle, uint8_t *data, int len)
{
    int ret = 0;
    
    ret = atk_ring_buf_size_write(uart_handle->p_uart_ring_buff, data, len);
     
    return ret;
}

//UART底层初始化，时钟使能，引脚配置，中断配置
//此函数会被HAL_UART_Init()调用
//huart:串口句柄
void HAL_LPUART1_MspInit(UART_HandleTypeDef *huart)
{
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_Initure;
    
    if(huart->Instance==LPUART1)//如果是串口1，进行串口1 MSP初始化
    {
        __HAL_RCC_LPUART1_CLK_ENABLE();
        
        /* GPIO Ports Clock Enable */
        __HAL_RCC_GPIOB_CLK_ENABLE();
       
        /**LPUART1 GPIO Configuration    
        PB10     ------> LPUART1_RX
        PB11     ------> LPUART1_TX 
        */
        GPIO_Initure.Pin = GPIO_PIN_10|GPIO_PIN_11;
        GPIO_Initure.Mode = GPIO_MODE_AF_PP;
        GPIO_Initure.Pull = GPIO_NOPULL;
        GPIO_Initure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_Initure.Alternate = GPIO_AF8_LPUART1;
        HAL_GPIO_Init(GPIOB, &GPIO_Initure);                 
    }

}


//初始化IO LPUART1
//bound:波特率
uart_dev_t *lpuart1_init (u32 bound)
{      
    int err = 0;
    
    hlpuart1.Instance = LPUART1;
    hlpuart1.Init.BaudRate = bound;
    hlpuart1.Init.WordLength = UART_WORDLENGTH_8B;
    hlpuart1.Init.StopBits = UART_STOPBITS_1;
    hlpuart1.Init.Parity = UART_PARITY_NONE;
    hlpuart1.Init.Mode = UART_MODE_TX_RX;
    hlpuart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    hlpuart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    hlpuart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

    HAL_LPUART1_MspInit(&hlpuart1); 
    
       
    if (HAL_UART_Init(&hlpuart1) != HAL_OK)
    {
       _Error_Handler(__FILE__, __LINE__);
    }
    
    //初始接收超时，如果两个字符之间的接收时间超过10ms没有到来，可以认为接收完成
    atk_soft_timer_init(&uart_dev.uart_rx_timer, __lpuart_rx_timeout_cb, &uart_dev, 10, 0);     
    
    //接收化接收环形缓冲区
    err = atk_ring_buf_init(&g_uart_ring_buf);
   
    //填充这个串口设备结构体    
    uart_dev.p_huart    = &hlpuart1; 
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
     __HAL_UART_DISABLE(&hlpuart1);    

    __HAL_UART_DISABLE_IT(&hlpuart1, UART_IT_TXE);  //禁止发送中断 
    __HAL_UART_DISABLE_IT(&hlpuart1, UART_IT_TC);   //禁能发送完成中断   
    
    //清除rx中断标记
    __HAL_UART_SEND_REQ(&hlpuart1, UART_RXDATA_FLUSH_REQUEST);
      
    //清除ilde中断标记
    __HAL_UART_CLEAR_IT(&hlpuart1, UART_CLEAR_IDLEF); 
     
    __HAL_UART_ENABLE_IT(&hlpuart1, UART_IT_RXNE);    //开启接收中断      
    __HAL_UART_ENABLE_IT(&hlpuart1, UART_IT_IDLE);    //开启空闲中断 
    
     //清除ilde中断标记
    __HAL_UART_CLEAR_IT(&hlpuart1, UART_CLEAR_IDLEF); 

    HAL_NVIC_EnableIRQ(LPUART1_IRQn);                //使能USART1中断通道
    HAL_NVIC_SetPriority(LPUART1_IRQn,3,3);          //抢占优先级3，子优先级3
    
    //使能串口
     __HAL_UART_ENABLE(&hlpuart1);   
       
    return  &uart_dev;   
}

static void __lpuart_rx_timeout_cb (void *p_arg)
{
    
    uart_dev_t *p_lpuart_dev  = (uart_dev_t *)p_arg;
    
    UART_HandleTypeDef *lphuart = p_lpuart_dev->p_huart;

#if !UART_ANY_DATA_LEN_RECV 
 
    /* Disable the UART Parity Error Interrupt and RXNE interrupts */
#if defined(USART_CR1_FIFOEN)
    CLEAR_BIT(lphuart->Instance->CR1, (USART_CR1_RXNEIE_RXFNEIE | USART_CR1_PEIE));
#else
    CLEAR_BIT(lphuart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE));
#endif //endif  USART_CR1_FIFOEN
    
    /* Disable the UART Error Interrupt: (Frame error, noise error, overrun error) */
    CLEAR_BIT(lphuart->Instance->CR3, USART_CR3_EIE);     
    
#endif //endif    UART_ANY_DATA_LEN_RECV
    
    //reset the lphuart->RxState
    lphuart->RxState= HAL_UART_STATE_READY; 

    //清垃圾数据
    READ_REG(lphuart->Instance->RDR);
             
    //清除溢出中断标记
    __HAL_UART_CLEAR_IT(&hlpuart1,UART_CLEAR_OREF); 
                               
   
    //清除帧错误中断标记
    __HAL_UART_CLEAR_IT(&hlpuart1, UART_CLEAR_FEF); 
                               
        
    //清除校验中断标记
    __HAL_UART_CLEAR_IT(&hlpuart1, UART_CLEAR_PEF); 
                                   
           
    //清除接收中断
    __HAL_UART_SEND_REQ(lphuart, UART_RXDATA_FLUSH_REQUEST);
    
    //清除ilde中断标记
    __HAL_UART_CLEAR_IT(lphuart, UART_FLAG_IDLE); 

    //重新开启接收中断 
    __HAL_UART_ENABLE_IT(&hlpuart1, UART_IT_RXNE);    
       
    //设置串口接收超时事件
    lpuart_event_set(p_lpuart_dev, UART_RX_TIMEOUT_EVENT);    
}


static void __lpuart_tx_timeout_cb (void *p_arg)
{
    
    uart_dev_t *p_lpuart_dev  = (uart_dev_t *)p_arg;
    
    UART_HandleTypeDef *lphuart = p_lpuart_dev->p_huart;
    
      /* Disable the UART Transmit Data Register Empty Interrupt */
#if defined(USART_CR1_FIFOEN)
      CLEAR_BIT(lphuart->Instance->CR1, USART_CR1_TXEIE_TXFNFIE);
#else
      CLEAR_BIT(lphuart->Instance->CR1, USART_CR1_TXEIE);
#endif
    
    /* Disable the UART Transmit Complete Interrupt */
    CLEAR_BIT(lphuart->Instance->CR1, USART_CR1_TCIE);          

    //reset the lphuart->RxState
    lphuart->gState= HAL_UART_STATE_READY;  
        
    //设置串口发送超时事件
    lpuart_event_set(p_lpuart_dev, UART_TX_TIMEOUT_EVENT);    
}

#ifdef UART_ANY_DATA_LEN_RECV  

//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误  
void LPUART1_IRQHandler(void)                    
{ 

     
    if ((__HAL_UART_GET_FLAG(&hlpuart1,UART_FLAG_RXNE)!=RESET))  
    {        
        //把数据写入环形缓冲区
        atk_ring_buf_write(&g_uart_ring_buf, hlpuart1.Instance->RDR);  
             
        //收到一个数据，重置超时 
        atk_soft_timer_timeout_change(&uart_dev.uart_rx_timer, 10);
      
          
        /* 收到数据发送在超时时间内正常完成，停止超时 */  
        atk_soft_timer_stop(&uart_dev.uart_tx_timer);
                                   
    }
    
    //ilde中断产生，代表当前数据帧接收结束
    if ((__HAL_UART_GET_FLAG(&hlpuart1, UART_FLAG_IDLE)!=RESET))  
    {
     
    
        //如果是空闲中断一开始是使能的
       if (__HAL_UART_GET_IT_SOURCE(&hlpuart1, UART_IT_IDLE)!=RESET) {
           
           //接收在超时时间内正常完成，停止超时  
           atk_soft_timer_stop(&uart_dev.uart_rx_timer);
           
           //设置串口接收完成事件
           lpuart_event_set(&uart_dev, UART_RX_EVENT); 
       } 

        //清除ilde中断标记
        __HAL_UART_CLEAR_IT(&hlpuart1, UART_CLEAR_IDLEF);        
                            
    }    
    
    //调用HAL中断处理程序
    HAL_UART_IRQHandler(&hlpuart1);    
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == LPUART1)//如果是串口1
    {
       /* 发送在超时时间内正常完成，停止超时 */  
       atk_soft_timer_stop(&uart_dev.uart_tx_timer);
       
       //设置串口发送完成事件
       lpuart_event_set(&uart_dev, UART_TX_EVENT);              
    }
}

#else 
//该函数在数据接收完成时被调用
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance==LPUART1)//如果是串口1
    {
       //接收在超时时间内正常完成，停止超时  
       atk_soft_timer_stop(&uart_dev.uart_rx_timer);
       
       //设置串口接收完成事件
       lpuart_event_set(&uart_dev,UART_RX_EVENT);
              
    }
}

//串口1中断服务程序
void LPUART1_IRQHandler(void)                    
{         
    HAL_UART_IRQHandler(&hlpuart1);    //调用HAL库中断处理公用函数 

    //ilde中断产生，代表当前数据帧接收结束
    if ((__HAL_UART_GET_FLAG(&hlpuart1, UART_FLAG_IDLE)!=RESET))  
    {
     
        //清除ilde中断标记
        __HAL_UART_CLEAR_IT(&hlpuart1, UART_FLAG_IDLE); 
                                         
    }    
}
#endif   //end if UART_ANY_DATA_LEN_RECV 
 

//轮询发送串口数据
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
        lpuart_event_set(uart_handle, UART_TX_TIMEOUT_EVENT); 
      
        return ret;
        
    } else if (ret == HAL_OK) {
        
        //设置串口发送完成事件
        lpuart_event_set(uart_handle, UART_TX_EVENT);         
    }   
    
    if (tx_int_flag) {
    
        __HAL_UART_ENABLE_IT(uart_handle->p_huart, UART_IT_TXE);    //使能发送中断     
    }
    
    
    /* 初始超时计算 */
    atk_soft_timer_init(&uart_handle->uart_tx_timer, __lpuart_tx_timeout_cb, uart_handle, Timeout, 0); 
    atk_soft_timer_start(&uart_handle->uart_tx_timer);    
    
    return  ret;
}

//轮询接收串口数据
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
        lpuart_event_set(uart_handle, UART_RX_TIMEOUT_EVENT); 
        
    } else if (ret == HAL_OK) {
        
        //设置串口接收完成事件
        lpuart_event_set(uart_handle, UART_RX_EVENT);         
    }
        
    if (rx_int_flag) {
        __HAL_UART_ENABLE_IT(uart_handle->p_huart, UART_IT_RXNE);    //开启接收中断
    }
    
    return ret;
}


//中断接收串口数据
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


//中断发送串口数据， 保留使用
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

//轮询串口事件
void uart_event_poll(uart_handle_t uart_handle)
{ 
    //回调注册进来的串口事件处理函数 
    uart_dev.uart_cb(uart_handle->p_arg);
    
}






