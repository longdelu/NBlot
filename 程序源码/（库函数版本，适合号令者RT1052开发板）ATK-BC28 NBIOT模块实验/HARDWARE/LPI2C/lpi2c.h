#ifndef _I2C_H
#define _I2C_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////     
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK I.MX RT1052开发板
//I2C驱动代码       
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2018/1/9
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved                                      
//////////////////////////////////////////////////////////////////////////////////     

extern lpi2c_master_config_t    lpi2c1_config;      //I2C1(主机)配置结构体
extern lpi2c_master_handle_t    lpi2c1_handle;      //I2C1(主机)句柄
extern volatile bool            lpi2c1_compflag;    //完成标志
extern volatile bool            lpi2c1_nakflag;     //nak标志

void LPI2C1_Init(u32 baudrate);
#endif
