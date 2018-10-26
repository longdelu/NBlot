#include "lpi2c.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////     
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK I.MX RT1052������
//I2C��������       
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2018/1/9
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved                                      
//////////////////////////////////////////////////////////////////////////////////     

lpi2c_master_config_t   lpi2c1_config;              //I2C1(����)���ýṹ��
lpi2c_master_handle_t   lpi2c1_handle;              //I2C1(����)���

volatile bool lpi2c1_compflag = false;       //��ɱ�־
volatile bool lpi2c1_nakflag = false;        //nak��־

//LPI2C1��ɻص�����
void LPI2C1_Mastercallback(LPI2C_Type *base, lpi2c_master_handle_t *handle, status_t status, void *userData)
{
    //���ݴ������
    if(status==kStatus_Success)
        lpi2c1_compflag=true;
    if (status==kStatus_LPI2C_Nak)
        lpi2c1_nakflag=true;
}

//��ʼ��I2C
//baudrate:IIC������
void LPI2C1_Init(u32 baudrate)
{
    u32 i2cclk=0;
    
    //����I2C��ʱ��ԴΪUSB1(PLL3)=480/8Mhz=60Mhz
    CLOCK_SetMux(kCLOCK_Lpi2cMux,0);   //LPI2Cʱ��ԴΪPLL3=60Mhz
    CLOCK_SetDiv(kCLOCK_Lpi2cDiv,5);   //��Ƶ���ã���ƵֵΪ5+1������LPI2C CLK=10Mhz
    
    //����IO��ΪI2C
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_00_LPI2C1_SCL,1); 
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_01_LPI2C1_SDA,1); 
    
    //����I2C���Ź��ܣ�
    //��ת���ٶ�,��������ΪR0/6,�ٶ�Ϊ100Mhz����·�����ʹ��pull/keepr
    //ѡ��keeper���ܣ�����22K Ohm���ر�Hyst
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_00_LPI2C1_SCL,0xD8B0); 
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_01_LPI2C1_SDA,0xD8B0); 
    
    i2cclk=((CLOCK_GetFreq(kCLOCK_Usb1PllClk)/8)/(5+1)); 
    
    //����I2C1��ΪI2C����
    LPI2C_MasterGetDefaultConfig(&lpi2c1_config);       //������ΪĬ������
    lpi2c1_config.baudRate_Hz=baudrate;                 //����I2C����
    LPI2C_MasterInit(LPI2C1,&lpi2c1_config,i2cclk);     //��ʼ��I2C1 
   
    /****************���ʹ��IIC����������Ļ���Ҫ����handler*************************
    LPI2C_MasterTransferCreateHandle(LPI2C1,&lpi2c1_handle,LPI2C1_Mastercallback, NULL);
    **********************************************************************************/
}

