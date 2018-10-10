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

#define KEY0    GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)//读取按键0
#define KEY1    GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)//读取按键1
#define KEY2    GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2)//读取按键2 
#define WK_UP   GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)//读取按键3(WK_UP) 


/**
  * @brief  定义各个按键事件
  */
#define KEY0_PRES   1
#define KEY1_PRES   2
#define KEY2_PRES   4
#define WKUP_PRES   8

//按键消抖时延
#define KEY_DELAY_TICK   15

/**
  * @brief  定义按键回调函数指针
  */
typedef void (*key_cb)(u32 key_event, void *p_arg);


/**
  * @brief  按键结构体数
  */
typedef struct key_dev
{
    uint8_t key_event;
    
    int     start_tick;
    
    key_cb  key_cb;
    
    void   *p_arg;       
    
}key_dev_t;


/**
  * @brief  按键设备句柄.
  */
typedef key_dev_t *key_handle_t;


/**
  * @brief  按键初始化函数
  * @param  None
  * @retval 键键设备句柄
  */
key_handle_t atk_key_exit_init(void);



/**
  * @brief  注册按键事件回调.
  * @param  key_handle : 指向按键设备句柄的指针.
  * @param  cb         : 回调函数.
  * @param  p_arg      : 回调函数参数
  * @retval None
  */
void atk_key_registercb (key_handle_t key_handle, key_cb cb, void *p_arg);

/**
  * @brief  轮询按钮事件
  * @param  key_handle : 指向按键设备句柄的指针.
  * @retval None
  */
void atk_key_event_poll(key_handle_t key_handle);

#endif
