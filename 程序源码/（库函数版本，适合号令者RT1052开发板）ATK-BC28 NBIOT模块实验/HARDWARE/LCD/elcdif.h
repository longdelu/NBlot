#ifndef _RGBLCD_H
#define _RGBLCD_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////     
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK I.MX RT1052开发板
//RGB LCD驱动代码       
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2018/1/4
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved                                      
//////////////////////////////////////////////////////////////////////////////////     

#define ELCDIF_LED(n)        (n?GPIO_PinWrite(GPIO3,2,1):GPIO_PinWrite(GPIO3,2,0))     //RGB LCD背光控制

#define LCD_PIXEL_FORMAT_RAW8       0X00    
#define LCD_PIXEL_FORMAT_RGB565     0X01    
#define LCD_PIXEL_FORMAT_RGB666     0X02       
#define LCD_PIXEL_FORMAT_XRGB8888   0X03      
#define LCD_PIXEL_FORMAT_RGB888     0X04     

///////////////////////////////////////////////////////////////////////
//用户修改配置部分:

//定义颜色像素格式,一般用RGB565
#define LCD_PIXFORMAT                LCD_PIXEL_FORMAT_RGB565    

//LCD帧缓冲区首地址,这里定义在SDRAM里面.
#define LCD_FRAME_BUF_ADDR            0X80200000  

//LCD LTDC重要参数集
typedef struct  
{                             
    u32 pwidth;            //LCD面板的宽度,固定参数,不随显示方向改变,如果为0,说明没有任何RGB屏接入
    u32 pheight;        //LCD面板的高度,固定参数,不随显示方向改变
    u16 hsw;            //水平同步宽度
    u16 vsw;            //垂直同步宽度
    u16 hbp;            //水平后廊
    u16 vbp;            //垂直后廊
    u16 hfp;            //水平前廊
    u16 vfp;            //垂直前廊 
    u8 activelayer;        //当前层编号:0/1    
    u8 dir;                //0,竖屏;1,横屏;
    u16 width;            //LCD宽度
    u16 height;            //LCD高度
    u32 pixsize;        //每个像素所占字节数
}_elcdif_dev; 

extern _elcdif_dev lcdelcdif;    //管理LCD LTDC参数

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

