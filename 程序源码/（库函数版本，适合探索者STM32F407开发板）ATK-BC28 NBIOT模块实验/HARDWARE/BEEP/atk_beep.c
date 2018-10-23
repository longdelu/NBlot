#include "atk_beep.h"
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

    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOF_CLK_ENABLE();           //����GPIOFʱ��
    
    GPIO_Initure.Pin=GPIO_PIN_8;            //PF8
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
    GPIO_Initure.Pull=GPIO_PULLUP;          //����
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
    HAL_GPIO_Init(GPIOF,&GPIO_Initure);
    
    HAL_GPIO_WritePin(GPIOF,GPIO_PIN_8, GPIO_PIN_RESET);    //��������Ӧ����GPIOF8����

}


/**
  * @brief  ����������
  * @param  None
  * @retval None
  */
void beep_on(void)
{
  
    HAL_GPIO_WritePin(GPIOF,GPIO_PIN_8, GPIO_PIN_SET);      //�򿪷�����  
  
}


/**
  * @brief  �������ر�
  * @param  None
  * @retval None
  */
void beep_off(void)
{  
    HAL_GPIO_WritePin(GPIOF,GPIO_PIN_8, GPIO_PIN_RESET);      //�رշ�����  
}
