#include "rs485.h"
#include "pcf8574.h"
#include "lpuart.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK I.MXRT1052开发板
//RS485驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/12/29
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

static lpuart_config_t lpuart4_config;

#if EN_USART4_RX   		//如果使能了接收   	  
//接收缓存区 	
u8 RS485_RX_BUF[64];  	//接收缓冲,最大64个字节.
//接收到的数据长度
u8 RS485_RX_CNT=0;  

void LPUART4_IRQHandler(void)
{
    u8 res;	  

    if((LPUART4->STAT)&kLPUART_RxDataRegFullFlag)   //接收中断
    {
        res=LPUART_ReadByte(LPUART4);  		        //读取数据			
        if(RS485_RX_CNT<64)
		{
			RS485_RX_BUF[RS485_RX_CNT]=res;		    //记录接收到的值
			RS485_RX_CNT++;						    //接收数据增加1 
		} 
    }
    __DSB();				                        //数据同步屏蔽指令
}    
#endif

//获取LPUART的时钟源频率，我们前面设置的是80MHz
//返回值：LPUART时钟源频率,根据我们的只是一般是80Mhz
u32 LPUART4_SrcFreqGet(void)
{
    uint32_t freq;
	
    if (CLOCK_GetMux(kCLOCK_UartMux)==0) 	//LPUART的时钟源选择PLL3/6
    {
        freq=(CLOCK_GetPllFreq(kCLOCK_PllUsb1)/6)/(CLOCK_GetDiv(kCLOCK_UartDiv)+1);
    }
    else									//LPUART的时钟源选择OSC
    {
        freq=CLOCK_GetOscFreq()/(CLOCK_GetDiv(kCLOCK_UartDiv)+1);
    }
    return freq;
}


//初始化IO 串口8
//bound:波特率
void RS485_Init(u32 bound)
{
	u32 freq=0;							//串口的时钟源频率
	
    PCF8574_Init();                     //初始化PCF8574，用于控制RE脚
    
//  LPUART1已经设置了UART时钟源    
//	CLOCK_EnableClock(kCLOCK_Lpuart4);	//使能LPUART8时钟
//	CLOCK_SetMux(kCLOCK_UartMux,0); 	//设置UART时钟源为PLL3 80Mhz，PLL3/6=480/6=80MHz
//  CLOCK_SetDiv(kCLOCK_UartDiv,0); 	//设置UART时钟1分频，即UART时钟为80Mhz
	
	//LPUART4所使用的IO功能配置，即：从ALT0~ALT7选择合适的功能。
	IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_00_LPUART4_TX,0);	//GPIO_SD_B1_00设置为LPUART4_TX
	IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_01_LPUART4_RX,0);	//GPIO_AD_B1_11设置为LPUART4_RX

	//配置IO引脚GPIO_SD_B1_00和GPIO_SD_B1_01的功能
	//低转换速度,驱动能力为R0/6,速度为100Mhz，关闭开路功能，使能pull/keepr
	//选择keeper功能，下拉100K Ohm，关闭Hyst
	IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_00_LPUART4_TX,0x10B0); 
	IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_01_LPUART4_RX,0x10B0); 

	freq=LPUART4_SrcFreqGet();	
	LPUART_GetDefaultConfig(&lpuart4_config); 				//先设置为默认配置，后面在根据实际情况配置
	
	lpuart4_config.baudRate_Bps=bound;						//波特率
	lpuart4_config.dataBitsCount=kLPUART_EightDataBits;		//8位
	lpuart4_config.stopBitCount=kLPUART_OneStopBit;			//1位停止位
	lpuart4_config.parityMode=kLPUART_ParityDisabled;		//无奇偶校验
	lpuart4_config.enableRx=true;							//使能接收
	lpuart4_config.enableTx=true;							//使能发送
	
	LPUART_Init(LPUART4,&lpuart4_config,freq);				//初始化LPUART4
	
	//LPUART中断设置
#if EN_USART4_RX
	LPUART_EnableInterrupts(LPUART4,kLPUART_RxDataRegFullInterruptEnable); //使能接收中断
	RT1052_NVIC_SetPriority(LPUART4_IRQn,6,0);				//抢占优先级6，子优先级0
	EnableIRQ(LPUART4_IRQn);	//使能LPUART1中断
#endif	

    RS485_TX_Set(0);                                        //设置为接收模式	
}

//RS485发送len个字节.
//buf:发送区首地址
//len:发送的字节数(为了和本代码的接收匹配,这里建议不要超过64个字节)
void RS485_Send_Data(u8 *buf,u8 len)
{
	RS485_TX_Set(1);			            //设置为发送模式
    LPUART_WriteBlocking(LPUART4,buf,len);  //串口4发送数据
	RS485_RX_CNT=0;	  
	RS485_TX_Set(0);			            //设置为接收模式	
}
//RS485查询接收到的数据
//buf:接收缓存首地址
//len:读到的数据长度
void RS485_Receive_Data(u8 *buf,u8 *len)
{
	u8 rxlen=RS485_RX_CNT;
	u8 i=0;
	*len=0;				//默认为0
	delay_ms(10);		//等待10ms,连续超过10ms没有接收到一个数据,则认为接收结束
	if(rxlen==RS485_RX_CNT&&rxlen)//接收到了数据,且接收完成了
	{
		for(i=0;i<rxlen;i++)
		{
			buf[i]=RS485_RX_BUF[i];	
		}		
		*len=RS485_RX_CNT;	//记录本次数据长度
		RS485_RX_CNT=0;		//清零
	}
} 
//RS485模式控制.
//en:0,接收;1,发送.
void RS485_TX_Set(u8 en)
{
	PCF8574_WriteBit(RS485_RE_IO,en);
}
