
/************************************************
 Copyright (c) 广州市星翼电子科技有限公司 2014-2024
 All rights reserved 
 ALIENTEK 阿波罗STM32F429开发板 
 NBIOT定时器定时实验
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/ 

#include "atk_led.h"
#include "atk_delay.h"
#include "atk_timer.h"
#include "atk_bsp.h"

/**
  * @brief  The timer timing application entry point.
  * @retval None
  */
void demo_timer_timing_entry(void)
{   
    //计数频率为200M/2000=100000HZ, 然后计数周期为50000个计数,为500ms    
    atk_tim3_init(50000-1,2000-1);

    while (1)
    { 
       LED1_Toggle; 
       delay_ms(500);
    }
}




