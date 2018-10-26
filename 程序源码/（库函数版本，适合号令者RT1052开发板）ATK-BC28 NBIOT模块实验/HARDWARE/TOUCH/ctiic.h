#ifndef __MYCT_IIC_H
#define __MYCT_IIC_H
#include "sys.h"	    
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//���ݴ�����-IIC ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/12/30
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
//********************************************************************************
//��
////////////////////////////////////////////////////////////////////////////////// 	

//IO��������
//SDA GPIO_B1_12����ģʽ
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

//SDA GPIO_B1_12���ģʽ
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


//IO��������	 
#define CT_IIC_SCL(n) (n?GPIO_PinWrite(GPIO2,29,1):GPIO_PinWrite(GPIO2,29,0))//SCL
#define CT_IIC_SDA(n) (n?GPIO_PinWrite(GPIO2,28,1):GPIO_PinWrite(GPIO2,28,0))//SDA	 

#define CT_READ_SDA   GPIO_PinRead(GPIO2,28)//����SDA  

//IIC���в�������
void CT_IIC_Init(void);                	//��ʼ��IIC��IO��				 
void CT_IIC_Start(void);				//����IIC��ʼ�ź�
void CT_IIC_Stop(void);	  				//����IICֹͣ�ź�
void CT_IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 CT_IIC_Read_Byte(unsigned char ack);	//IIC��ȡһ���ֽ�
u8 CT_IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void CT_IIC_Ack(void);					//IIC����ACK�ź�
void CT_IIC_NAck(void);					//IIC������ACK�ź�

#endif







