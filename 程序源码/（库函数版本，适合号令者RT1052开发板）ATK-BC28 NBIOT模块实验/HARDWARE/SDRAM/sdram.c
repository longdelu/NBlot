#include "sdram.h"
#include "lpuart.h"
//////////////////////////////////////////////////////////////////////////////////     
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK I.MX RT1052开发板
//SDRAM驱动代码       
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2018/1/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved                                      
//////////////////////////////////////////////////////////////////////////////////     

semc_config_t       semc_configure;     //semc配置结构体
semc_sdram_config_t sdram_config;       //SDRAM配置结构体

//初始化SDRAM
void SDRAM_Init(void)
{
    u32 semcclock=0;

    //配置SDRAM所使用的IO
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_00_SEMC_DATA00,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_01_SEMC_DATA01,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_02_SEMC_DATA02,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_03_SEMC_DATA03,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_04_SEMC_DATA04,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_05_SEMC_DATA05,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_06_SEMC_DATA06,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_07_SEMC_DATA07,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_08_SEMC_DM00,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_09_SEMC_ADDR00,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_10_SEMC_ADDR01,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_11_SEMC_ADDR02,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_12_SEMC_ADDR03,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_13_SEMC_ADDR04,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_14_SEMC_ADDR05,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_15_SEMC_ADDR06,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_16_SEMC_ADDR07,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_17_SEMC_ADDR08,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_18_SEMC_ADDR09,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_19_SEMC_ADDR11,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_20_SEMC_ADDR12,0);
    
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_21_SEMC_BA0,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_22_SEMC_BA1,0);
    
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_23_SEMC_ADDR10,0);
    
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_24_SEMC_CAS,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_25_SEMC_RAS,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_26_SEMC_CLK,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_27_SEMC_CKE,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_28_SEMC_WE,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_29_SEMC_CS0,0);
    
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_30_SEMC_DATA08,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_31_SEMC_DATA09,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_32_SEMC_DATA10,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_33_SEMC_DATA11,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_34_SEMC_DATA12,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_35_SEMC_DATA13,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_36_SEMC_DATA14,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_37_SEMC_DATA15,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_38_SEMC_DM01,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_39_SEMC_DQS,1);
  
    //高转换速度,关闭能力R0/7速度为200Mhz，关闭开路功能，使能pull/keepr
    //选择keeper功能，下拉100K Ohm，使能Hyst   
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_00_SEMC_DATA00,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_01_SEMC_DATA01,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_02_SEMC_DATA02,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_03_SEMC_DATA03,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_04_SEMC_DATA04,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_05_SEMC_DATA05,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_06_SEMC_DATA06,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_07_SEMC_DATA07,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_08_SEMC_DM00,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_09_SEMC_ADDR00,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_10_SEMC_ADDR01,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_11_SEMC_ADDR02,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_12_SEMC_ADDR03,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_13_SEMC_ADDR04,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_14_SEMC_ADDR05,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_15_SEMC_ADDR06,0X110F9);   
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_16_SEMC_ADDR07,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_17_SEMC_ADDR08,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_18_SEMC_ADDR09,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_19_SEMC_ADDR11,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_20_SEMC_ADDR12,0X110F9);
    
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_21_SEMC_BA0,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_22_SEMC_BA1,0X110F9);
    
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_23_SEMC_ADDR10,0X110F9);
    
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_24_SEMC_CAS,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_25_SEMC_RAS,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_26_SEMC_CLK,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_27_SEMC_CKE,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_28_SEMC_WE,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_29_SEMC_CS0,0X110F9);
    
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_30_SEMC_DATA08,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_31_SEMC_DATA09,0X110F9);        
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_32_SEMC_DATA10,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_33_SEMC_DATA11,0x110F9); 
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_34_SEMC_DATA12,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_35_SEMC_DATA13,0X110F9);        
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_36_SEMC_DATA14,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_37_SEMC_DATA15,0x110F9); 
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_38_SEMC_DM01,0x110F9);  
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_39_SEMC_DQS,0x110F9); 
    
    //配置SEMC的SDRAM控制器
    SEMC_GetDefaultConfig(&semc_configure);         //先配置为默认值
    semc_configure.dqsMode=kSEMC_Loopbackdqspad;    //DQS使用EMC_39引脚，不用此引脚的话SDRAM速度上不去！
    SEMC_Init(SEMC,&semc_configure);                //初始化SEMC
    
    semcclock=CLOCK_GetFreq(kCLOCK_SemcClk);        //获取SEMC时钟，132Mhz
    sdram_config.csxPinMux=kSEMC_MUXCSX0;           //片选CSX0
    sdram_config.address=SDRAM_START_ADDR;          //SDRAM开始地址
    sdram_config.memsize_kbytes=SDRAM_SIZE_KBYTES;  //SDRAM大小，单位kBytes
    sdram_config.portSize=kSEMC_PortSize16Bit;      //SDRAM位宽，16位
    sdram_config.burstLen=kSEMC_Sdram_BurstLen1;    //突发长度1
    sdram_config.columnAddrBitNum=kSEMC_SdramColunm_9bit;   //列地址9位
    sdram_config.casLatency=kSEMC_LatencyThree;     //CL=3
    sdram_config.tPrecharge2Act_Ns=20;              //tRP 
    sdram_config.tAct2ReadWrite_Ns=20;              //tRCD
    sdram_config.tRefreshRecovery_Ns=67;            //tRFC和tXSR的最大值
    sdram_config.tWriteRecovery_Ns=20;              //tWR
    sdram_config.tCkeOff_Ns=(1000000000/semcclock); //CLK最小关闭时间，也就是CLK周期，单位ns
    sdram_config.tAct2Prechage_Ns=50;               //tRAS
    sdram_config.tSelfRefRecovery_Ns=80;            //tXSR
    sdram_config.tRefresh2Refresh_Ns=100;           //tRFC
    sdram_config.tAct2Act_Ns=70;                    //tRC
    sdram_config.tPrescalePeriod_Ns=160*(1000000000/semcclock);//分频周期,160个时钟周期
    sdram_config.refreshPeriod_nsPerRow=(64*1000000)/8192;//刷新周期，刷新一行需要的时间，单位ns
    sdram_config.refreshUrgThreshold=sdram_config.refreshPeriod_nsPerRow;
    sdram_config.refreshBurstLen=1;
    
    SEMC_ConfigureSDRAM(SEMC,kSEMC_SDRAM_CS0,&sdram_config,semcclock);//配置SDRAM   
}

//在指定地址(WriteAddr+SDRAM_START_ADDR)开始,连续写入n个字节.
//pBuffer:字节指针
//WriteAddr:要写入的地址
//n:要写入的字节数
void SEMC_SDRAM_WriteBuffer(u8 *pBuffer,u32 WriteAddr,u32 n)
{
    for(;n!=0;n--)
    {
        *(vu8*)(SDRAM_START_ADDR+WriteAddr)=*pBuffer;
        WriteAddr++;
        pBuffer++;
    }
}

//在指定地址((WriteAddr+SDRAM_START_ADDR))开始,连续读出n个字节.
//pBuffer:字节指针
//ReadAddr:要读出的起始地址
//n:要写入的字节数
void SEMC_SDRAM_ReadBuffer(u8 *pBuffer,u32 ReadAddr,u32 n)
{
    for(;n!=0;n--)
    {
        *pBuffer++=*(vu8*)(SDRAM_START_ADDR+ReadAddr);
        ReadAddr++;
    }
}

