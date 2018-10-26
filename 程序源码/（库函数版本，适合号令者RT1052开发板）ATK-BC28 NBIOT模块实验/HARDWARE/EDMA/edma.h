#ifndef _EDMA_H
#define _EDMA_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK I.MX RT1052开发板
//EDMA驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2018/5/10
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

extern edma_handle_t lpuart1_txdmahandle;             //DMA句柄

void MYEDMA_Init(void);
void EDMA_Transmit(void *srcaddr, void *destaddr, u32 length);
#endif
