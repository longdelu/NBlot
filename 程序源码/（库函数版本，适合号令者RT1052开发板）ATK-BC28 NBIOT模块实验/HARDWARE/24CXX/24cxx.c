#include "24cxx.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////     
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK RT1052开发板
//24CXX驱动代码       
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2018/3/7
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved                                      
//////////////////////////////////////////////////////////////////////////////////     

lpi2c_master_transfer_t at24cxx_xfer; //传输结构体

//初始化IIC接口
void AT24CXX_Init(void)
{
    LPI2C1_Init(400000);  //初始化LPI2C1
}

//在AT24CXX指定地址读出一个数据
//ReadAddr:开始读数的地址  
//val:读取到的数据
//返回值:0 读取成功，其他值：读取失败
u8 AT24CXX_ReadOneByte(u16 ReadAddr,u8 *val)
{                  
    u8 status=0;

    //配置I2C xfer结构体
    at24cxx_xfer.slaveAddress=AT24C02_ADDR;     //设备地址
    at24cxx_xfer.direction=kLPI2C_Read;         //读数据
    at24cxx_xfer.subaddress=(u32)ReadAddr;      //要读取的寄存器地址
    at24cxx_xfer.subaddressSize=1;              //子地址长度
    at24cxx_xfer.data=val;                      //数据缓冲区
    at24cxx_xfer.dataSize=1;                    //要读取的数据长度
    at24cxx_xfer.flags=kLPI2C_TransferDefaultFlag;

    if(LPI2C_MasterTransferBlocking(LPI2C1,&at24cxx_xfer)==kStatus_Fail)
        status=1;
    return status;
}

//在AT24CXX指定地址写入一个数据
//WriteAddr  :写入数据的目的地址    
//DataToWrite:要写入的数据
//返回值:0 写入成功，其他值：写入失败
u8 AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{    
    u8 status=0;
    u8 writedata=0;
    
    writedata=DataToWrite;
    
    //配置I2C xfer结构体
    at24cxx_xfer.slaveAddress=AT24C02_ADDR;     //设备地址
    at24cxx_xfer.direction=kLPI2C_Write;        //写入数据
    at24cxx_xfer.subaddress=(u32)WriteAddr;     //要读取的寄存器地址
    at24cxx_xfer.subaddressSize=1;              //地址长度一个字节
    at24cxx_xfer.data=&writedata;               //要写入的数据
    at24cxx_xfer.dataSize=1;                    //写入数据长度1个字节
    at24cxx_xfer.flags=kLPI2C_TransferDefaultFlag;
    
    if(LPI2C_MasterTransferBlocking(LPI2C1,&at24cxx_xfer)==kStatus_Fail)
        status=1;
    
    delay_ms(10);      //AT24C02写周期最大5ms，所以这里要有延时
    return status;     
}

//在AT24CXX里面的指定地址开始写入长度为Len的数据
//该函数用于写入16bit或者32bit的数据.
//WriteAddr  :开始写入的地址  
//DataToWrite:数据数组首地址
//Len        :要写入数据的长度2,4
void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len)
{      
    u8 t;
    for(t=0;t<Len;t++)
    {
        AT24CXX_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
    }                                                    
}

//在AT24CXX里面的指定地址开始读出长度为Len的数据
//该函数用于读出16bit或者32bit的数据.
//ReadAddr   :开始读出的地址 
//返回值     :数据
//Len        :要读出数据的长度2,4
u32 AT24CXX_ReadLenByte(u16 ReadAddr,u8 Len)
{      
    u8 t;
    u8 val=0;
    u32 temp=0;

    for(t=0;t<Len;t++)
    {
        temp<<=8;
        AT24CXX_ReadOneByte(ReadAddr+Len-t-1,&val);      
        temp+=val;
    }
    return temp;                                                    
}

//检查AT24CXX是否正常
//这里用了24XX的最后一个地址(255)来存储标志字.
//如果用其他24C系列,这个地址要修改
//返回1:检测失败
//返回0:检测成功
u8 AT24CXX_Check(void)
{
    u8 temp;
    AT24CXX_ReadOneByte(255,&temp);//避免每次开机都写AT24CXX               
    if(temp==0X55)return 0;           
    else//排除第一次初始化的情况
    {
        AT24CXX_WriteOneByte(255,0X55);
        AT24CXX_ReadOneByte(255,&temp);      
        if(temp==0X55)return 0;
    }
    return 1;                                              
}

//在AT24CXX里面的指定地址开始读出指定个数的数据
//ReadAddr :开始读出的地址 对24c02为0~255
//pBuffer  :数据数组首地址
//NumToRead:要读出数据的个数
void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
{
    u8 val=0;
    
    while(NumToRead)
    {
        AT24CXX_ReadOneByte(ReadAddr++,&val);
        *pBuffer++=val;
        NumToRead--;
    }
}  

//在AT24CXX里面的指定地址开始写入指定个数的数据
//WriteAddr :开始写入的地址 对24c02为0~255
//pBuffer   :数据数组首地址
//NumToWrite:要写入数据的个数
void AT24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite)
{
    while(NumToWrite--)
    {
        AT24CXX_WriteOneByte(WriteAddr,*pBuffer);
        WriteAddr++;
        pBuffer++;
    }
}
