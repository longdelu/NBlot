#include "pitimer.h"
#include "led.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK I.MX RT1052������
//PIT��ʱ����������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2018/4/25
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

pit_config_t pit_config;

//��ʼ��PITͨ��0,PITʱ��ԴΪperclk_clk_root=75MHz.
//ldval: CH0��ʼֵ,0~0xFFFFFFFF
//�ݼ�����,��ldval==0ʱ,�����ж�.
//��ʱʱ��=ldval/PERCLK_CLK_ROOT
void PIT_CH0_Int_Init(u32 ldval)
{
    PIT_GetDefaultConfig(&pit_config);  //��ʼ��ΪĬ������
    PIT_Init(PIT,&pit_config);          //��ʼ��PIT��ʱ��
    
    PIT_SetTimerPeriod(PIT,kPIT_Chnl_0,ldval);//���õ���ʱ��ʼֵ
    PIT_EnableInterrupts(PIT,kPIT_Chnl_0,kPIT_TimerInterruptEnable);//ʹ���ж�
    RT1052_NVIC_SetPriority(PIT_IRQn,6,0);	//��ռ���ȼ�6�������ȼ�0
	EnableIRQ(PIT_IRQn);	                //ʹ��PIT�ж�
    PIT_StartTimer(PIT,kPIT_Chnl_0);        //��PIT
}

//PIT�жϷ�����
void PIT_IRQHandler(void)
{
    //PIT CH0�ж�
    if((PIT_GetStatusFlags(PIT,kPIT_Chnl_0)&kPIT_TimerFlag)==kPIT_TimerFlag)
    {
        LED1_Toggle;			//LED�Ʒ�ת
        PIT_ClearStatusFlags(PIT,kPIT_Chnl_0,kPIT_TimerFlag);//����жϱ�־λ
    }
    __DSB();				//����ͬ������ָ��
}
