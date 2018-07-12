
/**
  *
  * @file           : demo_timer_entry.c
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
  
    //计数频率为64M/640=100000HZ, 然后计数周期为50000个计数，为1S的一半    
    TIM7_Init(50000 - 1, 640 -1);

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
