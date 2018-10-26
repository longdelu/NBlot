#include "atk_led.h"
//////////////////////////////////////////////////////////////////////////////////     
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F7开发板
//LED驱动代码       
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/11/27
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved                                      
//////////////////////////////////////////////////////////////////////////////////     
          

/**
  * @brief  led初始化
  * @param  None
  * @retval None
  */
void atk_led_init(void)
{
  
    GPIO_InitTypeDef  GPIO_InitStructure;
  
    //使能PB,PE端口时钟   
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOE, ENABLE);    
      
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;                 //LED0-->PB.5 端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;          //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;         //IO口速度为50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);                    //根据设定参数初始化GPIOB.5
    GPIO_SetBits(GPIOB,GPIO_Pin_5);                           //PB.5 输出高

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;                  //LED1-->PE.5 端口配置, 推挽输出
    GPIO_Init(GPIOE, &GPIO_InitStructure);                     //推挽输出 ，IO口速度为50MHz
    GPIO_SetBits(GPIOE,GPIO_Pin_5);                            //PE.5 输出高   

}


/**
  * @brief  led亮
  * @param  led_id  
  * @retval None
  */
void atk_led_on(int led_id)
{
  
    if (0 == led_id)
    {
       LED0 = 0;                
    }
    
    else 
    {
      
       LED1 = 0;  
    }

}


/**
  * @brief  led灭
  * @param  led_id
  * @retval None
  */
void atk_led_off(int led_id)
{
    if (0 == led_id)
    {
       LED0 = 1;        
        
    }
    
    else 
    {
       LED1 = 1;        
      
    }
}

/**
  * @brief  led翻转
  * @param  led_id
  * @retval None
  */
void atk_led_toggle(int led_id)
{
    if (0 == led_id)
    {
      
       LED0_Toggle; 
    }
    
    else 
    {
      
       LED1_Toggle;
    }
}

