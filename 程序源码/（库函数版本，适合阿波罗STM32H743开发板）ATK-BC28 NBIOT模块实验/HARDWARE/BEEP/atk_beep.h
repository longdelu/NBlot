#ifndef _ATK_BEEP_H
#define _ATK_BEEP_H

#include "atk_sys.h"
#include "atk_bsp.h"
//////////////////////////////////////////////////////////////////////////////////     
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F7������
//LED��������       
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/6/10
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved                                      
//////////////////////////////////////////////////////////////////////////////////   



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


/**
  * @brief  ��������ʼ��
  * @param  None
  * @retval None
  */
void beep_init(void);



#endif
