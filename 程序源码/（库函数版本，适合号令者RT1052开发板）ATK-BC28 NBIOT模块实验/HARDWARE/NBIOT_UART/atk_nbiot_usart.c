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
  * @brief  串口配置设备结构体
  */
static lpuart_config_t lpuart_config;

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

//获取LPUART的时钟源频率，我们前面设置的是80MHz
//返回值：LPUART时钟源频率,根据我们的只是一般是80Mhz
static u32 LPUART_SrcFreqGet(void)
{
    uint32_t freq;
    
    if(CLOCK_GetMux(kCLOCK_UartMux)==0)     //LPUART的时钟源选择PLL3/6
    {
        freq=(CLOCK_GetPllFreq(kCLOCK_PllUsb1)/6U)/(CLOCK_GetDiv(kCLOCK_UartDiv)+1U);
    }
    else                                    //LPUART的时钟源选择OSC
    {
        freq=CLOCK_GetOscFreq()/(CLOCK_GetDiv(kCLOCK_UartDiv)+1U);
    }
    return freq;
}

/**
 * @brief  串口初始化
 * @param  bound : 波特率.
 * @retval 串口设备句柄的指针
 */
uart_dev_t *atk_nbiot_uart_init (u32 bound)
{   
    int err = 0;
  
    u32 freq=0;                           //串口的时钟源频率
  
    //填充这个串口设备结构体    
    uart_dev.p_huart    = LPUART8; 
    uart_dev.uart_event = UART_NONE_EVENT;   
    uart_dev.bound      = bound;   
    
    CLOCK_EnableClock(kCLOCK_Lpuart8);    //使能LPUART8时钟
    
    CLOCK_SetMux(kCLOCK_UartMux,0);       //设置UART时钟源为PLL3 80Mhz，PLL3/6=480/6=80MHz
    CLOCK_SetDiv(kCLOCK_UartDiv,0);       //设置UART时钟1分频，即UART时钟为80Mhz
    
    //LPUART8所使用的IO功能配置，即：从ALT0~ALT7选择合适的功能。
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_10_LPUART8_TX,0U);    //GPIO_AD_B1_10设置为LPUART8_TX
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_11_LPUART8_RX,0U);    //GPIO_AD_B1_11设置为LPUART8_RX

    //配置IO引脚GPIO_AD_B1_10和GPIO_AD_B1_11的功能
    //低转换速度,驱动能力为R0/6,速度为100Mhz，关闭开路功能，使能pull/keepr
    //选择keeper功能，下拉100K Ohm，关闭Hyst
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_10_LPUART8_TX,0x10B0u); 
    
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_11_LPUART8_RX,0x10B0u); 

    freq=LPUART_SrcFreqGet();    
    
    LPUART_GetDefaultConfig(&lpuart_config);                  //先设置为默认配置，后面在根据实际情况配置
    
    lpuart_config.baudRate_Bps=bound;                         //波特率
    lpuart_config.dataBitsCount=kLPUART_EightDataBits;        //8位
    lpuart_config.stopBitCount=kLPUART_OneStopBit;            //1位停止位
    lpuart_config.parityMode=kLPUART_ParityDisabled;          //无奇偶校验
    lpuart_config.enableRx=true;                              //使能接收
    lpuart_config.enableTx=true;                              //使能发送
    
    LPUART_Init(uart_dev.p_huart, &lpuart_config,freq);       //初始化LPUART 
           
    //初始接收超时，如果两个字符之间的接收时间超过10ms没有到来，可以认为接收完成
    atk_soft_timer_init(&uart_dev.uart_rx_timer, __lpuart_rx_timeout_cb, &uart_dev, 10, 0);     
    
    //初始化接收环形缓冲区
    err = atk_ring_buf_init(&g_uart_ring_buf);
       
    if (err == 0) 
    {
        uart_dev.p_uart_ring_buff  = &g_uart_ring_buf;           
    }
    else
    {
        uart_dev.p_uart_ring_buff  = NULL;   
    }
      
    //LPUART中断设置,使能接收中断与IDLE中断
    LPUART_EnableInterrupts(uart_dev.p_huart,kLPUART_RxDataRegFullInterruptEnable | kLPUART_IdleLineInterruptEnable); 
    RT1052_NVIC_SetPriority(LPUART8_IRQn,3,3);                //抢占优先级5，子优先级0
    EnableIRQ(LPUART8_IRQn);        
                                                                                                       
    return  &uart_dev;   
}

 /**
  * @brief  串口接收超时回调函数
  */
static void __lpuart_rx_timeout_cb (void *p_arg)
{
    
    uart_dev_t *p_lpuart_dev  = (uart_dev_t *)p_arg;
    
    LPUART_Type *p_huart = p_lpuart_dev->p_huart;
    
    //清垃圾数据
    LPUART_ReadByte(p_huart);
             
    //清除NE错误中断标记
    LPUART_ClearStatusFlags(p_huart, kLPUART_NoiseErrorFlag);
                                                       
    //清除溢出中断标记
    LPUART_ClearStatusFlags(p_huart, kLPUART_RxOverrunFlag);
                                
    //清除帧错误中断标记
    LPUART_ClearStatusFlags(p_huart, kLPUART_FramingErrorFlag);
                                        
    //清除校验中断标记
    LPUART_ClearStatusFlags(p_huart, kLPUART_ParityErrorFlag);
                                                 
    //清除ilde中断标记
    LPUART_ClearStatusFlags(p_huart, kLPUART_IdleLineFlag);   
    
    //清除接收中断

    //重新开启接收中断 
   
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
              
    //设置串口发送超时事件
    uart_event_set(p_lpuart_dev, UART_TX_TIMEOUT_EVENT);    
}

#ifdef UART_ANY_DATA_LEN_RECV  

 /**
  * @brief  串口中断处理函数
  */
void LPUART8_IRQHandler(void)                    
{ 
    LPUART_Type *p_huart = uart_dev.p_huart;  
  
    if (LPUART_GetStatusFlags(p_huart) & kLPUART_RxDataRegFullFlag)  
    {      
        //把数据写入环形缓冲区
        atk_ring_buf_write(&g_uart_ring_buf, p_huart->DATA);  
                      
        //收到一个数据，重置超时为500ms 
        atk_soft_timer_timeout_change(&uart_dev.uart_rx_timer, 500);
               
        // 收到数据表明，发送在超时时间内正常完成，停止发送超时  
        atk_soft_timer_stop(&uart_dev.uart_tx_timer);                                   
    }
    
    //ilde中断产生，代表当前数据帧接收结束
    if (LPUART_GetStatusFlags(p_huart) & kLPUART_IdleLineFlag)  
    {        
        //如果是空闲中断一开始是使能的
        if (LPUART_GetEnabledInterrupts(p_huart) & kLPUART_IdleLineInterruptEnable) 
        {           
            //接收在超时时间内正常完成，停止接收超时  
            atk_soft_timer_stop(&uart_dev.uart_rx_timer);

            //设置串口接收完成事件
            uart_event_set(&uart_dev, UART_RX_EVENT); 
                                              
            //清除NE错误中断标记
            LPUART_ClearStatusFlags(p_huart, kLPUART_NoiseErrorFlag);
                                                               
            //清除溢出中断标记
            LPUART_ClearStatusFlags(p_huart, kLPUART_RxOverrunFlag);
                                        
            //清除帧错误中断标记
            LPUART_ClearStatusFlags(p_huart, kLPUART_FramingErrorFlag);
                                                
            //清除校验中断标记
            LPUART_ClearStatusFlags(p_huart, kLPUART_ParityErrorFlag);
                                                    
            //清除接收中断
          
            //重新开启接收中断                      

        } 

        //清除ilde中断标记
        LPUART_ClearStatusFlags(p_huart, kLPUART_IdleLineFlag);     
                            
    } 

    __DSB();                //数据同步屏蔽指令    
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
     
    tx_int_flag = LPUART_GetEnabledInterrupts(uart_handle->p_huart) & kLPUART_TxDataRegEmptyInterruptEnable;
    
    if (tx_int_flag) 
    {   
        LPUART_DisableInterrupts(uart_handle->p_huart, kLPUART_TxDataRegEmptyInterruptEnable);    //禁能发送中断  
    }    
        
    /* 阻塞发送数据 */ 
    LPUART_WriteBlocking(uart_handle->p_huart, pData, size);
                
    //设置串口发送完成事件
    uart_event_set(uart_handle, UART_TX_EVENT);         
 
    
    if (tx_int_flag) {
    
        LPUART_EnableInterrupts(uart_handle->p_huart, kLPUART_TxDataRegEmptyInterruptEnable);   //使能发送中断     
    }
       
    /* 初始超时计算 */
    atk_soft_timer_init(&uart_handle->uart_tx_timer, __lpuart_tx_timeout_cb, uart_handle, Timeout, 0); 
    atk_soft_timer_start(&uart_handle->uart_tx_timer);    
    
    return  ret;
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






