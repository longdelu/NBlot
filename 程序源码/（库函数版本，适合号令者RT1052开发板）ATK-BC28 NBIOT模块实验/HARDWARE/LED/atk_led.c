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
    gpio_pin_config_t led_config;
    
    //IO功能设置
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_03_GPIO1_IO03,0);    //GPIO_AD_B0_03配置为ALT5,即GPIO1_3
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_03_GPIO3_IO03,0);    //GPIO_SD_B1_03配置为ALT5,即GPIO3_3
    
    //配置IO引脚GPIO_AD_B0_03和GPIO_SD_B1_03的功能
    //低转换速度,驱动能力为R0/6,速度为100Mhz，关闭开路功能，使能pull/keepr
    //选择keeper功能，下拉100K Ohm，关闭Hyst
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_03_GPIO1_IO03,0x10B0);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_03_GPIO3_IO03,0x10B0);

    led_config.direction=kGPIO_DigitalOutput;    //输出
    led_config.interruptMode=kGPIO_NoIntmode;    //不使用中断功能
    led_config.outputLogic=1;                    //默认高电平，LED灯关闭
    GPIO_PinInit(GPIO1,3,&led_config);             //初始化GPIO1_3
    
    led_config.direction=kGPIO_DigitalOutput;    //输出
    led_config.interruptMode=kGPIO_NoIntmode;    //不使用中断功能
    led_config.outputLogic=1;                    //默认高电平，LED灯关闭
    GPIO_PinInit(GPIO3,3,&led_config);           //初始化GPIO3_3 
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
       LED0(0);        
        
    }
    
    else 
    {
      
       LED1(0);  
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
       LED0(1);        
        
    }
    
    else 
    {
       LED1(1);        
      
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

