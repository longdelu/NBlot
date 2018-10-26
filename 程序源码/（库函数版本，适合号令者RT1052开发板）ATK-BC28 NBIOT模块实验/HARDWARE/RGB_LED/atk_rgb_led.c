#include "atk_rgb_led.h"

//////////////////////////////////////////////////////////////////////////////////     
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//P9813 RGB LED驱动代码            
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2016/10/20
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved                                      
//////////////////////////////////////////////////////////////////////////////////  

 //sda高低电平设置
void atk_sda_set(int leve)
{
    SDA(leve);          
}

//scl高低电平设置
void atk_scl_set(int leve)
{
    SCL(leve);  
}


//发送前32位'0'起始帧
void RGBLED_Send32Zero(void)
{ 
     u8 i;
    
     atk_sda_set(0);
    
     for(i=0;i<32;i++)
     {
        atk_scl_set(0);
        delay_us(200);
        atk_scl_set(1);
        delay_us(200);        
     } 
}

//反转前两位的灰度值
u8 RGBLED_TakeAntiCode(u8 dat)
{
     u8 tmp = 0;
    
     tmp = ((~dat) & 0xC0) >> 6;
     return tmp;
}

//发送灰度数据
void RGBLED_DatSend(u32 dx)
{
     u8 i;
     
     for(i=0;i<32;i++)
     {
        if((dx & 0x80000000) !=0)
           atk_sda_set(1);
        else 
         atk_sda_set(0);
        
        dx<<=1;
        atk_scl_set(0);delay_us(200);
        atk_scl_set(1);delay_us(200);
     }    
}

//数据处理与发送
//r;红色值 0~255
//g:绿色值 0~255
//b:蓝色值 0~255  
void RGBLED_DataDealWithAndSend(u8 r,u8 g,u8 b)
{ 
     u32 dx=0;
    
     dx |= (u32)0x03 << 30;  //前两位'1'位标志位
     dx |= (u32)RGBLED_TakeAntiCode(b) << 28;
     dx |= (u32)RGBLED_TakeAntiCode(g) << 26;
     dx |= (u32)RGBLED_TakeAntiCode(r) << 24;
    
     dx |= (u32)b << 16;
     dx |= (u32)g << 8;
     dx |= r;
    
     RGBLED_DatSend(dx);//发送数据
}
//RGB灯控制函数
//r;红色值 0~255
//g:绿色值 0~255
//b:蓝色值 0~255
void RGBLED_Show(u8 r,u8 g,u8 b)
{
     RGBLED_Send32Zero();//发送前32位'0'起始帧
     RGBLED_DataDealWithAndSend(r, g, b);//发送32bit灰度数据
     RGBLED_DataDealWithAndSend(r, g, b);
} 

//RGB灯初始化函数
void RGBLED_Init(void) 
{
    gpio_pin_config_t led_config;
    
    //IO功能设置
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_03_GPIO1_IO19,0);    //GPIO_AD_B0_03配置为ALT5,即GPIO1_19
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_09_GPIO1_IO09,0);    //GPIO_SD_B1_03配置为ALT5,即GPIO1_09
    
    //配置IO引脚GPIO_AD_B1_03和GPIO_AD_B0_09的功能
    //低转换速度,驱动能力为R0/6,速度为100Mhz，关闭开路功能，使能pull/keepr
    //选择keeper功能，下拉100K Ohm，关闭Hyst
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_03_GPIO1_IO19, 0x10B0);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_09_GPIO1_IO09, 0x10B0);

    led_config.direction=kGPIO_DigitalOutput;    //输出
    led_config.interruptMode=kGPIO_NoIntmode;    //不使用中断功能
    led_config.outputLogic=1;                    //默认高电平，LED灯关闭
    GPIO_PinInit(GPIO1,19, &led_config);          //初始化GPIO1_19
    
    led_config.direction=kGPIO_DigitalOutput;    //输出
    led_config.interruptMode=kGPIO_NoIntmode;    //不使用中断功能
    led_config.outputLogic=1;                    //默认高电平，LED灯关闭
    GPIO_PinInit(GPIO1,9,&led_config);           //初始化GPIO1_09
}

