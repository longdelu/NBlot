#include "gptimer.h"
#include "led.h"
//////////////////////////////////////////////////////////////////////////////////     
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK I.MX RT1052������
//GPT��ʱ����������       
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2012/12/23
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved                                      
//////////////////////////////////////////////////////////////////////////////////     

gpt_config_t gpt1_onfig;

//��ʼ��GPTIMER1��ʱ��ԴΪperclk_clk_root=75MHz
//pre:��Ƶֵ��0~4096
//comcount:�Ƚϼ���ֵ��0~0xFFFFFFFF
//��ocrx==CNTʱ,�����ж�.
//��ʱʱ��=ocrx*(psc+1)/PERCLK_CLK_ROOT
void GPT1_Int_Init(u16 psc,u32 ocrx)
{
    GPT_GetDefaultConfig(&gpt1_onfig);    //�ȳ�ʼ��GPT1ΪĬ��ֵ
    gpt1_onfig.clockSource=kGPT_ClockSource_Periph;    //��ʼ��ʱ��Դperclk_clk_root
    gpt1_onfig.divider=psc;            //���÷�Ƶֵ
    GPT_Init(GPT1,&gpt1_onfig);
    
    GPT_SetOutputCompareValue(GPT1,kGPT_OutputCompare_Channel1,ocrx);        //���ñȽϼ���ֵ
    GPT_EnableInterrupts(GPT1,kGPT_OutputCompare1InterruptEnable);            //ʹ��GPT�Ƚ�ͨ��1�ж�
    RT1052_NVIC_SetPriority(GPT1_IRQn,5,0);                                    //��ռ���ȼ�6�������ȼ�0
    EnableIRQ(GPT1_IRQn);    //ʹ��GPT1�ж�
    GPT_StartTimer(GPT1);    //��ʼ��ʱ��    
}


void atk_timer_timing_init(u16 psc,u32 ocrx)
{
   
   GPT1_Int_Init(psc, ocrx);
}



//GPT1�жϷ�����
void GPT1_IRQHandler(void)
{
    //OCR1�ж�
    if(GPT_GetStatusFlags(GPT1,kGPT_OutputCompare1Flag))
    {
        LED0_Toggle;            //LED0�Ʒ�ת
        GPT_ClearStatusFlags(GPT1,kGPT_OutputCompare1Flag);//����жϱ�־λ
    }
    __DSB();                //����ͬ������ָ��
}


