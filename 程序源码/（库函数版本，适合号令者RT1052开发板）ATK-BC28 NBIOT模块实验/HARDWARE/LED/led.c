#include "led.h"
//////////////////////////////////////////////////////////////////////////////////     
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK I.MX RT1052������
//LED��������       
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2012/12/10
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved                                      
//////////////////////////////////////////////////////////////////////////////////     

//��ʼ��LED   
//LED IO��ʼ��
void LED_Init(void)
{
    gpio_pin_config_t led_config;
    
    //IO��������
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_03_GPIO1_IO03,0);    //GPIO_AD_B0_03����ΪALT5,��GPIO1_3
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_03_GPIO3_IO03,0);    //GPIO_SD_B1_03����ΪALT5,��GPIO3_3
    
    //����IO����GPIO_AD_B0_03��GPIO_SD_B1_03�Ĺ���
    //��ת���ٶ�,��������ΪR0/6,�ٶ�Ϊ100Mhz���رտ�·���ܣ�ʹ��pull/keepr
    //ѡ��keeper���ܣ�����100K Ohm���ر�Hyst
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_03_GPIO1_IO03,0x10B0);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_03_GPIO3_IO03,0x10B0);

    led_config.direction=kGPIO_DigitalOutput;    //���
    led_config.interruptMode=kGPIO_NoIntmode;    //��ʹ���жϹ���
    led_config.outputLogic=1;                    //Ĭ�ϸߵ�ƽ��LED�ƹر�
    GPIO_PinInit(GPIO1,3,&led_config);             //��ʼ��GPIO1_3
    
    led_config.direction=kGPIO_DigitalOutput;    //���
    led_config.interruptMode=kGPIO_NoIntmode;    //��ʹ���жϹ���
    led_config.outputLogic=1;                    //Ĭ�ϸߵ�ƽ��LED�ƹر�
    GPIO_PinInit(GPIO3,3,&led_config);           //��ʼ��GPIO3_3
}
