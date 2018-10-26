#include "key.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////     
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK I.MX RT1052������
//������������       
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2012/12/15
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved                                      
//////////////////////////////////////////////////////////////////////////////////     

//������ʼ������
//KEY_UP    ->  WAKEUP(GPIO5_IO00)
//KEY0      ->  GPIO_AD_B0_05(GPIO1_IO05)
//KEY1      ->  PMIC_ON_REQ(GPIO5_IO01)
//KEY2      ->  GPIO_EMC_40(GPIO3_IO26)
void KEY_Init(void)
{
     gpio_pin_config_t key_config;
    
    //IO��������
    IOMUXC_SetPinMux(IOMUXC_SNVS_WAKEUP_GPIO5_IO00,0);            //SNVS_WAKEUP����ΪALT5,��GPIO5_00
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_05_GPIO1_IO05,0);        //GPIO_AD_B0_05����ΪALT5,��GPIO1_IO05
    IOMUXC_SetPinMux(IOMUXC_SNVS_PMIC_ON_REQ_GPIO5_IO01,0);     //PMIC_ON_REQ,��GPIO5_IO01
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_40_GPIO3_IO26,0);            //GPIO_EMC_40����ΪALT5,��GPIO3_IO26
    
    //����KEY���IO�Ĺ���
    //��ת���ٶ�,�ر��������,�ٶ�Ϊ100Mhz���رտ�·���ܣ�ʹ��pull/keepr
    //ѡ��pull���ܣ�����22K Ohm���ر�Hyst
    IOMUXC_SetPinConfig(IOMUXC_SNVS_WAKEUP_GPIO5_IO00,0xF080);      
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_05_GPIO1_IO05,0xF080);    
    IOMUXC_SetPinConfig(IOMUXC_SNVS_PMIC_ON_REQ_GPIO5_IO01,0xF080);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_40_GPIO3_IO26,0xF080);

    //KEY_UPĬ�ϵ�ƽ����
    key_config.direction=kGPIO_DigitalInput;    //����
    key_config.interruptMode=kGPIO_NoIntmode;    //��ʹ���жϹ���
    key_config.outputLogic=1;                    //Ĭ�ϸߵ�ƽ
    GPIO_PinInit(GPIO5,0,&key_config);             //��ʼ��GPIO5_00 
    
    //KEY0Ĭ�ϵ�ƽ����
    key_config.direction=kGPIO_DigitalInput;    //����
    key_config.interruptMode=kGPIO_NoIntmode;    //��ʹ���жϹ���
    key_config.outputLogic=1;                    //Ĭ�ϸߵ�ƽ
    GPIO_PinInit(GPIO1,5,&key_config);             //��ʼ��GPIO1_05
    
    //KEY1Ĭ�ϵ�ƽ����
    key_config.direction=kGPIO_DigitalInput;    //����
    key_config.interruptMode=kGPIO_NoIntmode;    //��ʹ���жϹ���
    key_config.outputLogic=1;                    //Ĭ�ϸߵ�ƽ
    GPIO_PinInit(GPIO5,1,&key_config);             //��ʼ��GPIO5_01
    
    //KEY2Ĭ�ϵ�ƽ����
    key_config.direction=kGPIO_DigitalInput;    //����
    key_config.interruptMode=kGPIO_NoIntmode;    //��ʹ���жϹ���
    key_config.outputLogic=1;                    //Ĭ�ϸߵ�ƽ
    GPIO_PinInit(GPIO3,26,&key_config);         //��ʼ��GPIO3_26 
}

//����������
//���ذ���ֵ
//mode:0,��֧��������;1,֧��������;
//0��û���κΰ�������
//1��WKUP���� WK_UP
//ע��˺�������Ӧ���ȼ�,KEY0>KEY1>KEY2>WK_UP!!
u8 KEY_Scan(u8 mode)
{
    static u8 key_up=1;     //�����ɿ���־
    if(mode==1)key_up=1;    //֧������
    if(key_up&&(KEY1==0||KEY2==0||KEY0==0||WK_UP==0))
    {
        delay_ms(10);
        key_up=0;
        if(KEY0==0)       return KEY0_PRES;
        else if(KEY1==0)  return KEY1_PRES;
        else if(KEY2==0)  return KEY2_PRES;
        else if(WK_UP==0) return WKUP_PRES;      
    }else if(KEY1==1&&KEY2==1&&KEY0==1&&WK_UP==1)key_up=1;
    return 0;   //�ް�������
}
