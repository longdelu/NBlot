#include "atk_key.h"
#include "atk_delay.h"
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
//mode: 0 查询模式； 1：中断模式
key_handle_t key_init(int mode)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOC_CLK_ENABLE();                                  //开启GPIOC时钟 
    
    GPIO_Initure.Pin =GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3; //PC0,1,2,3
    
    if (mode == 1) {
        GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;    //输入, 下降沿触发 
    } else {
        GPIO_Initure.Mode=GPIO_MODE_INPUT;        //输入, 下降沿触发
    }
    
    GPIO_Initure.Pull=GPIO_PULLUP;             //上拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;        //高速
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);
    
    if (mode == 1) {
        
        //中断线0-PC0
        HAL_NVIC_SetPriority(EXTI0_IRQn,12,0);       //抢占优先级为12，子优先级为0
        HAL_NVIC_EnableIRQ(EXTI0_IRQn);             //使能中断线0
        
        //中断线1-PC1
        HAL_NVIC_SetPriority(EXTI1_IRQn,12,0);       //抢占优先级为12，子优先级为0
        HAL_NVIC_EnableIRQ(EXTI1_IRQn);           
        
        //中断线2-PC2
        HAL_NVIC_SetPriority(EXTI2_IRQn,12,0);       //抢占优先级为12，子优先级为0
        HAL_NVIC_EnableIRQ(EXTI2_IRQn);              //使能中断线2
           
        //中断线3-PC3
        HAL_NVIC_SetPriority(EXTI3_IRQn,12,0);       //抢占优先级为12，子优先级为0
        HAL_NVIC_EnableIRQ(EXTI3_IRQn);              //使能中断线13          
    }
		
    return &key_dev;
}

//按键处理函数,非中断情况下调用
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//0，没有任何按键按下
//1，WKUP按下 WK_UP
//注意此函数有响应优先级,KEY0>KEY1>KEY2>WK_UP!!
u8 key_scan(u8 mode)
{
    static u8 key_up=1;     //按键松开标志
    if(mode==1)key_up=1;    //支持连按
    if(key_up&&(KEY0==0||KEY1==0||KEY2==0||WK_UP==1))
    {
        delay_ms(10);
        key_up=0;
        if(KEY0==0)       return KEY0_PRES;
        else if(KEY1==0)  return KEY1_PRES;
        else if(KEY2==0)  return KEY2_PRES;
        else if(WK_UP==0) return WKUP_PRES;          
    }else if(KEY0==1&&KEY1==1&&KEY2==1&&WK_UP==1)key_up=1;
    return 0;   //无按键按下
}


//中断服务函数
void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);//调用中断处理公用函数
}

void EXTI1_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);//调用中断处理公用函数
}

void EXTI2_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);//调用中断处理公用函数
}

void EXTI3_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);//调用中断处理公用函数
}

//中断服务程序中需要做的事情
//在HAL库中所有的外部中断服务函数都会调用此函数
//GPIO_Pin:中断引脚号
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    switch(GPIO_Pin)
    {
        case GPIO_PIN_0:
            if(WK_UP==0) 
            {
                key_dev.key_event  = WKUP_PRES;
                key_dev.start_tick = HAL_GetTick();
            }
            break;
            
        case GPIO_PIN_1:
            if (KEY2==0)  //
            {
                key_dev.key_event  = KEY2_PRES;
                key_dev.start_tick = HAL_GetTick();           
                
            }
            break;
            
        case GPIO_PIN_2:
            if(KEY1==0)  
            {
                key_dev.key_event  = KEY1_PRES;
                key_dev.start_tick = HAL_GetTick();
                
            }
            break;
            
        case GPIO_PIN_3:
            
            if(KEY0==0)  //
            {
                
                key_dev.key_event  = KEY0_PRES;
                key_dev.start_tick = HAL_GetTick();
            }
            break;
    }
}


//注册按键事件回调
void atk_key_registercb (key_handle_t key_handle, key_cb cb, void *p_arg)
{
    if ((cb != NULL) && (key_handle != NULL))
    {
        key_handle->key_cb = cb;
        key_handle->p_arg  = p_arg;
    }
}



//轮询按钮事件
void atk_key_event_poll(key_handle_t key_handle)
{
  uint8_t key_event = 0;
    
  if(key_dev.key_event)
  { 
    if(HAL_GetTick() - key_dev.start_tick >= KEY_DELAY_TICK )
    {
      if(key_dev.key_event & WKUP_PRES)
      {
        if(WK_UP == GPIO_PIN_RESET)
        {
          key_event |= WKUP_PRES;
        }

        key_dev.key_event ^= WKUP_PRES;
      }
      
      if(key_dev.key_event & KEY2_PRES)
      {
        if(KEY2 == GPIO_PIN_RESET)
        {
          key_event |= KEY2_PRES;
        }
        key_dev.key_event ^= KEY2_PRES;
      }
      
      if(key_dev.key_event & KEY1_PRES)
      {
        if(KEY1 == GPIO_PIN_RESET)
        {
          key_event |= KEY1_PRES;
        }
        key_dev.key_event ^= KEY1_PRES;
      }
      
      if(key_dev.key_event & KEY0_PRES)
      {
        if(KEY0 == GPIO_PIN_RESET)
        {
          key_event |= KEY0_PRES;
        }
        key_dev.key_event ^= KEY0_PRES;
      }
    }
  }
  
  //如果真的有按钮按下，则执行回调函数
  if(key_event && key_handle->key_cb)
  {
    key_handle->key_cb(key_event, key_handle->p_arg);
  }
}
