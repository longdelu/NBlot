#ifndef __ATK_TIMER_H
#define __ATK_TIMER_H
#include "sys.h"
#include "gptimer.h"
//////////////////////////////////////////////////////////////////////////////////     
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK I.MX RT1052������
//��GPT��ʱ����������   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2012/12/23
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved                                      
//////////////////////////////////////////////////////////////////////////////////     

void GPT1_Int_Init(u16 psc,u32 ocrx);

void atk_timer_timing_init(u16 psc,u32 ocrx);



#endif

