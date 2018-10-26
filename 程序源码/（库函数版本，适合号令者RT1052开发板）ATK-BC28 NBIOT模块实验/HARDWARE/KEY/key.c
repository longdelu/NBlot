#include "key.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////     
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK I.MX RT1052开发板
//按键驱动代码       
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2012/12/15
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved                                      
//////////////////////////////////////////////////////////////////////////////////     

//按键初始化函数
//KEY_UP    ->  WAKEUP(GPIO5_IO00)
//KEY0      ->  GPIO_AD_B0_05(GPIO1_IO05)
//KEY1      ->  PMIC_ON_REQ(GPIO5_IO01)
//KEY2      ->  GPIO_EMC_40(GPIO3_IO26)
void KEY_Init(void)
{
     gpio_pin_config_t key_config;
    
    //IO功能设置
    IOMUXC_SetPinMux(IOMUXC_SNVS_WAKEUP_GPIO5_IO00,0);            //SNVS_WAKEUP配置为ALT5,即GPIO5_00
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_05_GPIO1_IO05,0);        //GPIO_AD_B0_05配置为ALT5,即GPIO1_IO05
    IOMUXC_SetPinMux(IOMUXC_SNVS_PMIC_ON_REQ_GPIO5_IO01,0);     //PMIC_ON_REQ,即GPIO5_IO01
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_40_GPIO3_IO26,0);            //GPIO_EMC_40配置为ALT5,即GPIO3_IO26
    
    //配置KEY相关IO的功能
    //低转换速度,关闭输出驱动,速度为100Mhz，关闭开路功能，使能pull/keepr
    //选择pull功能，上拉22K Ohm，关闭Hyst
    IOMUXC_SetPinConfig(IOMUXC_SNVS_WAKEUP_GPIO5_IO00,0xF080);      
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_05_GPIO1_IO05,0xF080);    
    IOMUXC_SetPinConfig(IOMUXC_SNVS_PMIC_ON_REQ_GPIO5_IO01,0xF080);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_40_GPIO3_IO26,0xF080);

    //KEY_UP默认电平设置
    key_config.direction=kGPIO_DigitalInput;    //输入
    key_config.interruptMode=kGPIO_NoIntmode;    //不使用中断功能
    key_config.outputLogic=1;                    //默认高电平
    GPIO_PinInit(GPIO5,0,&key_config);             //初始化GPIO5_00 
    
    //KEY0默认电平设置
    key_config.direction=kGPIO_DigitalInput;    //输入
    key_config.interruptMode=kGPIO_NoIntmode;    //不使用中断功能
    key_config.outputLogic=1;                    //默认高电平
    GPIO_PinInit(GPIO1,5,&key_config);             //初始化GPIO1_05
    
    //KEY1默认电平设置
    key_config.direction=kGPIO_DigitalInput;    //输入
    key_config.interruptMode=kGPIO_NoIntmode;    //不使用中断功能
    key_config.outputLogic=1;                    //默认高电平
    GPIO_PinInit(GPIO5,1,&key_config);             //初始化GPIO5_01
    
    //KEY2默认电平设置
    key_config.direction=kGPIO_DigitalInput;    //输入
    key_config.interruptMode=kGPIO_NoIntmode;    //不使用中断功能
    key_config.outputLogic=1;                    //默认高电平
    GPIO_PinInit(GPIO3,26,&key_config);         //初始化GPIO3_26 
}

//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//0，没有任何按键按下
//1，WKUP按下 WK_UP
//注意此函数有响应优先级,KEY0>KEY1>KEY2>WK_UP!!
u8 KEY_Scan(u8 mode)
{
    static u8 key_up=1;     //按键松开标志
    if(mode==1)key_up=1;    //支持连按
    if(key_up&&(KEY1==0||KEY2==0||KEY0==0||WK_UP==0))
    {
        delay_ms(10);
        key_up=0;
        if(KEY0==0)       return KEY0_PRES;
        else if(KEY1==0)  return KEY1_PRES;
        else if(KEY2==0)  return KEY2_PRES;
        else if(WK_UP==0) return WKUP_PRES;      
    }else if(KEY1==1&&KEY2==1&&KEY0==1&&WK_UP==1)key_up=1;
    return 0;   //无按键按下
}
