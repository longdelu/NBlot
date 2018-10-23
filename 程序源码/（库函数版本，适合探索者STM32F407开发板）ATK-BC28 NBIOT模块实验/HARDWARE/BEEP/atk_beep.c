#include "atk_beep.h"
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
void beep_init(void)
{

    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOF_CLK_ENABLE();           //开启GPIOF时钟
    
    GPIO_Initure.Pin=GPIO_PIN_8;            //PF8
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
    HAL_GPIO_Init(GPIOF,&GPIO_Initure);
    
    HAL_GPIO_WritePin(GPIOF,GPIO_PIN_8, GPIO_PIN_RESET);    //蜂鸣器对应引脚GPIOF8拉低

}


/**
  * @brief  蜂鸣器鸣叫
  * @param  None
  * @retval None
  */
void beep_on(void)
{
  
    HAL_GPIO_WritePin(GPIOF,GPIO_PIN_8, GPIO_PIN_SET);      //打开蜂鸣器  
  
}


/**
  * @brief  蜂鸣器关闭
  * @param  None
  * @retval None
  */
void beep_off(void)
{  
    HAL_GPIO_WritePin(GPIOF,GPIO_PIN_8, GPIO_PIN_RESET);      //关闭蜂鸣器  
}
