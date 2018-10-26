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


#define LED0 PBout(5)   //LED0
#define LED1 PEout(5)   //LED1
                      

#define LED0_Toggle (GPIO_ReadOutputDataBit(GPIOB,GPIO_Pin_5)? GPIO_ResetBits(GPIOB, GPIO_Pin_5): GPIO_SetBits(GPIOB, GPIO_Pin_5))
#define LED1_Toggle (GPIO_ReadOutputDataBit(GPIOE,GPIO_Pin_5)? GPIO_ResetBits(GPIOE, GPIO_Pin_5): GPIO_SetBits(GPIOE, GPIO_Pin_5))

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
