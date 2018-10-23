#ifndef _ATK_KEY_H
#define _ATK_KEY_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////     
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//KEY驱动代码       
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2017/4/6
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved                                      
//////////////////////////////////////////////////////////////////////////////////



//下面的方式是通过直接操作HAL库函数方式读取IO
#define KEY0        HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_4)  //KEY0按键PE4
#define KEY1        HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_3)  //KEY1按键PE3
#define KEY2        HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_2)  //KEY2按键PE2
#define WK_UP       HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)  //WKUP按键PA0

#define KEY0_PRES     1
#define KEY1_PRES     2
#define KEY2_PRES     4
#define WKUP_PRES     8

//按键消抖时延
#define KEY_DELAY_TICK   35

/**
  * @brief  定义按键回调函数指针
  */
typedef void (*key_cb)(u32 key_event, void *p_arg);


/**
  * @brief  按键结构体
  */
typedef struct key_dev
{
  
    uint8_t key_event;    //按键事件
    
    int     start_tick;   //消抖起始时刻
    
    key_cb  key_cb;       //按键回回函数
    
    void   *p_arg;        //回回函数参数       
    
}key_dev_t;


/**
  * @brief  按键设备句柄
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
