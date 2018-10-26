#ifndef __DS18B20_H
#define __DS18B20_H
#include "sys.h"
#include "atk_bsp.h"
//////////////////////////////////////////////////////////////////////////////////     
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK I.MX RT1052开发板
//DS18B20驱动代码       
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/12/30
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved                                      
//////////////////////////////////////////////////////////////////////////////////     

//IO方向设置
//#define DS18B20_IO_IN()  {GPIOB->MODER&=~(3<<(12*2));GPIOB->MODER|=0<<12*2;}    //PB12输入模式
//#define DS18B20_IO_OUT() {GPIOB->MODER&=~(3<<(12*2));GPIOB->MODER|=1<<12*2;}     //PB12输出模式

#define DS18B20_IO_IN()                                             \
{                                                                   \
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_11_GPIO1_IO11,0);            \
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_11_GPIO1_IO11,0xF080);    \
    gpio_pin_config_t datain_config;                                \
    datain_config.direction=kGPIO_DigitalInput;                     \
    datain_config.interruptMode=kGPIO_NoIntmode;                    \
    datain_config.outputLogic=1;                                    \
    GPIO_PinInit(GPIO1,11,&datain_config);                          \
}

//SDA GPIO_B1_12输出模式
#define DS18B20_IO_OUT()                                            \
{                                                                   \
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_11_GPIO1_IO11,0);            \
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_11_GPIO1_IO11,0x10B0);    \
    gpio_pin_config_t dataout_config;                               \
    dataout_config.direction=kGPIO_DigitalOutput;                   \
    dataout_config.interruptMode=kGPIO_NoIntmode;                   \
    dataout_config.outputLogic=1;                                   \
    GPIO_PinInit(GPIO1,11,&dataout_config);                         \
}

//IO操作函数                                               
#define    DS18B20_DQ_OUT(n)   (n?GPIO_PinWrite(GPIO1,11,1):GPIO_PinWrite(GPIO1,11,0)) //数据端口
#define    DS18B20_DQ_IN       GPIO_PinRead(GPIO1,11)  //数据端口    
       
u8 DS18B20_Init(void);            //初始化DS18B20
short DS18B20_Get_Temp(void);    //获取温度
void DS18B20_Start(void);        //开始温度转换
void DS18B20_Write_Byte(u8 dat);//写入一个字节
u8 DS18B20_Read_Byte(void);        //读出一个字节
u8 DS18B20_Read_Bit(void);        //读出一个位
u8 DS18B20_Check(void);            //检测是否存在DS18B20
void DS18B20_Rst(void);            //复位DS18B20 
#endif
