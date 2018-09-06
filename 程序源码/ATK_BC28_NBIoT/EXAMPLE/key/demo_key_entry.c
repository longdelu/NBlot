
/**
  *
  * @file           : demo_key_entry.c
  * @brief          : 按下板载对应的按键会有相关的调试信息输出
  */
/* Includes ------------------------------------------------------------------*/

#include "atk_led.h"
#include "atk_delay.h"
#include "atk_key.h"
#include "stm32l4xx_hal.h"

static void key_event_handle(u32 key_event,void *p_arg)
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

	key_handle_t  key_handle = NULL;

	key_handle = key_init(1); 

	atk_key_registercb(key_handle, key_event_handle, NULL);

	if (key_mode == 1) {    
	    atk_key_registercb(key_handle, key_event_handle, NULL);
	}

	while (1)
	{  
		if (key_mode == 0) {
		  key = key_scan(0); 

		  key_event_handle(key, NULL);
		  delay_ms(10);          
		} else {
		  
		  atk_key_event_poll(key_handle);
		  
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
