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


#define KEY0     GPIO_PinRead(GPIO1,5)  //KEY0按键GPIO1_05
#define KEY1     GPIO_PinRead(GPIO5,1)  //KEY1按键GPIO5_01
#define KEY2     GPIO_PinRead(GPIO3,26) //KEY2按键GPIO3_26
#define WK_UP    GPIO_PinRead(GPIO5,0)  //WK_UP按键GPIO5_00

/**
  * @brief  定义各个按键事件
  */
#define KEY0_PRES   1
#define KEY1_PRES   2
#define KEY2_PRES   4
#define WKUP_PRES   8

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
