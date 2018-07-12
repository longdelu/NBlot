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


//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误       
atk_ring_buf_t g_uart_ring_buff;  //创建一个uart ringbuff 的缓冲区 


UART_HandleTypeDef hlpuart1;


//注册串口事件回调函数
void lpuart_event_registercb(uart_cb cb, void *p_arg)
{  
    if(cb != 0)
    {
        uart_dev.uart_cb  = cb;
        uart_dev.p_arg    = p_arg;
    }
}

//设置串口事件
void lpuart_event_set (int uart_event)
{ 
    uart_dev.uart_event |= uart_event;   
}

//获取串口事件
int lpuart_event_get (int uart_event)
{ 
    return (uart_dev.uart_event & uart_event); 
}

//清除串口事件
void lpuart_event_clr (int uart_event)
{ 
    uart_dev.uart_event ^= uart_event;
}

 /**
  * @brief 初始化环形缓冲区的相关信息
  */
void atk_ring_buff_init (atk_ring_buf_t *p_ring_buff)
{
    p_ring_buff->head   = 0;
    p_ring_buff->tail   = 0;
    p_ring_buff->lenght = 0;
}

 /**
  * @brief 往环形缓冲区写数据
  */
u8 atk_ring_buff_write(atk_ring_buf_t *p_ring_buff, uint8_t data)
{
   if(p_ring_buff->lenght >= RING_BUFF_LEN)          //判断缓冲区是否已满
    {
        return -1;
    }
    
    //加入临界区保护
    INTX_DISABLE();
    
    p_ring_buff->ring_buff[p_ring_buff->tail] = data;

    p_ring_buff->tail = (p_ring_buff->tail+1) % RING_BUFF_LEN;//防止越界非法访问
    p_ring_buff->lenght++;

    INTX_ENABLE();     
    
    return 0;
}

 /**
 * @brief  读取环形缓冲区的数据
 */
u8 atk_read_ringbuff(atk_ring_buf_t *p_ring_buff, uint8_t *data)
{
   if (p_ring_buff->lenght == 0)    //判断非空
   {
       return -1;
   }
   
   //加入临界区保护
   INTX_DISABLE();   
   
   *data = p_ring_buff->ring_buff[p_ring_buff->head];        //先进先出FIFO，从缓冲区头出
   p_ring_buff->head = (p_ring_buff->head+1) % RING_BUFF_LEN; //防止越界非法访问
   p_ring_buff->lenght--;
   
   INTX_ENABLE(); 
   
   return TRUE;
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
   
    // 填充这个串口设备结构体    
    uart_dev.p_huart    = &hlpuart1; 
    uart_dev.uart_event = UART_NONE_EVENT;   
    uart_dev.bound      = bound; 

    __HAL_UART_DISABLE_IT(&hlpuart1, UART_IT_TXE);  //禁止发送中断 
    __HAL_UART_DISABLE_IT(&hlpuart1,UART_IT_TC);    //禁能发送完成中断       
  
#if EN_LPUART1_RX
    __HAL_UART_ENABLE_IT(&hlpuart1,UART_IT_RXNE);    //开启接收中断
    
    HAL_NVIC_EnableIRQ(LPUART1_IRQn);                //使能USART1中断通道
    HAL_NVIC_SetPriority(LPUART1_IRQn,3,3);          //抢占优先级3，子优先级3
#endif 

    return  &uart_dev;   
}


//轮询发送串口数据
int uart_data_tx_poll(UART_HandleTypeDef *huart, uint8_t *pData,uint16_t size, uint32_t Timeout)
{   
    int ret = 0;
    
    int tx_int_flag = 0;
    
    if(size == 0 || pData == NULL)
    {
       return -1;
    }
    
    tx_int_flag = __HAL_UART_GET_IT_SOURCE(&hlpuart1, UART_IT_TXE);
    
    
    if (tx_int_flag) {
    
        __HAL_UART_DISABLE_IT(&hlpuart1, UART_IT_TXE);    //禁能发送中断  
    }    
        
    /* 发送设置超时为 Timeout ms */ 
    ret = HAL_UART_Transmit(huart, pData, size, Timeout);
    
    
    if (ret == HAL_TIMEOUT) {
        //设置串口发送超时事件
        lpuart_event_set(UART_TX_TIMEOUT_EVENT); 
        
    } else if (ret == HAL_OK) {
        
        //设置串口发送完成事件
        lpuart_event_set(UART_TX_EVENT);         
    }   
    
    if (tx_int_flag) {
    
        __HAL_UART_ENABLE_IT(&hlpuart1, UART_IT_TXE);    //使能发送中断     
    }     
            
    return  ret;
}

//轮询接收串口数据
int uart_data_rx_poll(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t size, uint32_t Timeout)
{
    int ret = 0;
    
    int rx_int_flag = 0;
    
    if(size == 0 || pData == NULL)
    {
       return -1;
    }
       
    rx_int_flag = __HAL_UART_GET_IT_SOURCE(&hlpuart1, UART_IT_RXNE);
    
    
    if (rx_int_flag) {
    
        __HAL_UART_DISABLE_IT(&hlpuart1, UART_IT_RXNE);    //禁止接收中断
    }
    
    /* 发送设置超时为 HAL_MAX_DELAY ms */
    ret = HAL_UART_Receive(huart, pData, size, Timeout);
        
    if (rx_int_flag) {
        __HAL_UART_ENABLE_IT(&hlpuart1, UART_IT_RXNE);    //开启接收中断
    }
    
    return ret;
}



#if 0

//串口1中断服务程序
void LPUART1_IRQHandler(void)                    
{ 
    u8 Res;

    if((__HAL_UART_GET_FLAG(&hlpuart1,UART_FLAG_RXNE)!=RESET))  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
    {
        HAL_UART_Receive(&hlpuart1,&Res,1,1000); 
        if((NBLOT_USART_RX_STA&0x8000)==0)//接收未完成
        {
            if(NBLOT_USART_RX_STA&0x4000)//接收到了0x0d
            {
                if(Res!=0x0a)NBLOT_USART_RX_STA=0;//接收错误,重新开始
                else NBLOT_USART_RX_STA|=0x8000;    //接收完成了 
            }
            else //还没收到0X0D
            {    
                if(Res==0x0d)NBLOT_USART_RX_STA|=0x4000;
                else
                {
                    NBLOT_RxBuffer[NBLOT_USART_RX_STA&0X3FFF]=Res ;
                    NBLOT_USART_RX_STA++;
                    if(NBLOT_USART_RX_STA>(LPUSART_REC_LEN-1))NBLOT_USART_RX_STA=0;//接收数据错误,重新开始接收      
                }         
            }
        }            
    }
    
    HAL_UART_IRQHandler(&hlpuart1);    
} 

#endif /* if 0 */

   

#if 0
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance==LPUART1)//如果是串口1
    {
       /* 接收在超时时间内正常完成，停止超时 */  
       atk_soft_timer_stop(&uart_dev.uart_rx_timer);
       
       //设置串口接收完成事件
       lpuart_event_set(UART_RX_EVENT);
              
    }
}


void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == LPUART1)//如果是串口1
    {
       /* 发送在超时时间内正常完成，停止超时 */  
       atk_soft_timer_stop(&uart_dev.uart_rx_timer);
       
       //设置串口发送完成事件
       lpuart_event_set(UART_TX_EVENT);
       //回调注册进来的串口事件处理函数 
//       uart_dev.uart_cb(uart_dev.p_arg); 
              
    }
}

//串口1中断服务程序
void LPUART1_IRQHandler(void)                    
{         
    HAL_UART_IRQHandler(&hlpuart1);    //调用HAL库中断处理公用函数            
}
#endif


static void __lpuart_rx_timeout_cb (void *p_arg)
{
    
    uart_dev_t *p_lpuart_dev  = (uart_dev_t *)p_arg;
    
    UART_HandleTypeDef *lphuart = p_lpuart_dev->p_huart;
          
      /* Disable the UART Parity Error Interrupt and RXNE interrupts */
#if defined(USART_CR1_FIFOEN)
    CLEAR_BIT(lphuart->Instance->CR1, (USART_CR1_RXNEIE_RXFNEIE | USART_CR1_PEIE));
#else
    CLEAR_BIT(lphuart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE));
#endif 
       
    /* Disable the UART Error Interrupt: (Frame error, noise error, overrun error) */
    CLEAR_BIT(lphuart->Instance->CR3, USART_CR3_EIE); 

    //reset the lphuart->RxState
    lphuart->RxState= HAL_UART_STATE_READY;  
       
    //假读一次，把里面的数据取走
    READ_REG(lphuart->Instance->RDR);
       
    //设置串口接收超时事件
    lpuart_event_set(UART_RX_TIMEOUT_EVENT);    
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
    lpuart_event_set(UART_TX_TIMEOUT_EVENT);    
} 


//中断接收串口数据
int uart_data_rx_int(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t size, uint32_t Timeout)
{
    int ret = 0;
    
    if(size == 0 || pData == NULL)
    {
       return - 1;
    }
         
    /* 发送设置超时为 HAL_MAX_DELAY ms */
    ret = HAL_UART_Receive_IT(huart, pData, size);
    
    /* 初始超时计算 */
    atk_soft_timer_init(&uart_dev.uart_rx_timer, __lpuart_rx_timeout_cb, &uart_dev, Timeout, 0); 
    atk_soft_timer_start(&uart_dev.uart_rx_timer);
             
    /* 同步超时的时间 */
    while (HAL_UART_GetState(&hlpuart1) != HAL_UART_STATE_READY);//等待就绪
               
    return ret;
}


//中断发送串口数据， 保留使用
int uart_data_tx_int(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t size, uint32_t Timeout)
{
    int ret = 0;
    
    if(size == 0 || pData == NULL)
    {
       return -1;
    }
    
    ret = HAL_UART_Transmit_IT(huart, pData, size);
    
     /* 初始超时计算 */
    atk_soft_timer_init(&uart_dev.uart_tx_timer, __lpuart_tx_timeout_cb, &uart_dev, Timeout, 0); 
    atk_soft_timer_start(&uart_dev.uart_tx_timer);
           
    /* 同步超时的时间 */    
    while (HAL_UART_GetState(&hlpuart1) != HAL_UART_STATE_READY);//等待就绪
    
    return ret;
}

//轮询串口事件
void uart_event_poll(uart_handle_t uart_handle)
{ 
    //回调注册进来的串口事件处理函数 
    uart_dev.uart_cb(uart_handle->p_arg);
    
}






