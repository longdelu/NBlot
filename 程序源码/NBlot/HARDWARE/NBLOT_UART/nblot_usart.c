#include "nblot_usart.h"
#include "delay.h"

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


#if EN_LPUART1_RX   //如果使能了接收

//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 NBLOT_RxBuffer[LPUSART_REC_LEN];     //接收缓冲,最大LPUSART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 NBLOT_USART_RX_STA=0;       //接收状态标记	

UART_HandleTypeDef hlpuart1;

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
void lpuart1_init (u32 bound)
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
  
#if EN_LPUART1_RX
		__HAL_UART_ENABLE_IT(&hlpuart1,UART_IT_RXNE);    //开启接收中断
		HAL_NVIC_EnableIRQ(LPUART1_IRQn);				//使能USART1中断通道
		HAL_NVIC_SetPriority(LPUART1_IRQn,3,3);			//抢占优先级3，子优先级3
#endif	  

}


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
				else NBLOT_USART_RX_STA|=0x8000;	//接收完成了 
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
#endif	


//轮询发送串口数据
int uart_data_tx_poll(UART_HandleTypeDef *huart, uint8_t *pData,uint16_t size)
{
    if(size == 0 || pData == NULL)
    {
       return -1;
    }
    
    /* 发送设置超时为 HAL_MAX_DELAY ms */
    return HAL_UART_Transmit(huart, pData, size, HAL_MAX_DELAY);
}

//轮询接收串口数据
int uart_data_rx_poll(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t size)
{
    int ret = 0;
    
    if(size == 0 || pData == NULL)
    {
       return -1;
    }
    
    __HAL_UART_DISABLE_IT(&hlpuart1, UART_IT_RXNE);    //禁止接收中断
    
    /* 发送设置超时为 HAL_MAX_DELAY ms */
    ret = HAL_UART_Receive(huart, pData, size, HAL_MAX_DELAY);
    
    __HAL_UART_DISABLE_IT(&hlpuart1, UART_IT_RXNE);    //开启接收中断
    
    return ret;
}




