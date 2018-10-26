#include "ap3216c.h"
#include "lpi2c.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////     
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK I.MX RT1052������
//AP3216C��������       
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/1/16
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved                                      
//////////////////////////////////////////////////////////////////////////////////     

lpi2c_master_transfer_t apc3216c_xfer; //����ṹ��

//��ʼ��AP3216C
//����ֵ:0,��ʼ���ɹ�
//      1,��ʼ��ʧ��
u8 AP3216C_Init(void)
{
    u8 temp=0;
    LPI2C1_Init(400000);                //��ʼ��IIC
    AP3216C_WriteOneByte(0x00,0X04);    //��λAP3216C
    delay_ms(50);                        //AP33216C��λ����10ms
    AP3216C_WriteOneByte(0x00,0X03);    //����ALS��PS+IR   
    temp=AP3216C_ReadOneByte(0X00);        //��ȡ�ո�д��ȥ��0X03
    if(temp==0X03)return 0;                //AP3216C����
    else return 1;                        //AP3216Cʧ��
} 

//��ȡAP3216C������
//��ȡԭʼ���ݣ�����ALS,PS��IR
//ע�⣡���ͬʱ��ALS,IR+PS�Ļ��������ݶ�ȡ��ʱ����Ҫ����112.5ms
void AP3216C_ReadData(u16* ir,u16* ps,u16* als)
{
    u8 buf[6];
    u8 i;
    for(i=0;i<6;i++)    
    {
        buf[i]=AP3216C_ReadOneByte(0X0A+i);        //ѭ����ȡ���д���������
    }
    if(buf[0]&0X80)*ir=0;                        //IR_OFλΪ1,��������Ч
    else *ir=((u16)buf[1]<<2)|(buf[0]&0X03);     //��ȡIR������������  
    *als=((u16)buf[3]<<8)|buf[2];                //��ȡALS������������   
    if(buf[4]&0x40)*ps=0;                        //IR_OFλΪ1,��������Ч
    else *ps=((u16)(buf[5]&0X3F)<<4)|(buf[4]&0X0F); //��ȡPS������������   
}

//IICдһ���ֽ�  
//reg:�Ĵ�����ַ
//data:Ҫд�������
//����ֵ:0,����
//    ����,�������
u8 AP3216C_WriteOneByte(u8 reg,u8 data)
{
    u8 status=0;
    u8 writedata=data;
    
    //����I2C xfer�ṹ��
    apc3216c_xfer.slaveAddress=AP3216C_ADDR;    //�豸��ַ
    apc3216c_xfer.direction=kLPI2C_Write;        //д������
    apc3216c_xfer.subaddress=(u32)reg;           //Ҫ��ȡ�ļĴ�����ַ
    apc3216c_xfer.subaddressSize=1;              //��ַ����һ���ֽ�
    apc3216c_xfer.data=&writedata;               //Ҫд�������
    apc3216c_xfer.dataSize=1;                    //д�����ݳ���1���ֽ�
    apc3216c_xfer.flags=kLPI2C_TransferDefaultFlag;

    if(LPI2C_MasterTransferBlocking(LPI2C1,&apc3216c_xfer)==kStatus_Fail)
        status=1;
    return status;
}

//IIC��һ���ֽ� 
//reg:�Ĵ�����ַ 
//����ֵ:����������
u8 AP3216C_ReadOneByte(u8 reg)
{
    u8 val=0;
    
    //����I2C xfer�ṹ��
    apc3216c_xfer.slaveAddress=AP3216C_ADDR;    //�豸��ַ
    apc3216c_xfer.direction=kLPI2C_Read;         //������
    apc3216c_xfer.subaddress=(u32)reg;           //Ҫ��ȡ�ļĴ�����ַ
    apc3216c_xfer.subaddressSize=1;              //�ӵ�ַ����
    apc3216c_xfer.data=&val;                     //���ݻ�����
    apc3216c_xfer.dataSize=1;                    //Ҫ��ȡ�����ݳ���
    apc3216c_xfer.flags=kLPI2C_TransferDefaultFlag;
    LPI2C_MasterTransferBlocking(LPI2C1,&apc3216c_xfer);

    return val;
}

