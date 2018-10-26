#ifndef __MYCT_IIC_H
#define __MYCT_IIC_H
#include "sys.h"	    
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//电容触摸屏-IIC 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/12/30
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
//********************************************************************************
//无
////////////////////////////////////////////////////////////////////////////////// 	

//IO方向设置
//SDA GPIO_B1_12输入模式
#define CT_SDA_IN()                                             \
{                                                               \
    IOMUXC_SetPinMux(IOMUXC_GPIO_B1_12_GPIO2_IO28,0);           \
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_12_GPIO2_IO28,0xF080);   \
    gpio_pin_config_t sdain_config;                             \
    sdain_config.direction=kGPIO_DigitalInput;                  \
    sdain_config.interruptMode=kGPIO_NoIntmode;                 \
    sdain_config.outputLogic=1;                                 \
    GPIO_PinInit(GPIO2,28,&sdain_config);                       \
}

//SDA GPIO_B1_12输出模式
#define CT_SDA_OUT()                                            \
{                                                               \
    IOMUXC_SetPinMux(IOMUXC_GPIO_B1_12_GPIO2_IO28,0);           \
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_12_GPIO2_IO28,0x10B0);   \
    gpio_pin_config_t sdaout_config;                            \
    sdaout_config.direction=kGPIO_DigitalOutput;                \
    sdaout_config.interruptMode=kGPIO_NoIntmode;                \
    sdaout_config.outputLogic=1;                                \
    GPIO_PinInit(GPIO2,28,&sdaout_config);                      \
}


//IO操作函数	 
#define CT_IIC_SCL(n) (n?GPIO_PinWrite(GPIO2,29,1):GPIO_PinWrite(GPIO2,29,0))//SCL
#define CT_IIC_SDA(n) (n?GPIO_PinWrite(GPIO2,28,1):GPIO_PinWrite(GPIO2,28,0))//SDA	 

#define CT_READ_SDA   GPIO_PinRead(GPIO2,28)//输入SDA  

//IIC所有操作函数
void CT_IIC_Init(void);                	//初始化IIC的IO口				 
void CT_IIC_Start(void);				//发送IIC开始信号
void CT_IIC_Stop(void);	  				//发送IIC停止信号
void CT_IIC_Send_Byte(u8 txd);			//IIC发送一个字节
u8 CT_IIC_Read_Byte(unsigned char ack);	//IIC读取一个字节
u8 CT_IIC_Wait_Ack(void); 				//IIC等待ACK信号
void CT_IIC_Ack(void);					//IIC发送ACK信号
void CT_IIC_NAck(void);					//IIC不发送ACK信号

#endif







