#ifndef _SDRAM_H
#define _SDRAM_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////     
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK I.MX RT1052开发板
//SDRAM驱动代码       
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2018/1/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved                                      
//////////////////////////////////////////////////////////////////////////////////     

#define SDRAM_START_ADDR    ((u32)(0X80000000)) //SDRAM开始地址
#define SDRAM_SIZE_KBYTES   ((u32)(32*1024))    //SDRAM大小，单位kBytes

void SDRAM_Init(void);
void SEMC_SDRAM_WriteBuffer(u8 *pBuffer,u32 WriteAddr,u32 n);
void SEMC_SDRAM_ReadBuffer(u8 *pBuffer,u32 ReadAddr,u32 n);
#endif
