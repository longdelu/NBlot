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

#define SCL(n) (n ? GPIO_PinWrite(GPIO1,9,1)  : GPIO_PinWrite(GPIO1,9,0))   //ʱ����
#define SDA(n) (n ? GPIO_PinWrite(GPIO1,19,1) : GPIO_PinWrite(GPIO1,19,0))  //������


void RGBLED_Init(void);//RGB��IO��ʼ��
void RGBLED_Show(u8 r,u8 g,u8 b);//RGB����ɫ����

void atk_sda_set(int leve);  //sda�ߵ͵�ƽ����

void atk_scl_set(int leve);  //scl�ߵ͵�ƽ����
  


#endif
