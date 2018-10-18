#include "atk_beep.h"
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

//��ʼ��PB8Ϊ�����.��ʹ������ڵ�ʱ��            
//��������ʼ��
void beep_init(void)
{
   GPIO_InitTypeDef  GPIO_InitStructure;
    
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//ʹ��GPIOB�˿�ʱ��
   
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;            //BEEP-->PB.8 �˿�����
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     //�������
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    //�ٶ�Ϊ50MHz
   GPIO_Init(GPIOB, &GPIO_InitStructure);               //���ݲ�����ʼ��GPIOB.8
   
   GPIO_ResetBits(GPIOB,GPIO_Pin_8);                    //���0���رշ��������  
}

/**
  * @brief  ����������
  * @param  None
  * @retval None
  */
void beep_on(void)
{  
    GPIO_SetBits(GPIOB,GPIO_Pin_8);                    //���1���򿪷��������    
}


/**
  * @brief  �������ر�
  * @param  None
  * @retval None
  */
void beep_off(void)
{  
    GPIO_ResetBits(GPIOB,GPIO_Pin_8);                    //���0���رշ��������  
}







