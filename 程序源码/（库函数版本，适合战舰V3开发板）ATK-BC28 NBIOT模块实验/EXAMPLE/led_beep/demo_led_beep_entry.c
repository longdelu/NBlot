
/************************************************
 Copyright (c) 广州市星翼电子科技有限公司 2014-2024
 All rights reserved 
 ALIENTEK 阿波罗STM32F429开发板 
 NBIOT多路控制测试实验 
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/ 

#include "atk_led.h"
#include "atk_delay.h"
#include "atk_bsp.h"

/**
  * @brief  The led beep application entry point.
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
