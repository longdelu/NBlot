#include "atk_rgb_led.h"

//////////////////////////////////////////////////////////////////////////////////     
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//P9813 RGB LED��������            
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2016/10/20
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved                                      
//////////////////////////////////////////////////////////////////////////////////   

//����ǰ32λ'0'��ʼ֡
void RGBLED_Send32Zero(void)
{
     u8 i;
    
     SDA = 0;
    
     for(i=0;i<32;i++)
     {
        SCL = 0;
        delay_us(200);
        SCL = 1;
        delay_us(200);        
     }
     
}

//��תǰ��λ�ĻҶ�ֵ
u8 RGBLED_TakeAntiCode(u8 dat)
{
     u8 tmp = 0;
    
     tmp = ((~dat) & 0xC0) >> 6;
     return tmp;
}

//���ͻҶ�����
void RGBLED_DatSend(u32 dx)
{
     u8 i;
     
     for(i=0;i<32;i++)
     {
        if((dx & 0x80000000) !=0)
         SDA=1;
        else 
         SDA=0;
        
        dx<<=1;
        SCL=0;delay_us(200);
        SCL=1;delay_us(200);
     }    
}

//���ݴ����뷢��
//r;��ɫֵ 0~255
//g:��ɫֵ 0~255
//b:��ɫֵ 0~255  
void RGBLED_DataDealWithAndSend(u8 r,u8 g,u8 b)
{
     
     u32 dx=0;
    
     dx |= (u32)0x03 << 30;  //ǰ��λ'1'λ��־λ
     dx |= (u32)RGBLED_TakeAntiCode(b) << 28;
     dx |= (u32)RGBLED_TakeAntiCode(g) << 26;
     dx |= (u32)RGBLED_TakeAntiCode(r) << 24;
    
     dx |= (u32)b << 16;
     dx |= (u32)g << 8;
     dx |= r;
    
     RGBLED_DatSend(dx);//��������
        
}
//RGB�ƿ��ƺ���
//r;��ɫֵ 0~255
//g:��ɫֵ 0~255
//b:��ɫֵ 0~255
void RGBLED_Show(u8 r,u8 g,u8 b)
{
    RGBLED_Send32Zero();//����ǰ32λ'0'��ʼ֡
    RGBLED_DataDealWithAndSend(r, g, b);//����32bit�Ҷ�����
    RGBLED_DataDealWithAndSend(r, g, b);
}

//RGB�Ƴ�ʼ������
void RGBLED_Init(void) 
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);//ʹ��PORTGʱ��
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8;//GPIO7��GPIO8
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//���ó��������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOG, &GPIO_InitStructure);
    
    RGBLED_Show(0,0,0);//�ر�RGB��
    
}

