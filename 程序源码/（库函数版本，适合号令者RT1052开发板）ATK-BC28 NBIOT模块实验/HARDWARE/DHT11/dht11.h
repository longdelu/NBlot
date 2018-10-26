#ifndef __DS18B20_H
#define __DS18B20_H
#include "sys.h"
#include "atk_bsp.h"
//////////////////////////////////////////////////////////////////////////////////     
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK I.MX RT1052������
//DHT11��������       
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/12/30
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved                                      
//////////////////////////////////////////////////////////////////////////////////     

//IO��������
#define DHT11_IO_IN()                                               \
{                                                                   \
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_11_GPIO1_IO11,0);            \
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_11_GPIO1_IO11,0xF080);    \
    gpio_pin_config_t datain_config;                                \
    datain_config.direction=kGPIO_DigitalInput;                     \
    datain_config.interruptMode=kGPIO_NoIntmode;                    \
    datain_config.outputLogic=1;                                    \
    GPIO_PinInit(GPIO1,11,&datain_config);                          \
}

#define DHT11_IO_OUT()                                              \
{                                                                   \
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_11_GPIO1_IO11,0);            \
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_11_GPIO1_IO11,0x10B0);    \
    gpio_pin_config_t dataout_config;                               \
    dataout_config.direction=kGPIO_DigitalOutput;                   \
    dataout_config.interruptMode=kGPIO_NoIntmode;                   \
    dataout_config.outputLogic=1;                                   \
    GPIO_PinInit(GPIO1,11,&dataout_config);                         \
}

#define    DHT11_DQ_OUT(n) (n?GPIO_PinWrite(GPIO1,11,1):GPIO_PinWrite(GPIO1,11,0)) //���ݶ˿�
#define    DHT11_DQ_IN     GPIO_PinRead(GPIO1,11) //���ݶ˿�
       
u8 DHT11_Init(void);//��ʼ��DHT11
u8 DHT11_Read_Data(u8 *temp,u8 *humi);//��ȡ��ʪ��
u8 DHT11_Read_Byte(void);//����һ���ֽ�
u8 DHT11_Read_Bit(void);//����һ��λ
u8 DHT11_Check(void);//����Ƿ����DHT11
void DHT11_Rst(void);//��λDHT11  
#endif
