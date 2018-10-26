#include "pitimer.h"
#include "led.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK I.MX RT1052开发板
//PIT定时器驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2018/4/25
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

pit_config_t pit_config;

//初始化PIT通道0,PIT时钟源为perclk_clk_root=75MHz.
//ldval: CH0初始值,0~0xFFFFFFFF
//递减计数,当ldval==0时,产生中断.
//定时时间=ldval/PERCLK_CLK_ROOT
void PIT_CH0_Int_Init(u32 ldval)
{
    PIT_GetDefaultConfig(&pit_config);  //初始化为默认配置
    PIT_Init(PIT,&pit_config);          //初始化PIT定时器
    
    PIT_SetTimerPeriod(PIT,kPIT_Chnl_0,ldval);//设置倒计时初始值
    PIT_EnableInterrupts(PIT,kPIT_Chnl_0,kPIT_TimerInterruptEnable);//使能中断
    RT1052_NVIC_SetPriority(PIT_IRQn,6,0);	//抢占优先级6，子优先级0
	EnableIRQ(PIT_IRQn);	                //使能PIT中断
    PIT_StartTimer(PIT,kPIT_Chnl_0);        //打开PIT
}

//PIT中断服务函数
void PIT_IRQHandler(void)
{
    //PIT CH0中断
    if((PIT_GetStatusFlags(PIT,kPIT_Chnl_0)&kPIT_TimerFlag)==kPIT_TimerFlag)
    {
        LED1_Toggle;			//LED灯翻转
        PIT_ClearStatusFlags(PIT,kPIT_Chnl_0,kPIT_TimerFlag);//清楚中断标志位
    }
    __DSB();				//数据同步屏蔽指令
}
