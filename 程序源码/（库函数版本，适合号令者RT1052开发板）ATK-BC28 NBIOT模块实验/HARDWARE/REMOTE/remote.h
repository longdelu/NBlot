#ifndef __REMOTE_H
#define __REMOTE_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK 号令者RT1052开发板
//红外遥控解码驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2018/5/12
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

#define RDATA   GPIO_PinRead(GPIO3,27) //红外数据输入脚

//红外遥控识别码(ID),每款遥控器的该值基本都不一样,但也有一样的.
//我们选用的遥控器识别码为0
#define REMOTE_ID 0    

//GPIO_EMC_41作为定时器引脚
#define REMOTE_IO_GPT()                                             \
{                                                                   \
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_41_GPT2_CAPTURE1,0);           \
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_41_GPT2_CAPTURE1,0xF0F1);   \
}

//SDA GPIO_EMC_41作为GPIO
#define REMOTE_IO_GPIO()                                            \
{                                                                   \
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_41_GPIO3_IO27,0);              \
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_41_GPIO3_IO27,0xF080);      \
    gpio_pin_config_t datain_config;                                \
    datain_config.direction=kGPIO_DigitalInput;                     \
    datain_config.interruptMode=kGPIO_NoIntmode;                    \
    GPIO_PinInit(GPIO3,27,&datain_config);                          \
}


extern u8 RmtCnt;	        //按键按下的次数

void Remote_Init(void);     //红外传感器接收头引脚初始化
u8 Remote_Scan(void);
#endif
