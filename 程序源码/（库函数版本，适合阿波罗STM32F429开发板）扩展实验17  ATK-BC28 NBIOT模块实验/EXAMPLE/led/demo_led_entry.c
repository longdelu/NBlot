/**
 * Copyright (c) 广州市星翼电子科技有限公司 2014-2024
 * All rights reserved 
 *
 * @file:  demo_led_entry.c
 * @brief 板载LED灯以500MS为周期进行闪烁
 */

#include "atk_led.h"
#include "atk_delay.h"
#include "stm32f4xx_hal.h"

/**
  * @brief  The led application entry point.
  *
  * @retval None
  */
void demo_led_entry(void)
{
    
  while (1)
  {
      LED0_Toggle;  
      delay_ms(500);      
  }

}




/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
