#ifndef _SDRAM_H
#define _SDRAM_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////     
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK I.MX RT1052������
//SDRAM��������       
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2018/1/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved                                      
//////////////////////////////////////////////////////////////////////////////////     

#define SDRAM_START_ADDR    ((u32)(0X80000000)) //SDRAM��ʼ��ַ
#define SDRAM_SIZE_KBYTES   ((u32)(32*1024))    //SDRAM��С����λkBytes

void SDRAM_Init(void);
void SEMC_SDRAM_WriteBuffer(u8 *pBuffer,u32 WriteAddr,u32 n);
void SEMC_SDRAM_ReadBuffer(u8 *pBuffer,u32 ReadAddr,u32 n);
#endif
