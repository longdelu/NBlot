#ifndef _LPSPI_H
#define _LPSPI_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////     
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK I.MX RT1052开发板
//LPSPI驱动代码       
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2018/3/7
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved                                      
//////////////////////////////////////////////////////////////////////////////////     

extern lpspi_master_config_t lpspi3_config;

void LPSPI3_Init(u8 baudrate);
u8 LPSPI3_ReadWriteByte(u8 TxData);
void SPI_Test(void);
#endif

