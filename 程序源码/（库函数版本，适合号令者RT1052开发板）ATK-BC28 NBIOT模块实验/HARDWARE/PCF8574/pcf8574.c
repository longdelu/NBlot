#include "pcf8574.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////     
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F429������
//PCF8574��������       
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/1/13
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved                                      
//////////////////////////////////////////////////////////////////////////////////     

lpi2c_master_transfer_t pcf8574_xfer; //����ṹ��

//��ʼ��PCF8574
u8 PCF8574_Init(void)
{
    u8 temp=0;
    u8 data=0XFF;
    status_t status=kStatus_Success;
    
    gpio_pin_config_t iicint_config;
    
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_00_GPIO3_IO00,0);  
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_00_GPIO3_IO00,0xF080); 
    
    //IIC_INTĬ�ϵ�ƽ����
    iicint_config.direction=kGPIO_DigitalInput;        //����
    iicint_config.interruptMode=kGPIO_NoIntmode;    //��ʹ���жϹ���
    iicint_config.outputLogic=1;                    //Ĭ�ϸߵ�ƽ
    GPIO_PinInit(GPIO3,0,&iicint_config);             //��ʼ��GPIO3_00 
    
    LPI2C1_Init(10000);                                //IIC��ʼ��
    
    status=LPI2C_MasterStart(LPI2C1,PCF8574_ADDR,kLPI2C_Write);
    status=LPI2C_MasterSend(LPI2C1,&data,1);               //Ĭ�����������IO����ߵ�ƽ
    status=LPI2C_MasterStop(LPI2C1);   
    if(status==kStatus_Fail) 
        temp=1;
    return temp;
}

//��ȡPCF8574��8λIOֵ
//����ֵ:����������
u8 PCF8574_ReadOneByte(void)
{                  
    u8 val=0;
    
    LPI2C_MasterStart(LPI2C1,PCF8574_ADDR,kLPI2C_Read);
    LPI2C_MasterReceive(LPI2C1,&val,1);
    LPI2C_MasterStop(LPI2C1);

    return val;
}

//��PCF8574д��8λIOֵ  
//DataToWrite:Ҫд�������
void PCF8574_WriteOneByte(u8 DataToWrite)
{    
    u8 data=DataToWrite;
    
    LPI2C_MasterStart(LPI2C1,PCF8574_ADDR,kLPI2C_Write);
    LPI2C_MasterSend(LPI2C1,&data,1);
    LPI2C_MasterStop(LPI2C1);
    
    delay_ms(10);     
}

//����PCF8574ĳ��IO�ĸߵ͵�ƽ
//bit:Ҫ���õ�IO���,0~7
//sta:IO��״̬;0��1
void PCF8574_WriteBit(u8 bit,u8 sta)
{
    u8 data;
    data=PCF8574_ReadOneByte(); //�ȶ���ԭ��������
    if(sta==0)data&=~(1<<bit);     
    else data|=1<<bit;
    PCF8574_WriteOneByte(data); //д���µ�����
}

//��ȡPCF8574��ĳ��IO��ֵ
//bit��Ҫ��ȡ��IO���,0~7
//����ֵ:��IO��ֵ,0��1
u8 PCF8574_ReadBit(u8 bit)
{
    u8 data;
    data=PCF8574_ReadOneByte(); //�ȶ�ȡ���8λIO��ֵ 
    if(data&(1<<bit))return 1;
    else return 0;   
}  
    
