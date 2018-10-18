#include "atk_beep.h"
#include "pcf8574.h"
//////////////////////////////////////////////////////////////////////////////////     
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F7������
//LED��������       
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/11/27
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved                                      
//////////////////////////////////////////////////////////////////////////////////     
          

/**
  * @brief  led��ʼ��
  * @param  None
  * @retval None
  */
void beep_init(void)
{
     PCF8574_Init();           //PCF8574��ʼ��  

}


/**
  * @brief  ����������
  * @param  None
  * @retval None
  */
void beep_on(void)
{
  
    PCF8574_WriteBit(BEEP_IO,0);//�򿪷�����  
  
}


/**
  * @brief  �������ر�
  * @param  None
  * @retval None
  */
void beep_off(void)
{  
    PCF8574_WriteBit(BEEP_IO,1);//�رշ�����  
}
