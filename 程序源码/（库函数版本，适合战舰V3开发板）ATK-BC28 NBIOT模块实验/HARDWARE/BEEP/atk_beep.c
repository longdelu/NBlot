#include "atk_beep.h"
//////////////////////////////////////////////////////////////////////////////////     
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板V3
//蜂鸣器驱动代码       
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/1/10
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved                                      
//////////////////////////////////////////////////////////////////////////////////        

//初始化PB8为输出口.并使能这个口的时钟            
//蜂鸣器初始化
void beep_init(void)
{
   GPIO_InitTypeDef  GPIO_InitStructure;
    
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//使能GPIOB端口时钟
   
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;            //BEEP-->PB.8 端口配置
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     //推挽输出
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    //速度为50MHz
   GPIO_Init(GPIOB, &GPIO_InitStructure);               //根据参数初始化GPIOB.8
   
   GPIO_ResetBits(GPIOB,GPIO_Pin_8);                    //输出0，关闭蜂鸣器输出  
}

/**
  * @brief  蜂鸣器鸣叫
  * @param  None
  * @retval None
  */
void beep_on(void)
{  
    GPIO_SetBits(GPIOB,GPIO_Pin_8);                    //输出1，打开蜂鸣器输出    
}


/**
  * @brief  蜂鸣器关闭
  * @param  None
  * @retval None
  */
void beep_off(void)
{  
    GPIO_ResetBits(GPIOB,GPIO_Pin_8);                    //输出0，关闭蜂鸣器输出  
}







