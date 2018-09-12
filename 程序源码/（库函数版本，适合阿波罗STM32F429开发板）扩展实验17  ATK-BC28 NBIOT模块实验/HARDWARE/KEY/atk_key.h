#ifndef _ATK_KEY_H
#define _ATK_KEY_H
#include "atk_sys.h"
//////////////////////////////////////////////////////////////////////////////////     
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F7开发板
//KEY驱动代码       
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/11/27
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved                                      
//////////////////////////////////////////////////////////////////////////////////

#define KEY0        HAL_GPIO_ReadPin(GPIOH,GPIO_PIN_3)  //KEY0按键PH3
#define KEY1        HAL_GPIO_ReadPin(GPIOH,GPIO_PIN_2)  //KEY1按键PH2
#define KEY2        HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13) //KEY2按键PC13
#define WK_UP       HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)  //WKUP按键PA0

#define KEY0_PRES   1
#define KEY1_PRES   2
#define KEY2_PRES   4
#define WKUP_PRES   8

//按键消抖时延
#define KEY_DELAY_TICK   15

//定义按键回调函数指针
typedef void (*key_cb)(u32 key_event, void *p_arg);

//按键结构体
typedef struct key_dev
{
    uint8_t key_event;
    
    int     start_tick;
    
    key_cb  key_cb;
    
    void   *p_arg;       
    
}key_dev_t;

//按键设备句柄
typedef key_dev_t *key_handle_t;

key_handle_t atk_key_exit_init(void);


//注册按键事件回调
void atk_key_registercb (key_handle_t key_handle, key_cb cb, void *p_arg);

// 轮询按钮事件
void atk_key_event_poll(key_handle_t key_handle);

#endif
