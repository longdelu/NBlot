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
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOB_CLK_ENABLE();           //����GPIOBʱ��

    GPIO_Initure.Pin=GPIO_PIN_0|GPIO_PIN_1; //PB1,0
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
    GPIO_Initure.Pull=GPIO_PULLUP;                  //����
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;   //����
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);

    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);    //PB0��1 
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET);    //PB1��1  
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

