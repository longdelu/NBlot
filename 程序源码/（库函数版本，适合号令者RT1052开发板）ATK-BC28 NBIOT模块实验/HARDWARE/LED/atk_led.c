#include "atk_led.h"
//////////////////////////////////////////////////////////////////////////////////     
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F7������
//LED��������       
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/11/27
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved                                      
//////////////////////////////////////////////////////////////////////////////////     
          

/**
  * @brief  led��ʼ��
  * @param  None
  * @retval None
  */
void atk_led_init(void)
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



/**
  * @brief  led��
  * @param  led_id  
  * @retval None
  */
void atk_led_on(int led_id)
{
  
    if (0 == led_id)
    {
       LED0(0);        
        
    }
    
    else 
    {
      
       LED1(0);  
    }

}


/**
  * @brief  led��
  * @param  led_id
  * @retval None
  */
void atk_led_off(int led_id)
{
    if (0 == led_id)
    {
       LED0(1);        
        
    }
    
    else 
    {
       LED1(1);        
      
    }
}

/**
  * @brief  led��ת
  * @param  led_id
  * @retval None
  */
void atk_led_toggle(int led_id)
{
    if (0 == led_id)
    {
      
       LED0_Toggle; 
    }
    
    else 
    {
      
       LED1_Toggle;
    }
}

