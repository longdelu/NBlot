#ifndef _EDMA_H
#define _EDMA_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK I.MX RT1052������
//EDMA��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2018/5/10
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

extern edma_handle_t lpuart1_txdmahandle;             //DMA���

void MYEDMA_Init(void);
void EDMA_Transmit(void *srcaddr, void *destaddr, u32 length);
#endif
