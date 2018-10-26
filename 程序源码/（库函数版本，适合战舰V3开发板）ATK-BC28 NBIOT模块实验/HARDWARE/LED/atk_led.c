#include "atk_led.h"
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
void atk_led_init(void)
{
  
    GPIO_InitTypeDef  GPIO_InitStructure;
  
    //ʹ��PB,PE�˿�ʱ��   
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOE, ENABLE);    
      
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;                 //LED0-->PB.5 �˿�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;          //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;         //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);                    //�����趨������ʼ��GPIOB.5
    GPIO_SetBits(GPIOB,GPIO_Pin_5);                           //PB.5 �����

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;                  //LED1-->PE.5 �˿�����, �������
    GPIO_Init(GPIOE, &GPIO_InitStructure);                     //������� ��IO���ٶ�Ϊ50MHz
    GPIO_SetBits(GPIOE,GPIO_Pin_5);                            //PE.5 �����   

}


/**
  * @brief  led��
  * @param  led_id  
  * @retval None
  */
void atk_led_on(int led_id)
{
  
    if (0 == led_id)
    {
       LED0 = 0;                
    }
    
    else 
    {
      
       LED1 = 0;  
    }

}


/**
  * @brief  led��
  * @param  led_id
  * @retval None
  */
void atk_led_off(int led_id)
{
    if (0 == led_id)
    {
       LED0 = 1;        
        
    }
    
    else 
    {
       LED1 = 1;        
      
    }
}

/**
  * @brief  led��ת
  * @param  led_id
  * @retval None
  */
void atk_led_toggle(int led_id)
{
    if (0 == led_id)
    {
      
       LED0_Toggle; 
    }
    
    else 
    {
      
       LED1_Toggle;
    }
}

