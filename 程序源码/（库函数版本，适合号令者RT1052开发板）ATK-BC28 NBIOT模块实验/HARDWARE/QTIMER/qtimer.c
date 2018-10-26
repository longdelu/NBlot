#include "qtimer.h"
#include "led.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK I.MX RT1052������
//QTMR��ʱ����������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2018/1/2
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//QTIMER1���ýṹ��
qtmr_config_t qtimer1_config;

//��ʼ��QTIMER,TMR��ʱ����ʱ��ԴΪIPG_CLK_ROOT=150MHz
//prisrc : ��һʱ��Դѡ��
//         0000~0011,ͨ��0~3����������.
//         0100~0111,ͨ��0~3�����.�����ڼ���.
//         1000~1111,IPG_CLK_ROOTʱ�ӵ�:1,2,4,8,16,32,64,128��Ƶ.
//cmp1	 : COMP1�Ĵ���ֵ,0~65535.
//��ʱʱ��=cmp1/QTMR1_CLK
//����prisrc=1111,��QTMR1_CLK=1171875Hz.
//��cmp1=46875,��ʱʱ��= 46875/1171875=0.04��.
//��ʹ��IPG_CLK_ROOT��Ϊʱ��Դʱ,�����ʱʱ��=65535*128/150M=55.9ms
void QTMR1_CH0_Int_Init(u8 prisrc,u16 cmp1)
{
    qtmr_primary_count_source_t qtimer1_source;
    
    u32 ipgclk=CLOCK_GetFreq(kCLOCK_IpgClk);            //��ȡIPG_CLK=150Mhz
    qtimer1_source=(qtmr_primary_count_source_t)prisrc;
    
    QTMR_GetDefaultConfig(&qtimer1_config);             //������ΪĬ�����ã������ڸ���ʵ���������
    
    qtimer1_config.primarySource=qtimer1_source;        //���õ�һʱ��Դ
    QTMR_Init(TMR1,kQTMR_Channel_0,&qtimer1_config);    //��ʼ��QTIMER
    QTMR_SetTimerPeriod(TMR1,kQTMR_Channel_0,cmp1);     //����COMP1ƥ��ֵ(���ϼ�����COMP1,������COMP2)

    //�����ж�
    QTMR_EnableInterrupts(TMR1,kQTMR_Channel_0,kQTMR_CompareInterruptEnable);  //ʹ��ͨ��0�ıȽ��ж�
    RT1052_NVIC_SetPriority(TMR1_IRQn,6,0);             //��ռ���ȼ�6�������ȼ�0
    EnableIRQ(TMR1_IRQn);                               //����TMR3�ж�
        
    //��ʼͨ��0
    QTMR_StartTimer(TMR1, kQTMR_Channel_0,kQTMR_PriSrcRiseEdge);    //ͨ��0��primaryʱ��Դ�������ؼ���
} 

//TMR1�жϷ�����
void TMR1_IRQHandler(void)
{
    static u8 count=0;	    //������
    
    //�ж��ǲ���TMR1ͨ��0�ıȽ��ж�
    if((QTMR_GetStatus(TMR1,kQTMR_Channel_0)&kQTMR_CompareFlag)==kQTMR_CompareFlag)
    {
        count++;
		if(count==25)       //ÿ����25���жϷ�����,��ִ��һ��.
		{ 
			LED1_Toggle;    //DS1��ת
			count=0;	    //���������
		}
        QTMR_ClearStatusFlags(TMR1,kQTMR_Channel_0,kQTMR_CompareFlag); //����жϱ�־λ 
    }
    __DSB();                //����ͬ������ָ��
}
