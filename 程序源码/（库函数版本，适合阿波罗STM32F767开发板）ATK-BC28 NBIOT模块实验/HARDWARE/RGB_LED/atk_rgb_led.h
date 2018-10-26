#ifndef __RGB_H
#define __RGB_H
#include "sys.h"
#include "delay.h"
 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//P9813 RGB LED驱动代码	 	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2016/10/20
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////  

#define SCL(n) (n?HAL_GPIO_WritePin(GPIOG,GPIO_PIN_10,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOG,GPIO_PIN_10,GPIO_PIN_RESET))//时钟线
#define SDA(n) (n?HAL_GPIO_WritePin(GPIOG,GPIO_PIN_12,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOG,GPIO_PIN_12,GPIO_PIN_RESET))//数据线


void RGBLED_Init(void);//RGB灯IO初始化
void RGBLED_Show(u8 r,u8 g,u8 b);//RGB灯颜色控制

void atk_sda_set(int leve);  //sda高低电平设置

void atk_scl_set(int leve);  //scl高低电平设置
  


#endif
