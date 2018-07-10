
/**
  *
  * @file           : demo_key_entry.c
  * @brief          : 按下板载对应的按键会有相关的调试信息输出
  */
/* Includes ------------------------------------------------------------------*/

#include "led.h"
#include "delay.h"
#include "key.h"
#include "stm32l4xx_hal.h"

void key_val_handle(u32 key_event,void *p_arg)
{   
  
    switch(key_event)
    {
        case KEY0_PRES://KEY0按下,读取sector
            printf("key0 press\r\n");
            
            break;
        
        case KEY1_PRES://KEY1按下,写入sector
            printf("key1 press\r\n");
            break;
        case KEY2_PRES://KEY2按下,恢复sector的数据
            printf("key2 press\r\n");
            break;
        
        case WKUP_PRES://KEY2按下,恢复sector的数据
            printf("WKUP_PRES press\r\n");
            break;        
        
    }   
    
}

/**
  * @brief  The led application entry point.
  *
  * @retval None
  */
void demo_key_entry(void)
{ 
  int key = 0;
  int key_mode = 1;
    
  key_init(key_mode); 

  if (key_mode == 1) {    
      key_registercb(key_val_handle, NULL);
  }
  
  while (1)
  {  
      if (key_mode == 0) {
          key = KEY_Scan(0); 
      
          key_val_handle(key, NULL);
          delay_ms(10); 
          
      } else {
          
          key_poll();
          
      }          
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
