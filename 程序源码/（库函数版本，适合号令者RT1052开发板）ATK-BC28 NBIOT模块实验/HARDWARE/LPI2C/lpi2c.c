#include "lpi2c.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////     
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK I.MX RT1052开发板
//I2C驱动代码       
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2018/1/9
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved                                      
//////////////////////////////////////////////////////////////////////////////////     

lpi2c_master_config_t   lpi2c1_config;              //I2C1(主机)配置结构体
lpi2c_master_handle_t   lpi2c1_handle;              //I2C1(主机)句柄

volatile bool lpi2c1_compflag = false;       //完成标志
volatile bool lpi2c1_nakflag = false;        //nak标志

//LPI2C1完成回调函数
void LPI2C1_Mastercallback(LPI2C_Type *base, lpi2c_master_handle_t *handle, status_t status, void *userData)
{
    //数据传输完成
    if(status==kStatus_Success)
        lpi2c1_compflag=true;
    if (status==kStatus_LPI2C_Nak)
        lpi2c1_nakflag=true;
}

//初始化I2C
//baudrate:IIC波特率
void LPI2C1_Init(u32 baudrate)
{
    u32 i2cclk=0;
    
    //设置I2C的时钟源为USB1(PLL3)=480/8Mhz=60Mhz
    CLOCK_SetMux(kCLOCK_Lpi2cMux,0);   //LPI2C时钟源为PLL3=60Mhz
    CLOCK_SetDiv(kCLOCK_Lpi2cDiv,5);   //分频设置，分频值为5+1，所以LPI2C CLK=10Mhz
    
    //配置IO作为I2C
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_00_LPI2C1_SCL,1); 
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_01_LPI2C1_SDA,1); 
    
    //配置I2C引脚功能，
    //低转换速度,驱动能力为R0/6,速度为100Mhz，开路输出，使能pull/keepr
    //选择keeper功能，上拉22K Ohm，关闭Hyst
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_00_LPI2C1_SCL,0xD8B0); 
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_01_LPI2C1_SDA,0xD8B0); 
    
    i2cclk=((CLOCK_GetFreq(kCLOCK_Usb1PllClk)/8)/(5+1)); 
    
    //配置I2C1作为I2C主机
    LPI2C_MasterGetDefaultConfig(&lpi2c1_config);       //先配置为默认配置
    lpi2c1_config.baudRate_Hz=baudrate;                 //设置I2C速率
    LPI2C_MasterInit(LPI2C1,&lpi2c1_config,i2cclk);     //初始化I2C1 
   
    /****************如果使用IIC非阻塞传输的话需要设置handler*************************
    LPI2C_MasterTransferCreateHandle(LPI2C1,&lpi2c1_handle,LPI2C1_Mastercallback, NULL);
    **********************************************************************************/
}

