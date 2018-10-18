#ifndef __AP3216C_H
#define __AP3216C_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F429������
//AP3216C��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/1/16
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

#define AP3216C_ADDR    0X3C	//AP3216C����IIC��ַ(������һλ)

u8 AP3216C_Init(void); 
u8 AP3216C_WriteOneByte(u8 reg,u8 data);
u8 AP3216C_ReadOneByte(u8 reg);
void AP3216C_ReadData(u16* ir,u16* ps,u16* als);
#endif
