#ifndef _KEY_H
#define _KEY_H
#include "sys.h"
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
#define KEY0        HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_3)  //KEY0按键PC3
#define KEY1        HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_2)  //KEY1按键PC2
#define KEY2        HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_1)  //KEY2按键PC1
#define WK_UP       HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_0)  //WKUP按键PC0

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
    
    key_cb  pFkey_cb;
    
    void   *p_arg;       
    
}key_dev_t;

void key_init(int mode);

u8 KEY_Scan(u8 mode);

//**************************************
// fn : KEY_RegisterCb
//
// brief : 注册按钮事件回调
//
// param : cb -> 处理按钮事件函数指针
//
// return : none
void key_registercb(key_cb cb, void *p_arg);

//**************************************
// fn : key_poll
//
// brief : 轮询按钮事件
//
// param : none
//
// return : none
void key_poll(void);

#endif
