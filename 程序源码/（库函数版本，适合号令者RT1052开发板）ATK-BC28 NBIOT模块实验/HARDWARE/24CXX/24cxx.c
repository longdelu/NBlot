#include "24cxx.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////     
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK RT1052������
//24CXX��������       
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2018/3/7
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved                                      
//////////////////////////////////////////////////////////////////////////////////     

lpi2c_master_transfer_t at24cxx_xfer; //����ṹ��

//��ʼ��IIC�ӿ�
void AT24CXX_Init(void)
{
    LPI2C1_Init(400000);  //��ʼ��LPI2C1
}

//��AT24CXXָ����ַ����һ������
//ReadAddr:��ʼ�����ĵ�ַ  
//val:��ȡ��������
//����ֵ:0 ��ȡ�ɹ�������ֵ����ȡʧ��
u8 AT24CXX_ReadOneByte(u16 ReadAddr,u8 *val)
{                  
    u8 status=0;

    //����I2C xfer�ṹ��
    at24cxx_xfer.slaveAddress=AT24C02_ADDR;     //�豸��ַ
    at24cxx_xfer.direction=kLPI2C_Read;         //������
    at24cxx_xfer.subaddress=(u32)ReadAddr;      //Ҫ��ȡ�ļĴ�����ַ
    at24cxx_xfer.subaddressSize=1;              //�ӵ�ַ����
    at24cxx_xfer.data=val;                      //���ݻ�����
    at24cxx_xfer.dataSize=1;                    //Ҫ��ȡ�����ݳ���
    at24cxx_xfer.flags=kLPI2C_TransferDefaultFlag;

    if(LPI2C_MasterTransferBlocking(LPI2C1,&at24cxx_xfer)==kStatus_Fail)
        status=1;
    return status;
}

//��AT24CXXָ����ַд��һ������
//WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ    
//DataToWrite:Ҫд�������
//����ֵ:0 д��ɹ�������ֵ��д��ʧ��
u8 AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{    
    u8 status=0;
    u8 writedata=0;
    
    writedata=DataToWrite;
    
    //����I2C xfer�ṹ��
    at24cxx_xfer.slaveAddress=AT24C02_ADDR;     //�豸��ַ
    at24cxx_xfer.direction=kLPI2C_Write;        //д������
    at24cxx_xfer.subaddress=(u32)WriteAddr;     //Ҫ��ȡ�ļĴ�����ַ
    at24cxx_xfer.subaddressSize=1;              //��ַ����һ���ֽ�
    at24cxx_xfer.data=&writedata;               //Ҫд�������
    at24cxx_xfer.dataSize=1;                    //д�����ݳ���1���ֽ�
    at24cxx_xfer.flags=kLPI2C_TransferDefaultFlag;
    
    if(LPI2C_MasterTransferBlocking(LPI2C1,&at24cxx_xfer)==kStatus_Fail)
        status=1;
    
    delay_ms(10);      //AT24C02д�������5ms����������Ҫ����ʱ
    return status;     
}

//��AT24CXX�����ָ����ַ��ʼд�볤��ΪLen������
//�ú�������д��16bit����32bit������.
//WriteAddr  :��ʼд��ĵ�ַ  
//DataToWrite:���������׵�ַ
//Len        :Ҫд�����ݵĳ���2,4
void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len)
{      
    u8 t;
    for(t=0;t<Len;t++)
    {
        AT24CXX_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
    }                                                    
}

//��AT24CXX�����ָ����ַ��ʼ��������ΪLen������
//�ú������ڶ���16bit����32bit������.
//ReadAddr   :��ʼ�����ĵ�ַ 
//����ֵ     :����
//Len        :Ҫ�������ݵĳ���2,4
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

//���AT24CXX�Ƿ�����
//��������24XX�����һ����ַ(255)���洢��־��.
//���������24Cϵ��,�����ַҪ�޸�
//����1:���ʧ��
//����0:���ɹ�
u8 AT24CXX_Check(void)
{
    u8 temp;
    AT24CXX_ReadOneByte(255,&temp);//����ÿ�ο�����дAT24CXX               
    if(temp==0X55)return 0;           
    else//�ų���һ�γ�ʼ�������
    {
        AT24CXX_WriteOneByte(255,0X55);
        AT24CXX_ReadOneByte(255,&temp);      
        if(temp==0X55)return 0;
    }
    return 1;                                              
}

//��AT24CXX�����ָ����ַ��ʼ����ָ������������
//ReadAddr :��ʼ�����ĵ�ַ ��24c02Ϊ0~255
//pBuffer  :���������׵�ַ
//NumToRead:Ҫ�������ݵĸ���
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

//��AT24CXX�����ָ����ַ��ʼд��ָ������������
//WriteAddr :��ʼд��ĵ�ַ ��24c02Ϊ0~255
//pBuffer   :���������׵�ַ
//NumToWrite:Ҫд�����ݵĸ���
void AT24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite)
{
    while(NumToWrite--)
    {
        AT24CXX_WriteOneByte(WriteAddr,*pBuffer);
        WriteAddr++;
        pBuffer++;
    }
}
