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
#define LED0(n)        (n ? GPIO_PinWrite(GPIO1,3,1):GPIO_PinWrite(GPIO1,3,0))
#define LED0_Toggle    (GPIO1->DR ^= (1<<3))     //LED0�����ƽ��ת

#define LED1(n)        (n ? GPIO_PinWrite(GPIO3,3,1):GPIO_PinWrite(GPIO3,3,0))
#define LED1_Toggle    (GPIO3->DR ^= (1<<3))     //LED1�����ƽ��ת

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
