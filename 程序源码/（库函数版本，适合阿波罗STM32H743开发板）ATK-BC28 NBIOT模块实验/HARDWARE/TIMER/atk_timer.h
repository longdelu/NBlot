#ifndef _ATK_TIMER_H
#define _ATK_TIMER_H

#include "atk_sys.h"

//////////////////////////////////////////////////////////////////////////////////     
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F7������
//��ʱ����������       
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/11/27
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved                                      
////////////////////////////////////////////////////////////////////////////////// 


extern TIM_HandleTypeDef TIM3_Handler; //��ʱ����� 

void atk_tim3_init(u16 arr,u16 psc);

#endif

