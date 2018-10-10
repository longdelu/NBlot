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

#define SCL PGout(7)//时钟线
#define SDA PGout(8)//数据线


void RGBLED_Init(void);//RGB灯IO初始化
void RGBLED_Show(u8 r,u8 g,u8 b);//RGB灯颜色控制

#endif
