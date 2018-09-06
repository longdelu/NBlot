
/**
  *
  * @file           : demo_led_entry.c
  * @brief          : 板载LED灯以500MS为周期进行闪烁
  */
/* Includes ------------------------------------------------------------------*/

#include "led.h"
#include "delay.h"
#include "stm32l4xx_hal.h"

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
	  delay_ms(100);	  
  }
  /* USER CODE END 3 */

}




/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
