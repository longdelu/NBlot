
/*
 * Copyright (c) 广州市星翼电子科技有限公司 2014-2024
 * All rights reserved 
 * @file           : demo_timer_entry.c
 * @brief          : 板载LED灯以500MS为周期进行闪烁
 */

#include "atk_led.h"
#include "atk_delay.h"
#include "atk_timer.h"
#include "stm32f4xx_hal.h"

/**
  * @brief  The timer timing application entry point.
  *
  * @retval None
  */
void demo_timer_timing_entry(void)
{   
    //计数频率为90M/900=100000HZ, 然后计数周期为50000个计数,为500ms    
    atk_tim3_init(50000 - 1, 900 -1);

    while (1)
    { 
       LED1_Toggle; 
       delay_ms(500);
    }
}




