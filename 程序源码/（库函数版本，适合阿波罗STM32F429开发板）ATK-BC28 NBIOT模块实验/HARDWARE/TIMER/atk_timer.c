#include "atk_timer.h"
#include "atk_led.h"
//////////////////////////////////////////////////////////////////////////////////     
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F7������
//��ʱ���ж���������       
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/11/27
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved                                      
//////////////////////////////////////////////////////////////////////////////////     

TIM_HandleTypeDef TIM3_Handler; //��ʱ����� 

//ͨ�ö�ʱ��7�жϳ�ʼ��
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz
//����ʹ�õ��Ƕ�ʱ��3!(��ʱ��7����APB1�ϣ�ʱ��ΪHCLK/2)
void atk_tim3_init(u16 arr,u16 psc)
{  
    __HAL_RCC_TIM3_CLK_ENABLE();                           //ʹ��TIM3ʱ��
    TIM3_Handler.Instance=TIM3;                            //������ʱ��7
    TIM3_Handler.Init.Prescaler=psc;                       //��Ƶϵ��
    TIM3_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;      //���ϼ�����
    TIM3_Handler.Init.Period=arr;                          //�Զ�װ��ֵ
    TIM3_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//ʱ�ӷ�Ƶ����
    
    HAL_TIM_Base_Init(&TIM3_Handler);
    
    HAL_TIM_Base_Start_IT(&TIM3_Handler);

    HAL_NVIC_SetPriority(TIM3_IRQn,13,0);                  //�����ж����ȼ�����ռ���ȼ�13�������ȼ�0
    HAL_NVIC_EnableIRQ(TIM3_IRQn);                         //����ITM3�ж�      
}


//��ʱ��3�жϷ�����
void TIM3_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM3_Handler);
}

//�ص���������ʱ���жϷ���������
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{ 
    
    if (htim == (&TIM3_Handler)) //��ʱ��3�жϷ���������(NBIOT����)
    {        
        LED0_Toggle;
    }
}


