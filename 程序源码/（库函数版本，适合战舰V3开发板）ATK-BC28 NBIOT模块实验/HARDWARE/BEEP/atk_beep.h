#ifndef __ATK_BEEP_H
#define __ATK_BEEP_H	 
#include "atk_sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������V3
//��������������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/1/10
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	  
//�������˿ڶ���
#define BEEP PBout(8)	// BEEP,�������ӿ�		   

void beep_init(void);	//��������ʼ������

/**
  * @brief  ����������
  * @param  None
  * @retval None
  */
void beep_on(void);


/**
  * @brief  �������ر�
  * @param  None
  * @retval None
  */
void beep_off(void);

#endif

















