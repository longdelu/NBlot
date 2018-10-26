#include "lpspi.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////     
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK I.MX RT1052������
//LPSPI��������       
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2018/3/7
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved                                      
//////////////////////////////////////////////////////////////////////////////////     

/******************************************************  
  SPI3��Ϊ���豸MASTER                            
  LPSPI3_SCK(GPIO_AD_B0_00)     
  LPSPI3_SDI(GPIO_AD_B0_02)    
  LPSPI3_SDO(GPIO_AD_B0_01)  
******************************************************/
lpspi_master_config_t lpspi3_config; //LPSPI3�������ýṹ��

//��ʼ��SPI3
//baudrate:�����ʣ���λMhz��SPI���120M������
void LPSPI3_Init(u8 baudrate)
{
    u32 lpspiclk=0;
    u32 spi3_baudrate=baudrate*1000000;
    
    //����LPSPI��ʱ��ԴΪPLL3_PFD0/(1+5)=576Mhz/(1+5)=96MHz,W25Q256ʱ�����104MHz
    CLOCK_SetMux(kCLOCK_LpspiMux,1);    //USB1 PLL PFD0(576MHz)��ΪLPSPIʱ��Դ
    CLOCK_SetDiv(kCLOCK_LpspiDiv,5);    //��Ƶ���ã���ƵֵΪ5+1������LPSPI CLK=576/(1+5)=96Mhz
    
    //����IO��ΪLPSPI3��Ƭѡ��������ģ�����һ��SPI������Ͳ�������Ϊ4��!
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_00_LPSPI3_SCK,0);                                    
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_01_LPSPI3_SDO,0);                                    
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_02_LPSPI3_SDI,0);                                    
    
    //����LPSPI���Ź��ܣ�
    //��ת���ٶ�,��������ΪR0/6,�ٶ�Ϊ100Mhz����·�����ʹ��pull/keepr
    //ѡ��keeper���ܣ�����100K Ohm���ر�Hyst
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_00_LPSPI3_SCK,0x10B0);                              
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_01_LPSPI3_SDO,0x10B0);                                                                              
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_02_LPSPI3_SDI,0x10B0);                                
    
    //����LPSPI3��ģʽ
    lpspiclk=(CLOCK_GetFreq(kCLOCK_Usb1PllPfd0Clk)/(5+1));//LPSPI3ʱ��
    lpspi3_config.baudRate=spi3_baudrate;               //SPI�ٶ�
    lpspi3_config.whichPcs=kLPSPI_Pcs1;                 //Ƭѡ�źţ�PCS1
    lpspi3_config.pcsActiveHighOrLow=kLPSPI_PcsActiveLow;//Ƭѡ�źŵ͵�ƽ��Ч
    lpspi3_config.bitsPerFrame=8;                       //����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
    lpspi3_config.cpol=kLPSPI_ClockPolarityActiveLow;   //����ͬ��ʱ�ӵ͵�ƽ��Ч
    lpspi3_config.cpha=kLPSPI_ClockPhaseSecondEdge;      //����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
    lpspi3_config.direction=kLPSPI_MsbFirst;            //ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
    lpspi3_config.pinCfg=kLPSPI_SdiInSdoOut;            //SDI�������ţ�SDO�������
    lpspi3_config.dataOutConfig=kLpspiDataOutRetained;  //������ݱ���
    lpspi3_config.pcsToSckDelayInNanoSec=10;            //Ƭѡ���͵�ʱ����Ч֮�����ʱʱ��,��λns
    lpspi3_config.lastSckToPcsDelayInNanoSec=10;        //���һ��ʱ�ӵ�Ƭѡ����֮�����ʱ����λns
    lpspi3_config.betweenTransferDelayInNanoSec=10;     //���δ���֮�����ʱ����λns
    LPSPI_MasterInit(LPSPI3,&lpspi3_config,lpspiclk);
    LPSPI_Enable(LPSPI3,true);                          //ʹ��LPSPI3
}


//LPSPI3 ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
u8 LPSPI3_ReadWriteByte(u8 TxData)
{ 
    u8 spirxdata=0;
    u8 spitxdata=TxData;
    lpspi_transfer_t spi_tranxfer;
    
    spi_tranxfer.configFlags=kLPSPI_MasterPcs1|kLPSPI_MasterPcsContinuous;     //PCS1
    spi_tranxfer.txData=&spitxdata;                 //Ҫ���͵�����
    spi_tranxfer.rxData=&spirxdata;                 //���յ�������
    spi_tranxfer.dataSize=1;                        //���ݳ���
    LPSPI_MasterTransferBlocking(LPSPI3,&spi_tranxfer);        //SPI��������                           
    return spirxdata;
}

