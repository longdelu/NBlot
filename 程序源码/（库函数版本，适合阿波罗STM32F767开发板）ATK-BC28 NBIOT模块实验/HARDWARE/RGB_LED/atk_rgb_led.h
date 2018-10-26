#ifndef __RGB_H
#define __RGB_H
#include "sys.h"
#include "delay.h"
 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//P9813 RGB LED��������	 	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2016/10/20
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////  

#define SCL(n) (n?HAL_GPIO_WritePin(GPIOG,GPIO_PIN_10,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOG,GPIO_PIN_10,GPIO_PIN_RESET))//ʱ����
#define SDA(n) (n?HAL_GPIO_WritePin(GPIOG,GPIO_PIN_12,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOG,GPIO_PIN_12,GPIO_PIN_RESET))//������


void RGBLED_Init(void);//RGB��IO��ʼ��
void RGBLED_Show(u8 r,u8 g,u8 b);//RGB����ɫ����

void atk_sda_set(int leve);  //sda�ߵ͵�ƽ����

void atk_scl_set(int leve);  //scl�ߵ͵�ƽ����
  


#endif
