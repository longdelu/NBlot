#ifndef _ATK_BEEP_H
#define _ATK_BEEP_H

#include "atk_sys.h"
#include "atk_bsp.h"
//////////////////////////////////////////////////////////////////////////////////     
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F7开发板
//LED驱动代码       
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/6/10
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved                                      
//////////////////////////////////////////////////////////////////////////////////   



/**
  * @brief  蜂鸣器鸣叫
  * @param  None
  * @retval None
  */
void beep_on(void);


/**
  * @brief  蜂鸣器关闭
  * @param  None
  * @retval None
  */
void beep_off(void);


/**
  * @brief  蜂鸣器初始化
  * @param  None
  * @retval None
  */
void beep_init(void);



#endif
