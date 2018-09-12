
/**
 * Copyright (c) 广州市星翼电子科技有限公司 2014-2024
 * All rights reserved 
 * @file           : demo_key_entry.c
 * @brief          : 按下板载对应的按键会有相关的调试信息输出
 */

#include "atk_led.h"
#include "atk_delay.h"
#include "atk_key.h"
#include "stm32f4xx_hal.h"

static void key_event_handle(u32 key_event,void *p_arg)
{     
    switch(key_event)
    {
        case KEY0_PRES: //KEY0按下,读取sector                        
            printf("key0 press\r\n");            
            break;
        
        case KEY1_PRES: //KEY1按下,写入sector
            printf("key1 press\r\n");
            break;
        case KEY2_PRES: //KEY2按下,恢复sector的数据
            printf("key2 press\r\n");
            break;
        
        case WKUP_PRES: //KEY2按下,恢复sector的数据
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
    key_handle_t  key_handle = NULL;

    key_handle = atk_key_exit_init(); 

    atk_key_registercb(key_handle, key_event_handle, NULL);
  
    atk_key_registercb(key_handle, key_event_handle, NULL);
    

    while (1)
    {  
          
        atk_key_event_poll(key_handle);
                            
    }
}




