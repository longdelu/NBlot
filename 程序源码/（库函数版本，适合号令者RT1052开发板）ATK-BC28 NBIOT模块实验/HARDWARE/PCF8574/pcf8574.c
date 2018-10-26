#include "pcf8574.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////     
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F429开发板
//PCF8574驱动代码       
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2016/1/13
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved                                      
//////////////////////////////////////////////////////////////////////////////////     

lpi2c_master_transfer_t pcf8574_xfer; //传输结构体

//初始化PCF8574
u8 PCF8574_Init(void)
{
    u8 temp=0;
    u8 data=0XFF;
    status_t status=kStatus_Success;
    
    gpio_pin_config_t iicint_config;
    
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_00_GPIO3_IO00,0);  
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_00_GPIO3_IO00,0xF080); 
    
    //IIC_INT默认电平设置
    iicint_config.direction=kGPIO_DigitalInput;        //输入
    iicint_config.interruptMode=kGPIO_NoIntmode;    //不使用中断功能
    iicint_config.outputLogic=1;                    //默认高电平
    GPIO_PinInit(GPIO3,0,&iicint_config);             //初始化GPIO3_00 
    
    LPI2C1_Init(10000);                                //IIC初始化
    
    status=LPI2C_MasterStart(LPI2C1,PCF8574_ADDR,kLPI2C_Write);
    status=LPI2C_MasterSend(LPI2C1,&data,1);               //默认情况下所有IO输出高电平
    status=LPI2C_MasterStop(LPI2C1);   
    if(status==kStatus_Fail) 
        temp=1;
    return temp;
}

//读取PCF8574的8位IO值
//返回值:读到的数据
u8 PCF8574_ReadOneByte(void)
{                  
    u8 val=0;
    
    LPI2C_MasterStart(LPI2C1,PCF8574_ADDR,kLPI2C_Read);
    LPI2C_MasterReceive(LPI2C1,&val,1);
    LPI2C_MasterStop(LPI2C1);

    return val;
}

//向PCF8574写入8位IO值  
//DataToWrite:要写入的数据
void PCF8574_WriteOneByte(u8 DataToWrite)
{    
    u8 data=DataToWrite;
    
    LPI2C_MasterStart(LPI2C1,PCF8574_ADDR,kLPI2C_Write);
    LPI2C_MasterSend(LPI2C1,&data,1);
    LPI2C_MasterStop(LPI2C1);
    
    delay_ms(10);     
}

//设置PCF8574某个IO的高低电平
//bit:要设置的IO编号,0~7
//sta:IO的状态;0或1
void PCF8574_WriteBit(u8 bit,u8 sta)
{
    u8 data;
    data=PCF8574_ReadOneByte(); //先读出原来的设置
    if(sta==0)data&=~(1<<bit);     
    else data|=1<<bit;
    PCF8574_WriteOneByte(data); //写入新的数据
}

//读取PCF8574的某个IO的值
//bit：要读取的IO编号,0~7
//返回值:此IO的值,0或1
u8 PCF8574_ReadBit(u8 bit)
{
    u8 data;
    data=PCF8574_ReadOneByte(); //先读取这个8位IO的值 
    if(data&(1<<bit))return 1;
    else return 0;   
}  
    
