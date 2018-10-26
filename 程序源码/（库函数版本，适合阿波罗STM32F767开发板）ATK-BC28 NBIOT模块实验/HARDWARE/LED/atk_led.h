#ifndef _ATK_LED_H
#define _ATK_LED_H
#include "atk_sys.h"
//////////////////////////////////////////////////////////////////////////////////     
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F7������
//LED��������       
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/6/10
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved                                      
////////////////////////////////////////////////////////////////////////////////// 


//LED�˿ڶ���
#define LED0(n)     (n?HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET))
#define LED1(n)     (n?HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET))

#define LED0_Toggle (HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_1))
#define LED1_Toggle (HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0))

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
