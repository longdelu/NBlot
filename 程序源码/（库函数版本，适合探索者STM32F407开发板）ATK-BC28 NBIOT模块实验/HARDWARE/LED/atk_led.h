#ifndef _ATK_LED_H
#define _ATK_LED_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////     
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//LED��������       
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/9/7
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved                                      
//////////////////////////////////////////////////////////////////////////////////     

#define LED0 PFout(9)       //LED0
#define LED1 PFout(10)       //LED1

#define LED0_Toggle (HAL_GPIO_TogglePin(GPIOF, GPIO_PIN_9))
#define LED1_Toggle (HAL_GPIO_TogglePin(GPIOF, GPIO_PIN_10))

/**
  * @brief  led��ʼ��
  * @param  None
  * @retval None
  */
void atk_led_init(void);


/**
  * @brief  led��
  * @param  led_id  
  * @retval None
  */
void atk_led_on(int led_id);


/**
  * @brief  led��
  * @param  led_id
  * @retval None
  */
void atk_led_off(int led_id);

/**
  * @brief  led��ת
  * @param  led_id
  * @retval None
  */
void atk_led_toggle(int led_id);


#endif
