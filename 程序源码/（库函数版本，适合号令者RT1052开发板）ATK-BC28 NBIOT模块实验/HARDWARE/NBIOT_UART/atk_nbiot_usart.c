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
  * @brief  ���������豸�ṹ��
  */
static lpuart_config_t lpuart_config;

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

//��ȡLPUART��ʱ��ԴƵ�ʣ�����ǰ�����õ���80MHz
//����ֵ��LPUARTʱ��ԴƵ��,�������ǵ�ֻ��һ����80Mhz
static u32 LPUART_SrcFreqGet(void)
{
    uint32_t freq;
    
    if(CLOCK_GetMux(kCLOCK_UartMux)==0)     //LPUART��ʱ��Դѡ��PLL3/6
    {
        freq=(CLOCK_GetPllFreq(kCLOCK_PllUsb1)/6U)/(CLOCK_GetDiv(kCLOCK_UartDiv)+1U);
    }
    else                                    //LPUART��ʱ��Դѡ��OSC
    {
        freq=CLOCK_GetOscFreq()/(CLOCK_GetDiv(kCLOCK_UartDiv)+1U);
    }
    return freq;
}

/**
 * @brief  ���ڳ�ʼ��
 * @param  bound : ������.
 * @retval �����豸�����ָ��
 */
uart_dev_t *atk_nbiot_uart_init (u32 bound)
{   
    int err = 0;
  
    u32 freq=0;                           //���ڵ�ʱ��ԴƵ��
  
    //�����������豸�ṹ��    
    uart_dev.p_huart    = LPUART8; 
    uart_dev.uart_event = UART_NONE_EVENT;   
    uart_dev.bound      = bound;   
    
    CLOCK_EnableClock(kCLOCK_Lpuart8);    //ʹ��LPUART8ʱ��
    
    CLOCK_SetMux(kCLOCK_UartMux,0);       //����UARTʱ��ԴΪPLL3 80Mhz��PLL3/6=480/6=80MHz
    CLOCK_SetDiv(kCLOCK_UartDiv,0);       //����UARTʱ��1��Ƶ����UARTʱ��Ϊ80Mhz
    
    //LPUART8��ʹ�õ�IO�������ã�������ALT0~ALT7ѡ����ʵĹ��ܡ�
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_10_LPUART8_TX,0U);    //GPIO_AD_B1_10����ΪLPUART8_TX
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_11_LPUART8_RX,0U);    //GPIO_AD_B1_11����ΪLPUART8_RX

    //����IO����GPIO_AD_B1_10��GPIO_AD_B1_11�Ĺ���
    //��ת���ٶ�,��������ΪR0/6,�ٶ�Ϊ100Mhz���رտ�·���ܣ�ʹ��pull/keepr
    //ѡ��keeper���ܣ�����100K Ohm���ر�Hyst
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_10_LPUART8_TX,0x10B0u); 
    
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_11_LPUART8_RX,0x10B0u); 

    freq=LPUART_SrcFreqGet();    
    
    LPUART_GetDefaultConfig(&lpuart_config);                  //������ΪĬ�����ã������ڸ���ʵ���������
    
    lpuart_config.baudRate_Bps=bound;                         //������
    lpuart_config.dataBitsCount=kLPUART_EightDataBits;        //8λ
    lpuart_config.stopBitCount=kLPUART_OneStopBit;            //1λֹͣλ
    lpuart_config.parityMode=kLPUART_ParityDisabled;          //����żУ��
    lpuart_config.enableRx=true;                              //ʹ�ܽ���
    lpuart_config.enableTx=true;                              //ʹ�ܷ���
    
    LPUART_Init(uart_dev.p_huart, &lpuart_config,freq);       //��ʼ��LPUART 
           
    //��ʼ���ճ�ʱ����������ַ�֮��Ľ���ʱ�䳬��10msû�е�����������Ϊ�������
    atk_soft_timer_init(&uart_dev.uart_rx_timer, __lpuart_rx_timeout_cb, &uart_dev, 10, 0);     
    
    //��ʼ�����ջ��λ�����
    err = atk_ring_buf_init(&g_uart_ring_buf);
       
    if (err == 0) 
    {
        uart_dev.p_uart_ring_buff  = &g_uart_ring_buf;           
    }
    else
    {
        uart_dev.p_uart_ring_buff  = NULL;   
    }
      
    //LPUART�ж�����,ʹ�ܽ����ж���IDLE�ж�
    LPUART_EnableInterrupts(uart_dev.p_huart,kLPUART_RxDataRegFullInterruptEnable | kLPUART_IdleLineInterruptEnable); 
    RT1052_NVIC_SetPriority(LPUART8_IRQn,3,3);                //��ռ���ȼ�5�������ȼ�0
    EnableIRQ(LPUART8_IRQn);        
                                                                                                       
    return  &uart_dev;   
}

 /**
  * @brief  ���ڽ��ճ�ʱ�ص�����
  */
static void __lpuart_rx_timeout_cb (void *p_arg)
{
    
    uart_dev_t *p_lpuart_dev  = (uart_dev_t *)p_arg;
    
    LPUART_Type *p_huart = p_lpuart_dev->p_huart;
    
    //����������
    LPUART_ReadByte(p_huart);
             
    //���NE�����жϱ��
    LPUART_ClearStatusFlags(p_huart, kLPUART_NoiseErrorFlag);
                                                       
    //�������жϱ��
    LPUART_ClearStatusFlags(p_huart, kLPUART_RxOverrunFlag);
                                
    //���֡�����жϱ��
    LPUART_ClearStatusFlags(p_huart, kLPUART_FramingErrorFlag);
                                        
    //���У���жϱ��
    LPUART_ClearStatusFlags(p_huart, kLPUART_ParityErrorFlag);
                                                 
    //���ilde�жϱ��
    LPUART_ClearStatusFlags(p_huart, kLPUART_IdleLineFlag);   
    
    //��������ж�

    //���¿��������ж� 
   
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
              
    //���ô��ڷ��ͳ�ʱ�¼�
    uart_event_set(p_lpuart_dev, UART_TX_TIMEOUT_EVENT);    
}

#ifdef UART_ANY_DATA_LEN_RECV  

 /**
  * @brief  �����жϴ�����
  */
void LPUART8_IRQHandler(void)                    
{ 
    LPUART_Type *p_huart = uart_dev.p_huart;  
  
    if (LPUART_GetStatusFlags(p_huart) & kLPUART_RxDataRegFullFlag)  
    {      
        //������д�뻷�λ�����
        atk_ring_buf_write(&g_uart_ring_buf, p_huart->DATA);  
                      
        //�յ�һ�����ݣ����ó�ʱΪ500ms 
        atk_soft_timer_timeout_change(&uart_dev.uart_rx_timer, 500);
               
        // �յ����ݱ����������ڳ�ʱʱ����������ɣ�ֹͣ���ͳ�ʱ  
        atk_soft_timer_stop(&uart_dev.uart_tx_timer);                                   
    }
    
    //ilde�жϲ���������ǰ����֡���ս���
    if (LPUART_GetStatusFlags(p_huart) & kLPUART_IdleLineFlag)  
    {        
        //����ǿ����ж�һ��ʼ��ʹ�ܵ�
        if (LPUART_GetEnabledInterrupts(p_huart) & kLPUART_IdleLineInterruptEnable) 
        {           
            //�����ڳ�ʱʱ����������ɣ�ֹͣ���ճ�ʱ  
            atk_soft_timer_stop(&uart_dev.uart_rx_timer);

            //���ô��ڽ�������¼�
            uart_event_set(&uart_dev, UART_RX_EVENT); 
                                              
            //���NE�����жϱ��
            LPUART_ClearStatusFlags(p_huart, kLPUART_NoiseErrorFlag);
                                                               
            //�������жϱ��
            LPUART_ClearStatusFlags(p_huart, kLPUART_RxOverrunFlag);
                                        
            //���֡�����жϱ��
            LPUART_ClearStatusFlags(p_huart, kLPUART_FramingErrorFlag);
                                                
            //���У���жϱ��
            LPUART_ClearStatusFlags(p_huart, kLPUART_ParityErrorFlag);
                                                    
            //��������ж�
          
            //���¿��������ж�                      

        } 

        //���ilde�жϱ��
        LPUART_ClearStatusFlags(p_huart, kLPUART_IdleLineFlag);     
                            
    } 

    __DSB();                //����ͬ������ָ��    
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
     
    tx_int_flag = LPUART_GetEnabledInterrupts(uart_handle->p_huart) & kLPUART_TxDataRegEmptyInterruptEnable;
    
    if (tx_int_flag) 
    {   
        LPUART_DisableInterrupts(uart_handle->p_huart, kLPUART_TxDataRegEmptyInterruptEnable);    //���ܷ����ж�  
    }    
        
    /* ������������ */ 
    LPUART_WriteBlocking(uart_handle->p_huart, pData, size);
                
    //���ô��ڷ�������¼�
    uart_event_set(uart_handle, UART_TX_EVENT);         
 
    
    if (tx_int_flag) {
    
        LPUART_EnableInterrupts(uart_handle->p_huart, kLPUART_TxDataRegEmptyInterruptEnable);   //ʹ�ܷ����ж�     
    }
       
    /* ��ʼ��ʱ���� */
    atk_soft_timer_init(&uart_handle->uart_tx_timer, __lpuart_tx_timeout_cb, uart_handle, Timeout, 0); 
    atk_soft_timer_start(&uart_handle->uart_tx_timer);    
    
    return  ret;
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






