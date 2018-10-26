#ifndef _RGBLCD_H
#define _RGBLCD_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////     
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK I.MX RT1052������
//RGB LCD��������       
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2018/1/4
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved                                      
//////////////////////////////////////////////////////////////////////////////////     

#define ELCDIF_LED(n)        (n?GPIO_PinWrite(GPIO3,2,1):GPIO_PinWrite(GPIO3,2,0))     //RGB LCD�������

#define LCD_PIXEL_FORMAT_RAW8       0X00    
#define LCD_PIXEL_FORMAT_RGB565     0X01    
#define LCD_PIXEL_FORMAT_RGB666     0X02       
#define LCD_PIXEL_FORMAT_XRGB8888   0X03      
#define LCD_PIXEL_FORMAT_RGB888     0X04     

///////////////////////////////////////////////////////////////////////
//�û��޸����ò���:

//������ɫ���ظ�ʽ,һ����RGB565
#define LCD_PIXFORMAT                LCD_PIXEL_FORMAT_RGB565    

//LCD֡�������׵�ַ,���ﶨ����SDRAM����.
#define LCD_FRAME_BUF_ADDR            0X80200000  

//LCD LTDC��Ҫ������
typedef struct  
{                             
    u32 pwidth;            //LCD���Ŀ��,�̶�����,������ʾ����ı�,���Ϊ0,˵��û���κ�RGB������
    u32 pheight;        //LCD���ĸ߶�,�̶�����,������ʾ����ı�
    u16 hsw;            //ˮƽͬ�����
    u16 vsw;            //��ֱͬ�����
    u16 hbp;            //ˮƽ����
    u16 vbp;            //��ֱ����
    u16 hfp;            //ˮƽǰ��
    u16 vfp;            //��ֱǰ�� 
    u8 activelayer;        //��ǰ����:0/1    
    u8 dir;                //0,����;1,����;
    u16 width;            //LCD���
    u16 height;            //LCD�߶�
    u32 pixsize;        //ÿ��������ռ�ֽ���
}_elcdif_dev; 

extern _elcdif_dev lcdelcdif;    //����LCD LTDC����

void ELCDIF_Init(void);
void ELCDIF_PinInit(void);
void ELCDIF_Display_Dir(u8 dir);
u16 ELCDIF_PanelID_Read(void);
void ELCDIF_ClockInit(u8 loopdiv,u8 postdiv,u8 lcdif1prediv,u8 lcdif1div);
void ELCDIF_Draw_Point(u16 x,u16 y,u32 color);
u32 ELCDIF_Read_Point(u16 x,u16 y);
void ELCDIF_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u32 color);
void ELCDIF_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color);
void ELCDIF_Clear(u32 color);
void ELCDIF_Switch(u8 sw);
#endif

