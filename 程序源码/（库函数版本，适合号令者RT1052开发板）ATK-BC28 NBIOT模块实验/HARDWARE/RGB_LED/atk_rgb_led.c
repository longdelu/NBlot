#include "atk_rgb_led.h"

//////////////////////////////////////////////////////////////////////////////////     
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//P9813 RGB LED��������            
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2016/10/20
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved                                      
//////////////////////////////////////////////////////////////////////////////////  

 //sda�ߵ͵�ƽ����
void atk_sda_set(int leve)
{
    SDA(leve);          
}

//scl�ߵ͵�ƽ����
void atk_scl_set(int leve)
{
    SCL(leve);  
}


//����ǰ32λ'0'��ʼ֡
void RGBLED_Send32Zero(void)
{ 
     u8 i;
    
     atk_sda_set(0);
    
     for(i=0;i<32;i++)
     {
        atk_scl_set(0);
        delay_us(200);
        atk_scl_set(1);
        delay_us(200);        
     } 
}

//��תǰ��λ�ĻҶ�ֵ
u8 RGBLED_TakeAntiCode(u8 dat)
{
     u8 tmp = 0;
    
     tmp = ((~dat) & 0xC0) >> 6;
     return tmp;
}

//���ͻҶ�����
void RGBLED_DatSend(u32 dx)
{
     u8 i;
     
     for(i=0;i<32;i++)
     {
        if((dx & 0x80000000) !=0)
           atk_sda_set(1);
        else 
         atk_sda_set(0);
        
        dx<<=1;
        atk_scl_set(0);delay_us(200);
        atk_scl_set(1);delay_us(200);
     }    
}

//���ݴ����뷢��
//r;��ɫֵ 0~255
//g:��ɫֵ 0~255
//b:��ɫֵ 0~255  
void RGBLED_DataDealWithAndSend(u8 r,u8 g,u8 b)
{ 
     u32 dx=0;
    
     dx |= (u32)0x03 << 30;  //ǰ��λ'1'λ��־λ
     dx |= (u32)RGBLED_TakeAntiCode(b) << 28;
     dx |= (u32)RGBLED_TakeAntiCode(g) << 26;
     dx |= (u32)RGBLED_TakeAntiCode(r) << 24;
    
     dx |= (u32)b << 16;
     dx |= (u32)g << 8;
     dx |= r;
    
     RGBLED_DatSend(dx);//��������
}
//RGB�ƿ��ƺ���
//r;��ɫֵ 0~255
//g:��ɫֵ 0~255
//b:��ɫֵ 0~255
void RGBLED_Show(u8 r,u8 g,u8 b)
{
     RGBLED_Send32Zero();//����ǰ32λ'0'��ʼ֡
     RGBLED_DataDealWithAndSend(r, g, b);//����32bit�Ҷ�����
     RGBLED_DataDealWithAndSend(r, g, b);
} 

//RGB�Ƴ�ʼ������
void RGBLED_Init(void) 
{
    gpio_pin_config_t led_config;
    
    //IO��������
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_03_GPIO1_IO19,0);    //GPIO_AD_B0_03����ΪALT5,��GPIO1_19
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_09_GPIO1_IO09,0);    //GPIO_SD_B1_03����ΪALT5,��GPIO1_09
    
    //����IO����GPIO_AD_B1_03��GPIO_AD_B0_09�Ĺ���
    //��ת���ٶ�,��������ΪR0/6,�ٶ�Ϊ100Mhz���رտ�·���ܣ�ʹ��pull/keepr
    //ѡ��keeper���ܣ�����100K Ohm���ر�Hyst
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_03_GPIO1_IO19, 0x10B0);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_09_GPIO1_IO09, 0x10B0);

    led_config.direction=kGPIO_DigitalOutput;    //���
    led_config.interruptMode=kGPIO_NoIntmode;    //��ʹ���жϹ���
    led_config.outputLogic=1;                    //Ĭ�ϸߵ�ƽ��LED�ƹر�
    GPIO_PinInit(GPIO1,19, &led_config);          //��ʼ��GPIO1_19
    
    led_config.direction=kGPIO_DigitalOutput;    //���
    led_config.interruptMode=kGPIO_NoIntmode;    //��ʹ���жϹ���
    led_config.outputLogic=1;                    //Ĭ�ϸߵ�ƽ��LED�ƹر�
    GPIO_PinInit(GPIO1,9,&led_config);           //��ʼ��GPIO1_09
}

