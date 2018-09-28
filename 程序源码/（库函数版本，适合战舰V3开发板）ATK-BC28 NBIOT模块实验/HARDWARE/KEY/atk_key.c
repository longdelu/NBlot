#include "atk_key.h"
#include "atk_delay.h"
#include "atk_soft_timer.h"

//////////////////////////////////////////////////////////////////////////////////     
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F7开发板
//KEY驱动代码       
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/11/27
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved                                      
//////////////////////////////////////////////////////////////////////////////////

static key_dev_t key_dev = {0,0, NULL, NULL};

//按键初始化函数

/**
  * @brief  按键初始化函数
  * @param  None
  * @retval 按键设备句柄的指针
  */
key_handle_t atk_key_exit_init(void)
{    
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    GPIO_InitTypeDef GPIO_InitStructure;
  
    //使能PORTA,PORTE时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOE,ENABLE);
  
    //使能复用功能时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);                      

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;//KEY0-KEY2
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;                   //设置成上拉输入
    GPIO_Init(GPIOE, &GPIO_InitStructure);                          //初始化GPIOE2,3,4

    //初始化 WK_UP-->GPIOA.0      下拉输入
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0设置成输入，默认下拉      
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.0
    

    //GPIOE.2 中断线以及中断初始化配置   下降沿触发
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource2);

    EXTI_InitStructure.EXTI_Line=EXTI_Line2;  //KEY2
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;  
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);     //根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

   //GPIOE.3    中断线以及中断初始化配置 下降沿触发 //KEY1
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource3);
    EXTI_InitStructure.EXTI_Line=EXTI_Line3;
    EXTI_Init(&EXTI_InitStructure);      //根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

   //GPIOE.4    中断线以及中断初始化配置  下降沿触发  //KEY0
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource4);
    EXTI_InitStructure.EXTI_Line=EXTI_Line4;
    EXTI_Init(&EXTI_InitStructure);      //根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器


   //GPIOA.0    中断线以及中断初始化配置 上升沿触发 PA0  WK_UP
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0); 

    EXTI_InitStructure.EXTI_Line=EXTI_Line0;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_Init(&EXTI_InitStructure);    //根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器


    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;      //使能按键WK_UP所在的外部中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;  //抢占优先级2， 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;          //子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                //使能外部中断通道
    NVIC_Init(&NVIC_InitStructure); 

    NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;      //使能按键KEY2所在的外部中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;  //抢占优先级2， 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;          //子优先级2
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                //使能外部中断通道
    NVIC_Init(&NVIC_InitStructure);


    NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;      //使能按键KEY1所在的外部中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;  //抢占优先级2 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;          //子优先级1 
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                //使能外部中断通道
    NVIC_Init(&NVIC_InitStructure);      //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;      //使能按键KEY0所在的外部中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;  //抢占优先级2 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;          //子优先级0 
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                //使能外部中断通道
    NVIC_Init(&NVIC_InitStructure);      //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
        
    return &key_dev;
}


/**
  * @brief  中断服务函数.
  */
void EXTI0_IRQHandler(void)
{
  
    if(WK_UP==1) 
    {
        key_dev.key_event  = WKUP_PRES;
        key_dev.start_tick = atk_soft_timer_getick();
    }
   
    EXTI_ClearITPendingBit(EXTI_Line0); //清除LINE0上的中断标志位  
}
 
/**
  * @brief  中断服务函数.
  */
void EXTI2_IRQHandler(void)
{
  
  if (KEY2==0)    //按键KEY2
  {
      key_dev.key_event  = KEY2_PRES;
      key_dev.start_tick = atk_soft_timer_getick();    
  }
  
  EXTI_ClearITPendingBit(EXTI_Line2);  //清除LINE2上的中断标志位  
}
/**
  * @brief  中断服务函数.
  */
void EXTI3_IRQHandler(void)
{

  if (KEY1==0)   //按键KEY1
  {         
     key_dev.key_event  = KEY1_PRES;
     key_dev.start_tick = atk_soft_timer_getick();
  }     
  EXTI_ClearITPendingBit(EXTI_Line3);  //清除LINE3上的中断标志位  
}

/**
  * @brief  中断服务函数.
  */
void EXTI4_IRQHandler(void)
{

    if (KEY0==0)   //按键KEY0
    {
       key_dev.key_event  = KEY0_PRES;
       key_dev.start_tick = atk_soft_timer_getick();
    }
    
    EXTI_ClearITPendingBit(EXTI_Line4);  //清除LINE4上的中断标志位  
}



/**
  * @brief  注册按键事件回调.
  * @param  key_handle : 指向按键设备句柄的指针.
  * @param  cb         : 回调函数.
  * @param  p_arg      : 回调函数参数
  * @retval None
  */
void atk_key_registercb (key_handle_t key_handle, key_cb cb, void *p_arg)
{
    if ((cb != NULL) && (key_handle != NULL))
    {
        key_handle->key_cb = cb;
        key_handle->p_arg  = p_arg;
    }
}



/**
  * @brief  轮询按钮事件
  * @param  key_handle : 指向按键设备句柄的指针.
  * @retval None
  */
void atk_key_event_poll(key_handle_t key_handle)
{
    uint8_t key_event = 0;

    //按键消抖    
    if(key_dev.key_event)
    { 
        if (atk_soft_timer_getick() - key_dev.start_tick >= KEY_DELAY_TICK )
        {
            if (key_dev.key_event & WKUP_PRES)
            {
                if (WK_UP == 1)
                {
                    key_event |= WKUP_PRES;
                }

                key_dev.key_event ^= WKUP_PRES;
            }
          
            if(key_dev.key_event & KEY2_PRES)
            {
              
                if (KEY2 == 0)
                {
                     key_event |= KEY2_PRES;
                }
                key_dev.key_event ^= KEY2_PRES;
            }
          
           if (key_dev.key_event & KEY1_PRES)
           {
                if (KEY1 == 0)
                {
                     key_event |= KEY1_PRES;
                }
                key_dev.key_event ^= KEY1_PRES;
           }
          
           if (key_dev.key_event & KEY0_PRES)
           {
                if (KEY0 == 0)
                {
                    key_event |= KEY0_PRES;
                }
                key_dev.key_event ^= KEY0_PRES;
           }
        }
    }

    //如果真的有按钮按下，则执行回调函数
    if (key_event && key_handle->key_cb)
    {
        key_handle->key_cb(key_event, key_handle->p_arg);
    }
}
