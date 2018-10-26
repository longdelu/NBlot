#include "lpspi.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////     
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK I.MX RT1052开发板
//LPSPI驱动代码       
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2018/3/7
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved                                      
//////////////////////////////////////////////////////////////////////////////////     

/******************************************************  
  SPI3作为主设备MASTER                            
  LPSPI3_SCK(GPIO_AD_B0_00)     
  LPSPI3_SDI(GPIO_AD_B0_02)    
  LPSPI3_SDO(GPIO_AD_B0_01)  
******************************************************/
lpspi_master_config_t lpspi3_config; //LPSPI3主机配置结构体

//初始化SPI3
//baudrate:波特率，单位Mhz，SPI最大120M波特率
void LPSPI3_Init(u8 baudrate)
{
    u32 lpspiclk=0;
    u32 spi3_baudrate=baudrate*1000000;
    
    //设置LPSPI的时钟源为PLL3_PFD0/(1+5)=576Mhz/(1+5)=96MHz,W25Q256时钟最高104MHz
    CLOCK_SetMux(kCLOCK_LpspiMux,1);    //USB1 PLL PFD0(576MHz)作为LPSPI时钟源
    CLOCK_SetDiv(kCLOCK_LpspiDiv,5);    //分频设置，分频值为5+1，所以LPSPI CLK=576/(1+5)=96Mhz
    
    //配置IO作为LPSPI3，片选采用软件的，这样一个SPI的外设就不会限制为4个!
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_00_LPSPI3_SCK,0);                                    
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_01_LPSPI3_SDO,0);                                    
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_02_LPSPI3_SDI,0);                                    
    
    //配置LPSPI引脚功能，
    //低转换速度,驱动能力为R0/6,速度为100Mhz，开路输出，使能pull/keepr
    //选择keeper功能，下拉100K Ohm，关闭Hyst
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_00_LPSPI3_SCK,0x10B0);                              
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_01_LPSPI3_SDO,0x10B0);                                                                              
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_02_LPSPI3_SDI,0x10B0);                                
    
    //配置LPSPI3主模式
    lpspiclk=(CLOCK_GetFreq(kCLOCK_Usb1PllPfd0Clk)/(5+1));//LPSPI3时钟
    lpspi3_config.baudRate=spi3_baudrate;               //SPI速度
    lpspi3_config.whichPcs=kLPSPI_Pcs1;                 //片选信号，PCS1
    lpspi3_config.pcsActiveHighOrLow=kLPSPI_PcsActiveLow;//片选信号低电平有效
    lpspi3_config.bitsPerFrame=8;                       //设置SPI的数据大小:SPI发送接收8位帧结构
    lpspi3_config.cpol=kLPSPI_ClockPolarityActiveLow;   //串行同步时钟低电平有效
    lpspi3_config.cpha=kLPSPI_ClockPhaseSecondEdge;      //串行同步时钟的第二个跳变沿（上升或下降）数据被采样
    lpspi3_config.direction=kLPSPI_MsbFirst;            //指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
    lpspi3_config.pinCfg=kLPSPI_SdiInSdoOut;            //SDI输入引脚，SDO输出引脚
    lpspi3_config.dataOutConfig=kLpspiDataOutRetained;  //输出数据保留
    lpspi3_config.pcsToSckDelayInNanoSec=10;            //片选拉低到时钟有效之间的延时时间,单位ns
    lpspi3_config.lastSckToPcsDelayInNanoSec=10;        //最后一个时钟到片选拉高之间的延时，单位ns
    lpspi3_config.betweenTransferDelayInNanoSec=10;     //两次传输之间的延时，单位ns
    LPSPI_MasterInit(LPSPI3,&lpspi3_config,lpspiclk);
    LPSPI_Enable(LPSPI3,true);                          //使能LPSPI3
}


//LPSPI3 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
u8 LPSPI3_ReadWriteByte(u8 TxData)
{ 
    u8 spirxdata=0;
    u8 spitxdata=TxData;
    lpspi_transfer_t spi_tranxfer;
    
    spi_tranxfer.configFlags=kLPSPI_MasterPcs1|kLPSPI_MasterPcsContinuous;     //PCS1
    spi_tranxfer.txData=&spitxdata;                 //要发送的数据
    spi_tranxfer.rxData=&spirxdata;                 //接收到的数据
    spi_tranxfer.dataSize=1;                        //数据长度
    LPSPI_MasterTransferBlocking(LPSPI3,&spi_tranxfer);        //SPI阻塞发送                           
    return spirxdata;
}

