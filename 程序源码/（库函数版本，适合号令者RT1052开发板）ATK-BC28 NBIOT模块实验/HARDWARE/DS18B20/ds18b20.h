#ifndef __DS18B20_H
#define __DS18B20_H
#include "sys.h"
#include "atk_bsp.h"
//////////////////////////////////////////////////////////////////////////////////     
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK I.MX RT1052������
//DS18B20��������       
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/12/30
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved                                      
//////////////////////////////////////////////////////////////////////////////////     

//IO��������
//#define DS18B20_IO_IN()  {GPIOB->MODER&=~(3<<(12*2));GPIOB->MODER|=0<<12*2;}    //PB12����ģʽ
//#define DS18B20_IO_OUT() {GPIOB->MODER&=~(3<<(12*2));GPIOB->MODER|=1<<12*2;}     //PB12���ģʽ

#define DS18B20_IO_IN()                                             \
{                                                                   \
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_11_GPIO1_IO11,0);            \
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_11_GPIO1_IO11,0xF080);    \
    gpio_pin_config_t datain_config;                                \
    datain_config.direction=kGPIO_DigitalInput;                     \
    datain_config.interruptMode=kGPIO_NoIntmode;                    \
    datain_config.outputLogic=1;                                    \
    GPIO_PinInit(GPIO1,11,&datain_config);                          \
}

//SDA GPIO_B1_12���ģʽ
#define DS18B20_IO_OUT()                                            \
{                                                                   \
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_11_GPIO1_IO11,0);            \
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_11_GPIO1_IO11,0x10B0);    \
    gpio_pin_config_t dataout_config;                               \
    dataout_config.direction=kGPIO_DigitalOutput;                   \
    dataout_config.interruptMode=kGPIO_NoIntmode;                   \
    dataout_config.outputLogic=1;                                   \
    GPIO_PinInit(GPIO1,11,&dataout_config);                         \
}

//IO��������                                               
#define    DS18B20_DQ_OUT(n)   (n?GPIO_PinWrite(GPIO1,11,1):GPIO_PinWrite(GPIO1,11,0)) //���ݶ˿�
#define    DS18B20_DQ_IN       GPIO_PinRead(GPIO1,11)  //���ݶ˿�    
       
u8 DS18B20_Init(void);            //��ʼ��DS18B20
short DS18B20_Get_Temp(void);    //��ȡ�¶�
void DS18B20_Start(void);        //��ʼ�¶�ת��
void DS18B20_Write_Byte(u8 dat);//д��һ���ֽ�
u8 DS18B20_Read_Byte(void);        //����һ���ֽ�
u8 DS18B20_Read_Bit(void);        //����һ��λ
u8 DS18B20_Check(void);            //����Ƿ����DS18B20
void DS18B20_Rst(void);            //��λDS18B20 
#endif
