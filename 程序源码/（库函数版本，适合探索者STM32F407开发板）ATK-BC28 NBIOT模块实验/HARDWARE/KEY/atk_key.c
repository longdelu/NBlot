#include "key.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////     
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//KEY驱动代码       
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2017/4/6
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved                                      
//////////////////////////////////////////////////////////////////////////////////

static key_dev_t key_dev = {0,0, NULL, NULL};



/**
  * @brief  按键初始化函数
  * @param  None
  * @retval 按键设备句柄的指针
  */
key_handle_t atk_key_exit_init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOA_CLK_ENABLE();               //开启GPIOA时钟
    __HAL_RCC_GPIOE_CLK_ENABLE();               //开启GPIOE时钟
    
    GPIO_Initure.Pin=GPIO_PIN_0;                //PA0
    GPIO_Initure.Mode=GPIO_MODE_IT_RISING;      //上升沿触发
    GPIO_Initure.Pull=GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
    
    GPIO_Initure.Pin=GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4; //PE2,3,4
    GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;            //下降沿触发
    GPIO_Initure.Pull=GPIO_PULLUP;
    HAL_GPIO_Init(GPIOE,&GPIO_Initure);
    
    //中断线0-PA0
    HAL_NVIC_SetPriority(EXTI0_IRQn,2,0);       //抢占优先级为2，子优先级为0
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);             //使能中断线0
    
    //中断线2-PE2
    HAL_NVIC_SetPriority(EXTI2_IRQn,2,1);       //抢占优先级为2，子优先级为1
    HAL_NVIC_EnableIRQ(EXTI2_IRQn);             //使能中断线2
    
    //中断线3-PE3
    HAL_NVIC_SetPriority(EXTI3_IRQn,2,2);       //抢占优先级为2，子优先级为2
    HAL_NVIC_EnableIRQ(EXTI3_IRQn);             //使能中断线2
    
    //中断线4-PE4
    HAL_NVIC_SetPriority(EXTI4_IRQn,2,3);       //抢占优先级为2，子优先级为3
    HAL_NVIC_EnableIRQ(EXTI4_IRQn);             //使能中断线4
        
    return &key_dev;     
}

/**
  * @brief  中断服务函数.
  */
void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);//调用中断处理公用函数
}

/**
  * @brief  中断服务函数.
  */
void EXTI2_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);//调用中断处理公用函数
}

/**
  * @brief  中断服务函数.
  */
void EXTI3_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);//调用中断处理公用函数
}

/**
  * @brief  中断服务函数.
  */
void EXTI4_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);//调用中断处理公用函数
}

/**
  * @brief  HAL库外部中断服务函数都会调用此函数.
  * @param  GPIO_Pin : 中断引脚号.
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    switch(GPIO_Pin)
    {
        case GPIO_PIN_0:
            if(WK_UP==GPIO_PIN_SET) 
            {
                key_dev.key_event  = WKUP_PRES;
                key_dev.start_tick = HAL_GetTick();
            }
            break;
            
        case GPIO_PIN_4:
            if (KEY0==GPIO_PIN_RESET)  //
            {
                key_dev.key_event  = KEY0_PRES;
                key_dev.start_tick = HAL_GetTick();           
                
            }
            break;
            
        case GPIO_PIN_2:
            if(KEY2==GPIO_PIN_RESET)  
            {
                key_dev.key_event  = KEY2_PRES;
                key_dev.start_tick = HAL_GetTick();
                
            }
            break;
            
        case GPIO_PIN_3:
            
            if(KEY1==GPIO_PIN_RESET)  //
            {
                
                key_dev.key_event  = KEY1_PRES;
                key_dev.start_tick = HAL_GetTick();
            }
            break;
            
        default:
                
            break;
    }
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
        if (HAL_GetTick() - key_dev.start_tick >= KEY_DELAY_TICK )
        {
            if (key_dev.key_event & WKUP_PRES)
            {
                if (WK_UP == GPIO_PIN_SET)
                {
                    key_event |= WKUP_PRES;
                }

                key_dev.key_event ^= WKUP_PRES;
            }
          
            if(key_dev.key_event & KEY2_PRES)
            {
              
                if (KEY2 == GPIO_PIN_RESET)
                {
                     key_event |= KEY2_PRES;
                }
                key_dev.key_event ^= KEY2_PRES;
            }
          
           if (key_dev.key_event & KEY1_PRES)
           {
                if (KEY1 == GPIO_PIN_RESET)
                {
                     key_event |= KEY1_PRES;
                }
                key_dev.key_event ^= KEY1_PRES;
           }
          
           if (key_dev.key_event & KEY0_PRES)
           {
                if (KEY0 == GPIO_PIN_RESET)
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
