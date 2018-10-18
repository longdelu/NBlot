#ifndef __ATK_BEEP_H
#define __ATK_BEEP_H	 
#include "atk_sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板V3
//蜂鸣器驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/1/10
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	  
//蜂鸣器端口定义
#define BEEP PBout(8)	// BEEP,蜂鸣器接口		   

void beep_init(void);	//蜂鸣器初始化函数

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

#endif

















