
/**
  *
  * @file           : demo_led_entry.c
  * @brief          : 板载LED灯以500MS为周期进行闪烁
  */
/* Includes ------------------------------------------------------------------*/

#include "led.h"
#include "delay.h"
#include "timer.h"
#include "stm32l4xx_hal.h"

/**
  * @brief  The timer timing application entry point.
  *
  * @retval None
  */
void demo_timer_timing_entry(void)
{ 
    
  TIM7_Init(50000 - 1, 720 -1);
    
  while (1)
  { 
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
