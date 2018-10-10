#include "atk_rgb_led.h"

//////////////////////////////////////////////////////////////////////////////////     
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//P9813 RGB LED驱动代码            
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2016/10/20
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved                                      
//////////////////////////////////////////////////////////////////////////////////   

//发送前32位'0'起始帧
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

//反转前两位的灰度值
u8 RGBLED_TakeAntiCode(u8 dat)
{
     u8 tmp = 0;
    
     tmp = ((~dat) & 0xC0) >> 6;
     return tmp;
}

//发送灰度数据
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

//数据处理与发送
//r;红色值 0~255
//g:绿色值 0~255
//b:蓝色值 0~255  
void RGBLED_DataDealWithAndSend(u8 r,u8 g,u8 b)
{
     
     u32 dx=0;
    
     dx |= (u32)0x03 << 30;  //前两位'1'位标志位
     dx |= (u32)RGBLED_TakeAntiCode(b) << 28;
     dx |= (u32)RGBLED_TakeAntiCode(g) << 26;
     dx |= (u32)RGBLED_TakeAntiCode(r) << 24;
    
     dx |= (u32)b << 16;
     dx |= (u32)g << 8;
     dx |= r;
    
     RGBLED_DatSend(dx);//发送数据
        
}
//RGB灯控制函数
//r;红色值 0~255
//g:绿色值 0~255
//b:蓝色值 0~255
void RGBLED_Show(u8 r,u8 g,u8 b)
{
    RGBLED_Send32Zero();//发送前32位'0'起始帧
    RGBLED_DataDealWithAndSend(r, g, b);//发送32bit灰度数据
    RGBLED_DataDealWithAndSend(r, g, b);
}

//RGB灯初始化函数
void RGBLED_Init(void) 
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);//使能PORTG时钟
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8;//GPIO7、GPIO8
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//设置成推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOG, &GPIO_InitStructure);
    
    RGBLED_Show(0,0,0);//关闭RGB灯
    
}

