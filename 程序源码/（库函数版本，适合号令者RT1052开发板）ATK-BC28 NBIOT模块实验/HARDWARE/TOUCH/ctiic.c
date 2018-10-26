#include "ctiic.h"
#include "delay.h"	 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//电容触摸屏-IIC 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/12/28
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
//********************************************************************************
//无
////////////////////////////////////////////////////////////////////////////////// 	

//控制I2C速度的延时
void CT_Delay(void)
{
	delay_us(2);
} 
//电容触摸芯片IIC接口初始化
void CT_IIC_Init(void)
{	
    gpio_pin_config_t ctiic_config;
    
	//IO功能设置
    IOMUXC_SetPinMux(IOMUXC_GPIO_B1_12_GPIO2_IO28,0);	    //SDA 
    IOMUXC_SetPinMux(IOMUXC_GPIO_B1_13_GPIO2_IO29,0);       //SCL
    
	IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_12_GPIO2_IO28,0x10B0);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_13_GPIO2_IO29,0x10B0);

	ctiic_config.direction=kGPIO_DigitalOutput;	//输出
	ctiic_config.interruptMode=kGPIO_NoIntmode;	//不使用中断功能
	ctiic_config.outputLogic=1;					//默认高电平
	GPIO_PinInit(GPIO2,28,&ctiic_config); 	    
    
    ctiic_config.direction=kGPIO_DigitalOutput;	//输出
	ctiic_config.interruptMode=kGPIO_NoIntmode;	//不使用中断功能
	ctiic_config.outputLogic=1;					//默认高电平
	GPIO_PinInit(GPIO2,29,&ctiic_config);         
}
//产生IIC起始信号
void CT_IIC_Start(void)
{
	CT_SDA_OUT();     //sda线输出
	CT_IIC_SDA(1);	  	  
	CT_IIC_SCL(1);
	delay_us(30);
 	CT_IIC_SDA(0);//START:when CLK is high,DATA change form high to low 
	CT_Delay();
	CT_IIC_SCL(0);//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void CT_IIC_Stop(void)
{
	CT_SDA_OUT();//sda线输出
	CT_IIC_SCL(1);
	delay_us(30);
	CT_IIC_SDA(0);//STOP:when CLK is high DATA change form low to high
	CT_Delay();
	CT_IIC_SDA(1);//发送I2C总线结束信号  
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 CT_IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	CT_SDA_IN();      //SDA设置为输入  
	CT_IIC_SDA(1);	   
	CT_IIC_SCL(1);
	CT_Delay();
	while(CT_READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			CT_IIC_Stop();
			return 1;
		} 
		CT_Delay();
	}
	CT_IIC_SCL(0);//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void CT_IIC_Ack(void)
{
	CT_IIC_SCL(0);
	CT_SDA_OUT();
	CT_Delay();
	CT_IIC_SDA(0);
	CT_Delay();
	CT_IIC_SCL(1);
	CT_Delay();
	CT_IIC_SCL(0);
}
//不产生ACK应答		    
void CT_IIC_NAck(void)
{
	CT_IIC_SCL(0);
	CT_SDA_OUT();
	CT_Delay();
	CT_IIC_SDA(1);
	CT_Delay();
	CT_IIC_SCL(1);
	CT_Delay();
	CT_IIC_SCL(0);
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void CT_IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	CT_SDA_OUT(); 	    
    CT_IIC_SCL(0);//拉低时钟开始数据传输
	CT_Delay();
	for(t=0;t<8;t++)
    {              
        CT_IIC_SDA((txd&0x80)>>7);
        txd<<=1; 	      
		CT_IIC_SCL(1); 
		CT_Delay();
		CT_IIC_SCL(0);	
		CT_Delay();
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 CT_IIC_Read_Byte(unsigned char ack)
{
	u8 i,receive=0;
 	CT_SDA_IN();//SDA设置为输入
	delay_us(30);
	for(i=0;i<8;i++ )
	{ 
		CT_IIC_SCL(0); 	    	   
		CT_Delay();
		CT_IIC_SCL(1);	 
		receive<<=1;
		if(CT_READ_SDA)receive++;   
	}	  				 
	if (!ack)CT_IIC_NAck();//发送nACK
	else CT_IIC_Ack(); //发送ACK   
 	return receive;
}




























