#include "lpuart.h"
//////////////////////////////////////////////////////////////////////////////////      
//如果使用os,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"                    //os 使用      
#endif
//////////////////////////////////////////////////////////////////////////////////     
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK I.MX RT1052开发板
//串口1初始化           
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2017/12/10
//版本：V1.5
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//********************************************************************************
//V1.0修改说明 
//////////////////////////////////////////////////////////////////////////////////      

static lpuart_config_t lpuart1_config;

//加入以下代码,支持printf函数,而不需要选择use MicroLIB      
//#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)    
#if 1
//#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
    int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
    x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{     
    while((LPUART1->STAT&LPUART_STAT_TDRE_MASK)==0);
    LPUART1->DATA=(u8)ch;    
    
    return ch;
}
#endif 

#if EN_LPUART1_RX                        //如果使能了接收,则编译以下代码
//串口1中断服务程序      
u8 LPUART_RX_BUF[LPUART_REC_LEN];     //接收缓冲,最大LPUART_REC_LEN个字节.
//接收状态
//bit15，    接收完成标志
//bit14，    接收到0x0d
//bit13~0，    接收到的有效字节数目
u16 LPUART_RX_STA=0;       //接收状态标记    

//LPUART1中断服务函数，为了接收效率直接操作寄存器
void LPUART1_IRQHandler(void)
{
    u8 res=0;
    
    if((LPUART1->STAT)&kLPUART_RxDataRegFullFlag) //接收中断
    {
        res=LPUART1->DATA;                    //读取数据
        if((LPUART_RX_STA&0x8000)==0)        //接收未完成
        {
            if(LPUART_RX_STA&0x4000)            //接收到了0x0d
            {
                if(res!=0x0a)LPUART_RX_STA=0;//接收错误,重新开始
                else LPUART_RX_STA|=0x8000;    //接收完成了 
            }
            else                             //还没收到0X0D
            {    
                if(res==0x0d)LPUART_RX_STA|=0x4000;
                else
                {
                    LPUART_RX_BUF[LPUART_RX_STA&0X3FFF]=res ;
                    LPUART_RX_STA++;
                    if(LPUART_RX_STA>(LPUART_REC_LEN-1))LPUART_RX_STA=0;//接收数据错误,重新开始接收      
                }         
            }
        } 
    }
    __DSB();                //数据同步屏蔽指令
}

//void LPUART1_IRQHandler(void)
//{
//    u8 res=0;
//    
//    if(LPUART_GetStatusFlags(LPUART1)&kLPUART_RxDataRegFullFlag) //接收中断
//    {
//        res=LPUART_ReadByte(LPUART1);          //读取数据
//        if((LPUART_RX_STA&0x8000)==0)        //接收未完成
//        {
//            if(LPUART_RX_STA&0x4000)            //接收到了0x0d
//            {
//                if(res!=0x0a)LPUART_RX_STA=0;//接收错误,重新开始
//                else LPUART_RX_STA|=0x8000;    //接收完成了 
//            }
//            else                             //还没收到0X0D
//            {    
//                if(res==0x0d)LPUART_RX_STA|=0x4000;
//                else
//                {
//                    LPUART_RX_BUF[LPUART_RX_STA&0X3FFF]=res ;
//                    LPUART_RX_STA++;
//                    if(LPUART_RX_STA>(LPUART_REC_LEN-1))LPUART_RX_STA=0;//接收数据错误,重新开始接收      
//                }         
//            }
//        } 
//    }
//}
#endif    

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

//初始化IO 串口1 
//bound:波特率
void LPUART1_Init(u32 bound)
{    
    u32 freq=0;                            //串口的时钟源频率
    
    CLOCK_EnableClock(kCLOCK_Lpuart1);    //使能LPUART1时钟
    
    CLOCK_SetMux(kCLOCK_UartMux,0);     //设置UART时钟源为PLL3 80Mhz，PLL3/6=480/6=80MHz
    CLOCK_SetDiv(kCLOCK_UartDiv,0);     //设置UART时钟1分频，即UART时钟为80Mhz
    
    //LPUART1所使用的IO功能配置，即：从ALT0~ALT7选择合适的功能。
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_12_LPUART1_TX,0U);    //GPIO_AD_B0_12设置为LPUART1_TX
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_13_LPUART1_RX,0U);    //GPIO_AD_B0_13设置为LPUART1_RX

    //配置IO引脚GPIO_AD_B0_12和GPIO_AD_B0_13的功能
    //低转换速度,驱动能力为R0/6,速度为100Mhz，关闭开路功能，使能pull/keepr
    //选择keeper功能，下拉100K Ohm，关闭Hyst
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_12_LPUART1_TX,0x10B0u); 
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_13_LPUART1_RX,0x10B0u); 

    freq=LPUART_SrcFreqGet();    
    
    LPUART_GetDefaultConfig(&lpuart1_config);                 //先设置为默认配置，后面在根据实际情况配置
    
    //初始化NXP官方提供的debug console，此函数会重新初始化LPUART1，但是我们后面会
    //重新显示的初始化一次LPUART1，DbgConsole_Init()主要是给那些想要使用NXP官方
    //调试功能的开发者使用的，不需要使用的话就可以将下面代码注释掉
    DbgConsole_Init(BOARD_DEBUG_UART_BASEADDR,bound,BOARD_DEBUG_UART_TYPE,freq);
    
    lpuart1_config.baudRate_Bps=bound;                        //波特率
    lpuart1_config.dataBitsCount=kLPUART_EightDataBits;        //8位
    lpuart1_config.stopBitCount=kLPUART_OneStopBit;            //1位停止位
    lpuart1_config.parityMode=kLPUART_ParityDisabled;        //无奇偶校验
    lpuart1_config.enableRx=true;                            //使能接收
    lpuart1_config.enableTx=true;                            //使能发送
    
    LPUART_Init(LPUART1,&lpuart1_config,freq);                //初始化LPUART1 
    
#if EN_LPUART1_RX                                            //是否需要开启中断?    
    //LPUART中断设置
    LPUART_EnableInterrupts(LPUART1,kLPUART_RxDataRegFullInterruptEnable); //使能接收中断
    RT1052_NVIC_SetPriority(LPUART1_IRQn,5,0);                //抢占优先级5，子优先级0
    EnableIRQ(LPUART1_IRQn);                                //使能LPUART1中断
#endif
}    
//void LPUART1_IRQHandler(void)
//{
//    u8 res=0;
//    
//    if(LPUART_GetStatusFlags(LPUART1)&kLPUART_RxDataRegFullFlag) //接收中断
//    {
//        res=LPUART_ReadByte(LPUART1);          //读取数据
//        if((LPUART_RX_STA&0x8000)==0)        //接收未完成
//        {
//            if(LPUART_RX_STA&0x4000)            //接收到了0x0d
//            {
//                if(res!=0x0a)LPUART_RX_STA=0;//接收错误,重新开始
//                else LPUART_RX_STA|=0x8000;    //接收完成了 
//            }
//            else                             //还没收到0X0D
//            {    
//                if(res==0x0d)LPUART_RX_STA|=0x4000;
//                else
//                {
//                    LPUART_RX_BUF[LPUART_RX_STA&0X3FFF]=res ;
//                    LPUART_RX_STA++;
//                    if(LPUART_RX_STA>(LPUART_REC_LEN-1))LPUART_RX_STA=0;//接收数据错误,重新开始接收      
//                }         
//            }
//        } 
//    }
//}








 




