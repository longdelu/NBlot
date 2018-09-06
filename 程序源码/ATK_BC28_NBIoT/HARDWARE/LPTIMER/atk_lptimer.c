#include "lptimer.h"
#include "atk_led.h"
//////////////////////////////////////////////////////////////////////////////////     
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F7开发板
//定时器中断驱动代码       
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/11/27
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved                                      
//////////////////////////////////////////////////////////////////////////////////     

//低功耗定时器句柄      
LPTIM_HandleTypeDef hlptim1;
LPTIM_HandleTypeDef hlptim2;


//低功耗定时器1中断初始化
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是低功耗定时器1(定时器1挂在APB1上，时钟为HCLK/2)
void LPTIM1_Init(u32 psc, u32 arr)
{
    hlptim1.Instance = LPTIM1;
    
    hlptim1.Init.Clock.Source = LPTIM_CLOCKSOURCE_APBCLOCK_LPOSC;
    hlptim1.Init.Clock.Prescaler = LPTIM_PRESCALER_DIV1;               
    
    hlptim1.Init.Trigger.Source = LPTIM_TRIGSOURCE_SOFTWARE;
    hlptim1.Init.OutputPolarity = LPTIM_OUTPUTPOLARITY_HIGH;
    hlptim1.Init.UpdateMode = LPTIM_UPDATE_IMMEDIATE;
    hlptim1.Init.CounterSource = LPTIM_COUNTERSOURCE_INTERNAL;
    hlptim1.Init.Input1Source = LPTIM_INPUT1SOURCE_GPIO;
    hlptim1.Init.Input2Source = LPTIM_INPUT2SOURCE_GPIO;
    
       
    if (HAL_LPTIM_Init(&hlptim1) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }
    
    
    /* ### - 3 - Start the Timeout function in interrupt mode ################# */
    /*
    *  arr = 65535
    *  psc  = 32767
    *  According to this configuration (LPTIMER clocked by LSE & compare = 32767,
    *  the Timeout period = (compare + 1)/LSE_Frequency = 1s
    */
    if (HAL_LPTIM_TimeOut_Start_IT(&hlptim1, arr, psc) != HAL_OK)
    {
        Error_Handler();
    }   
}


//低功耗定时器2中断初始化
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是低功耗定时器1(定时器1挂在APB1上，时钟为HCLK/2)
void LPTIM2_Init(u32 psc, u32 arr)
{
          
  hlptim2.Instance = LPTIM2;
  hlptim2.Init.Clock.Source = LPTIM_CLOCKSOURCE_APBCLOCK_LPOSC;
  hlptim1.Init.Clock.Prescaler=psc;                   //分频系数
//  hlptim1.Init.CounterMode=TIM_COUNTERMODE_UP;      //向上计数器
//  hlptim1.Init.Period=arr;                          //自动装载值
//  hlptim1.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//时钟分频因子   
  hlptim2.Init.Trigger.Source = LPTIM_TRIGSOURCE_SOFTWARE;
  hlptim2.Init.OutputPolarity = LPTIM_OUTPUTPOLARITY_HIGH;
  hlptim2.Init.UpdateMode = LPTIM_UPDATE_IMMEDIATE;
  hlptim2.Init.CounterSource = LPTIM_COUNTERSOURCE_INTERNAL;
  hlptim2.Init.Input1Source = LPTIM_INPUT1SOURCE_GPIO;
  hlptim2.Init.Input2Source = LPTIM_INPUT2SOURCE_GPIO;
  if (HAL_LPTIM_Init(&hlptim2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

//低功耗定时器底册驱动，开启时钟，设置中断优先级
//此函数会被HAL_TIM_Base_Init()函数调用
void HAL_LPTIM_MspInit(LPTIM_HandleTypeDef *htim)
{
    /* Clocks structure declaration */
    RCC_PeriphCLKInitTypeDef        RCC_PeriphCLKInitStruct;
    
    if(htim->Instance==LPTIM1)
    {
        __HAL_RCC_LPTIM1_CLK_ENABLE();             //使能LPTIM1时钟
        
        /* ### - 1 - Re-target the LSE to Clock the LPTIM Counter ################# */
        /* Select the LSE clock as LPTIM peripheral clock */
        RCC_PeriphCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LPTIM1;
        RCC_PeriphCLKInitStruct.Lptim2ClockSelection = RCC_LPTIM2CLKSOURCE_PCLK1;
        HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphCLKInitStruct);       
        
        HAL_NVIC_SetPriority(LPTIM1_IRQn,14,0);    //设置中断优先级，抢占优先级1，子优先级3
        HAL_NVIC_EnableIRQ(LPTIM1_IRQn);           //开启ITM3中断   
    }  
}

//低功耗定时器1中断服务函数
void LPTIM1_IRQHandler(void)
{
    HAL_LPTIM_IRQHandler(&hlptim1);
}


//低功耗定时器2中断服务函数
void LPTIM2_IRQHandler(void)
{
    HAL_LPTIM_IRQHandler(&hlptim2);
}


/**
  * @brief  Compare match callback in non blocking mode 
  * @param  hlptim : LPTIM handle
  * @retval None
  */
void HAL_LPTIM_CompareMatchCallback(LPTIM_HandleTypeDef *hlptim)
{
    if(hlptim==(&hlptim1)) {
        
        
        printf("lptim1 timeout reach\r\n");
       
    } else if(hlptim==(&hlptim2)) {
        
        printf("lptim2 timeout reach\r\n");
        
    } else {
        
    }
}

