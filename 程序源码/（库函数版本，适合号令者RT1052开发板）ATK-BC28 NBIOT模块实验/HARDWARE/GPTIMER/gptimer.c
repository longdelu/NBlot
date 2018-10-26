#include "gptimer.h"
#include "led.h"
//////////////////////////////////////////////////////////////////////////////////     
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK I.MX RT1052开发板
//GPT定时器驱动代码       
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2012/12/23
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved                                      
//////////////////////////////////////////////////////////////////////////////////     

gpt_config_t gpt1_onfig;

//初始化GPTIMER1，时钟源为perclk_clk_root=75MHz
//pre:分频值，0~4096
//comcount:比较计数值，0~0xFFFFFFFF
//当ocrx==CNT时,产生中断.
//定时时间=ocrx*(psc+1)/PERCLK_CLK_ROOT
void GPT1_Int_Init(u16 psc,u32 ocrx)
{
    GPT_GetDefaultConfig(&gpt1_onfig);    //先初始化GPT1为默认值
    gpt1_onfig.clockSource=kGPT_ClockSource_Periph;    //初始化时钟源perclk_clk_root
    gpt1_onfig.divider=psc;            //设置分频值
    GPT_Init(GPT1,&gpt1_onfig);
    
    GPT_SetOutputCompareValue(GPT1,kGPT_OutputCompare_Channel1,ocrx);        //设置比较计数值
    GPT_EnableInterrupts(GPT1,kGPT_OutputCompare1InterruptEnable);            //使能GPT比较通道1中断
    RT1052_NVIC_SetPriority(GPT1_IRQn,5,0);                                    //抢占优先级6，子优先级0
    EnableIRQ(GPT1_IRQn);    //使能GPT1中断
    GPT_StartTimer(GPT1);    //开始定时器    
}


void atk_timer_timing_init(u16 psc,u32 ocrx)
{
   
   GPT1_Int_Init(psc, ocrx);
}



//GPT1中断服务函数
void GPT1_IRQHandler(void)
{
    //OCR1中断
    if(GPT_GetStatusFlags(GPT1,kGPT_OutputCompare1Flag))
    {
        LED0_Toggle;            //LED0灯翻转
        GPT_ClearStatusFlags(GPT1,kGPT_OutputCompare1Flag);//清除中断标志位
    }
    __DSB();                //数据同步屏蔽指令
}


