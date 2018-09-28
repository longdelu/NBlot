
/************************************************
 Copyright (c) 广州市星翼电子科技有限公司 2014-2024
 All rights reserved 
 ALIENTEK 阿波罗STM32F429开发板 
 NBIOT按键事件驱动实验
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/ 

#include "atk_led.h"
#include "atk_delay.h"
#include "atk_key.h"
#include "atk_bsp.h"

static void __key_event_handle(u32 key_event,void *p_arg)
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

    atk_key_registercb(key_handle, __key_event_handle, NULL);
  
    atk_key_registercb(key_handle, __key_event_handle, NULL);
    
    while (1)
    {  
          
        atk_key_event_poll(key_handle);
                            
    }
}




