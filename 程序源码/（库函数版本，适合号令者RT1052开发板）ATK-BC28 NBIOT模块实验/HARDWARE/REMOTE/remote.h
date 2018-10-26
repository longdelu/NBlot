#ifndef __REMOTE_H
#define __REMOTE_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK ������RT1052������
//����ң�ؽ�����������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2018/5/12
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

#define RDATA   GPIO_PinRead(GPIO3,27) //�������������

//����ң��ʶ����(ID),ÿ��ң�����ĸ�ֵ��������һ��,��Ҳ��һ����.
//����ѡ�õ�ң����ʶ����Ϊ0
#define REMOTE_ID 0    

//GPIO_EMC_41��Ϊ��ʱ������
#define REMOTE_IO_GPT()                                             \
{                                                                   \
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_41_GPT2_CAPTURE1,0);           \
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_41_GPT2_CAPTURE1,0xF0F1);   \
}

//SDA GPIO_EMC_41��ΪGPIO
#define REMOTE_IO_GPIO()                                            \
{                                                                   \
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_41_GPIO3_IO27,0);              \
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_41_GPIO3_IO27,0xF080);      \
    gpio_pin_config_t datain_config;                                \
    datain_config.direction=kGPIO_DigitalInput;                     \
    datain_config.interruptMode=kGPIO_NoIntmode;                    \
    GPIO_PinInit(GPIO3,27,&datain_config);                          \
}


extern u8 RmtCnt;	        //�������µĴ���

void Remote_Init(void);     //���⴫��������ͷ���ų�ʼ��
u8 Remote_Scan(void);
#endif
