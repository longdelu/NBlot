#ifndef _ATK_LED_H
#define _ATK_LED_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////     
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//LED驱动代码       
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/9/7
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved                                      
//////////////////////////////////////////////////////////////////////////////////     

#define LED0 PFout(9)       //LED0
#define LED1 PFout(10)       //LED1

#define LED0_Toggle (HAL_GPIO_TogglePin(GPIOF, GPIO_PIN_9))
#define LED1_Toggle (HAL_GPIO_TogglePin(GPIOF, GPIO_PIN_10))

/**
  * @brief  led初始化
  * @param  None
  * @retval None
  */
void atk_led_init(void);


/**
  * @brief  led亮
  * @param  led_id  
  * @retval None
  */
void atk_led_on(int led_id);


/**
  * @brief  led灭
  * @param  led_id
  * @retval None
  */
void atk_led_off(int led_id);

/**
  * @brief  led翻转
  * @param  led_id
  * @retval None
  */
void atk_led_toggle(int led_id);


#endif
