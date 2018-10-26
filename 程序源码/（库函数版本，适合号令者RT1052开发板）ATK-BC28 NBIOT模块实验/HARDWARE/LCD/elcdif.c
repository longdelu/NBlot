#include "elcdif.h"
#include "lpuart.h"
#include "delay.h"
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

//根据不同的颜色格式,定义帧缓存数组，一定要双字对其，也就是64位对其！！！
#if (LCD_PIXFORMAT==LCD_PIXEL_FORMAT_XRGB8888)||(LCD_PIXFORMAT==LCD_PIXEL_FORMAT_RGB888)
__align(64)    u32 elcdif_lcd_framebuf[1366][768] __attribute__((at(LCD_FRAME_BUF_ADDR)));    //定义最大屏分辨率时,LCD所需的帧缓存数组大小
#else
__align(64)    u16 elcdif_lcd_framebuf[1366][768] __attribute__((at(LCD_FRAME_BUF_ADDR)));    //定义最大屏分辨率时,LCD所需的帧缓存数组大小
#endif

_elcdif_dev lcdelcdif;                    //管理LCD LTDC的重要参数

//设置LCD显示方向
//dir:0,竖屏；1,横屏
void ELCDIF_Display_Dir(u8 dir)
{
    lcdelcdif.dir=dir;     //显示方向
    if(dir==0)            //竖屏
    {
        lcdelcdif.width=lcdelcdif.pheight;
        lcdelcdif.height=lcdelcdif.pwidth;    
    }else if(dir==1)    //横屏
    {
        lcdelcdif.width=lcdelcdif.pwidth;
        lcdelcdif.height=lcdelcdif.pheight;
    }
}

//打开LCD开关
//lcd_switch:1 打开,0，关闭
void ELCDIF_Switch(u8 sw)
{
    if(sw==1) 
    {
    
    }
    else if(sw==0)
    {
    
    }
}

//读取面板参数
//GPIO_B1_03=R7(M0);GPIO_B0_14=G7(M1);GPIO_B0_08=B7(M2);
//M2:M1:M0
//0 :0 :0    //4.3寸480*272 RGB屏,ID=0X4342
//0 :0 :1    //7寸800*480 RGB屏,ID=0X7084
//0 :1 :0    //7寸1024*600 RGB屏,ID=0X7016
//0 :1 :1    //7寸1280*800 RGB屏,ID=0X7018
//1 :0 :0    //8寸1024*768 RGB屏,ID=0X8017 
//返回值:LCD ID:0,非法;其他值,ID;
u16 ELCDIF_PanelID_Read(void)
{
    u8 idx=0;
    
    gpio_pin_config_t idpiin_config;
    
    //IO功能设置
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_01_GPIO2_IO01,0);   //GPIO_B1_01配置为ALT5,即GPIO2_1
    IOMUXC_SetPinMux(IOMUXC_GPIO_B1_03_GPIO2_IO19,0);    //GPIO_B1_03配置为ALT5,即GPIO2_19
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_14_GPIO2_IO14,0);    //GPIO_B0_14配置为ALT5,即GPIO2_14
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_08_GPIO2_IO08,0);    //GPIO_B0_08配置为ALT5,即GPIO2_08
    
    //配置GPIO2_18,GPIO2_IO13和GPIO2_IO08的功能
    //低转换速度,关闭输出驱动,速度为100Mhz，关闭开路功能，使能pull/keepr
    //选择pull功能，上拉22K Ohm，关闭Hyst
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_01_GPIO2_IO01,0x10B0);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_03_GPIO2_IO19,0xF080);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_14_GPIO2_IO14,0xF080);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_08_GPIO2_IO08,0xF080);

    idpiin_config.direction=kGPIO_DigitalInput; //输入
    GPIO_PinInit(GPIO2,19,&idpiin_config);         //初始化GPIO2_18  
    GPIO_PinInit(GPIO2,14,&idpiin_config);         //初始化GPIO2_13  
    GPIO_PinInit(GPIO2,8,&idpiin_config);       //初始化GPIO2_08 
    
    idpiin_config.direction=kGPIO_DigitalOutput;    //输出
    idpiin_config.interruptMode=kGPIO_NoIntmode;    //不使用中断功能
    idpiin_config.outputLogic=1;                    
    GPIO_PinInit(GPIO2,1,&idpiin_config);             //初始化GPIO2_1
    GPIO_PinWrite(GPIO2,1,1);                       //GPIO2_IO01(LCD_DE)输出1,使能SGM3157选择NO通道,从而可以正常读取RGBLCD的ID
   
    idx=(u8)GPIO_PinRead(GPIO2,19);                 //读取M0
    idx|=(u8)GPIO_PinRead(GPIO2,14)<<1;             //读取M1
    idx|=(u8)GPIO_PinRead(GPIO2,8)<<2;              //读取M2
    switch(idx)
    {
        case 0:return 0X4342;                        //4.3寸屏,480*272分辨率
        case 1:return 0X7084;                        //7寸屏,800*480分辨率
        case 2:return 0X7016;                        //7寸屏,1024*600分辨率
        case 3:return 0X7018;                        //7寸屏,1280*800分辨率
        case 4:return 0X8016;                         //8寸屏,1024*600分辨率
        case 5:return 0X1018;                         //10.1寸屏,1280*800分辨率
        default:return 0;
    }
}


//LCD时钟设置,LCD CLK=24Mhz*loopdiv/postdiv/lcdif1prediv/lcdif1div
//如要设置LCD CLK=9.3Mhz，那么LCD CLK=24*31/8/5/2=9.3Mhz
//loopdiv: PLL5(VIDEO PLL)的loop Divider，可选范围27~54
//postdiv: PLL5(VIDEO PLL)的post Divider，可选范围1，2，4，8
//lcdif1prediv：LCDIF PRE分频值，可选范围1~8
//lcdif1div：   LCDIF 分频值，可选范围1~8
void ELCDIF_ClockInit(u8 loopdiv,u8 postdiv,u8 lcdif1prediv,u8 lcdif1div)
{
    clock_video_pll_config_t video_config;
    
    video_config.src=kCLOCK_PllClkSrc24M;  
    video_config.loopDivider=loopdiv;
    video_config.postDivider=postdiv;
    video_config.numerator=0;
    video_config.denominator=0;

    //配置Video PLL(PLL5)
    CLOCK_InitVideoPll(&video_config);
    
    //设置eLCDIF Pre时钟源，这里选择PLL5(Video PLL)作为eLCDIF时钟源，可选择范围：
    //  0     PLL2
    //  1     PLL3 PFD3
    //  2     PLL5
    //  3     PLL2 PFD0
    //  4     PLL2 PFD1
    //  5     PLL3 PFD1
    CLOCK_SetMux(kCLOCK_LcdifPreMux,2);
    CLOCK_SetDiv(kCLOCK_LcdifPreDiv,lcdif1prediv-1);   //LCDIF_PRE分频值设置,可设置值:0~7，对应为1~8分频      
    CLOCK_SetDiv(kCLOCK_LcdifDiv,lcdif1div-1);         //LCDIF分频值设置，可设置值:0~7,对应为1~8分频， 
}

//初始化LCD
void ELCDIF_Init(void)
{
    u16 lcdid=0;
    elcdif_rgb_mode_config_t elcdif_config; //eLCDIF配置结构体 
    
    //先获取LCD ID!LCD ID引脚后面会被重新初始化为eLCDIF引脚
    lcdid=ELCDIF_PanelID_Read();
    ELCDIF_PinInit();                   //初始化LCD引脚
    
    if(lcdid==0X4342)
    {
        lcdelcdif.pwidth=480;            //面板宽度,单位:像素
        lcdelcdif.pheight=272;            //面板高度,单位:像素
        lcdelcdif.hsw=1;                //水平同步宽度
        lcdelcdif.vsw=1;                //垂直同步宽度
        lcdelcdif.hbp=40;                //水平后廊
        lcdelcdif.vbp=8;                //垂直后廊
        lcdelcdif.hfp=5;                //水平前廊
        lcdelcdif.vfp=8;                //垂直前廊
        ELCDIF_ClockInit(31,8,5,2);     //设置LCD时钟为9.3Mhz
        //其他参数待定.
    }else if(lcdid==0X7084)
    {
        lcdelcdif.pwidth=800;            //面板宽度,单位:像素
        lcdelcdif.pheight=480;            //面板高度,单位:像素
        lcdelcdif.hsw=1;                //水平同步宽度
        lcdelcdif.vsw=1;                //垂直同步宽度
        lcdelcdif.hbp=46;                //水平后廊
        lcdelcdif.vbp=23;                //垂直后廊
        lcdelcdif.hfp=210;                //水平前廊
        lcdelcdif.vfp=22;                //垂直前廊
        ELCDIF_ClockInit(33,4,6,1);     //设置LCD时钟为33Mhz
    }else if(lcdid==0X7016)        
    {
        lcdelcdif.pwidth=1024;            //面板宽度,单位:像素
        lcdelcdif.pheight=600;            //面板高度,单位:像素
        lcdelcdif.hsw=20;                //水平同步宽度
        lcdelcdif.vsw=3;                //垂直同步宽度
        lcdelcdif.hbp=140;                //水平后廊
        lcdelcdif.vbp=20;                //垂直后廊
        lcdelcdif.hfp=160;                //水平前廊
        lcdelcdif.vfp=12;                //垂直前廊
        ELCDIF_ClockInit(30,4,4,1);     //设置LCD时钟为45Mhz
        //其他参数待定.
    }else if(lcdid==0X7018)        
    {
        lcdelcdif.pwidth=1280;            //面板宽度,单位:像素
        lcdelcdif.pheight=800;            //面板高度,单位:像素
        //其他参数待定.
    }else if(lcdid==0X8017)        
    {
        lcdelcdif.pwidth=1024;            //面板宽度,单位:像素
        lcdelcdif.pheight=768;            //面板高度,单位:像素
        //其他参数待定.
    }else if(lcdid==0X1018)                //10.1寸1280*800 RGB屏    
    {
        lcdelcdif.pwidth=1280;            //面板宽度,单位:像素
        lcdelcdif.pheight=800;            //面板高度,单位:像素
        lcdelcdif.hbp=140;                //水平后廊
        lcdelcdif.hfp=10;                //水平前廊
        lcdelcdif.hsw=10;                //水平同步宽度
        lcdelcdif.vbp=10;                //垂直后廊
        lcdelcdif.vfp=10;                //垂直前廊
        lcdelcdif.vsw=3;                //垂直同步宽度 
        ELCDIF_ClockInit(35,2,5,0);     //设置LCD时钟为70Mhz  
    } 
    
    lcdelcdif.width=lcdelcdif.pwidth;
    lcdelcdif.height=lcdelcdif.pheight;
    
#if LCD_PIXFORMAT==LCD_PIXFORMAT_ARGB8888||LCD_PIXFORMAT==LCD_PIXFORMAT_RGB888 
    lcdelcdif.pixsize=4;                //每个像素占4个字节
#else 
    lcdelcdif.pixsize=2;                //每个像素占2个字节
#endif     
    
    //配置LCD
    elcdif_config.panelWidth=lcdelcdif.pwidth;          //LCD宽度
    elcdif_config.panelHeight=lcdelcdif.pheight;        //LCD高度
    elcdif_config.hsw=lcdelcdif.hsw;
    elcdif_config.hfp=lcdelcdif.hfp;
    elcdif_config.hbp=lcdelcdif.hbp;
    elcdif_config.vsw=lcdelcdif.vsw;
    elcdif_config.vfp=lcdelcdif.vfp;
    elcdif_config.vbp=lcdelcdif.vbp;

    //设置信号极性
    elcdif_config.polarityFlags=kELCDIF_DataEnableActiveHigh|\
                                kELCDIF_VsyncActiveLow|\
                                kELCDIF_HsyncActiveLow|\
                                kELCDIF_DriveDataOnRisingClkEdge;
    elcdif_config.bufferAddr=(uint32_t)elcdif_lcd_framebuf;    //LCD缓冲区首地址
    elcdif_config.pixelFormat=kELCDIF_PixelFormatRGB565;    //输入像素格式
    elcdif_config.dataBus=kELCDIF_DataBus16Bit;             //输出像素格式
    ELCDIF_RgbModeInit(LCDIF,&elcdif_config);               //初始化eLCDIF为RGB模式
    
    ELCDIF_RgbModeStart(LCDIF);                             //开启eLCDIF RGB模式
    ELCDIF_LED(1);                                          //打开LCD背光
    ELCDIF_Display_Dir(0);                                  //竖屏
    ELCDIF_Clear(0XFFFFFFFF);                               //清屏
}

//RGB LCD引脚初始化
void ELCDIF_PinInit(void)
{
    gpio_pin_config_t lcd_config;
    
    //配置RGB LCD所使用的IO的复用功能
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_00_LCD_CLK,0); 
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_01_LCD_ENABLE,0); 
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_02_LCD_HSYNC,0); 
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_03_LCD_VSYNC,0); 
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_04_LCD_DATA00,0); 
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_05_LCD_DATA01,0); 
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_06_LCD_DATA02,0); 
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_07_LCD_DATA03,0); 
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_08_LCD_DATA04,0); 
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_09_LCD_DATA05,0); 
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_10_LCD_DATA06,0); 
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_11_LCD_DATA07,0); 
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_12_LCD_DATA08,0); 
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_13_LCD_DATA09,0); 
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_14_LCD_DATA10,0); 
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_15_LCD_DATA11,0); 
    IOMUXC_SetPinMux(IOMUXC_GPIO_B1_00_LCD_DATA12,0); 
    IOMUXC_SetPinMux(IOMUXC_GPIO_B1_01_LCD_DATA13,0); 
    IOMUXC_SetPinMux(IOMUXC_GPIO_B1_02_LCD_DATA14,0); 
    IOMUXC_SetPinMux(IOMUXC_GPIO_B1_03_LCD_DATA15,0); 
    
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_02_GPIO3_IO02,0);       //LCD背光使能引脚
    
    //配置RGB LCD所使用的IO的详细功能
    //低转换速度,驱动能力为R0/2,速度为100Mhz，关闭开路功能，使能pull/keepr
    //选择pull功能，上拉100K Ohm，是能Hyst
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_00_LCD_CLK,0x1B090);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_01_LCD_ENABLE,0x1B090);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_02_LCD_HSYNC,0x1B090);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_03_LCD_VSYNC,0x1B090);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_04_LCD_DATA00,0x1B090);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_05_LCD_DATA01,0x1B090);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_06_LCD_DATA02,0x1B090);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_07_LCD_DATA03,0x1B090);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_08_LCD_DATA04,0x1B090);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_09_LCD_DATA05,0x1B090);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_10_LCD_DATA06,0x1B090);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_11_LCD_DATA07,0x1B090);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_12_LCD_DATA08,0x1B090);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_13_LCD_DATA09,0x1B090);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_14_LCD_DATA10,0x1B090);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_15_LCD_DATA11,0x1B090);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_00_LCD_DATA12,0x1B090);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_01_LCD_DATA13,0x1B090);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_02_LCD_DATA14,0x1B090);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_03_LCD_DATA15,0x1B090);
    
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_02_GPIO3_IO02,0x10B0);  

    lcd_config.direction=kGPIO_DigitalOutput;    //输出
    lcd_config.interruptMode=kGPIO_NoIntmode;    //不使用中断功能
    lcd_config.outputLogic=0;                    //默认低电平，LCD背光引脚
    GPIO_PinInit(GPIO3,2,&lcd_config);             //初始化GPIO2_31
}

//画点函数
//x,y:坐标
//color:颜色值
inline void ELCDIF_Draw_Point(u16 x,u16 y,u32 color)
{ 
#if LCD_PIXFORMAT==LCD_PIXEL_FORMAT_XRGB8888||LCD_PIXFORMAT==LCD_PIXEL_FORMAT_RGB888
    if(lcdelcdif.dir)    //横屏
    {
        *(u32*)((u32)elcdif_lcd_framebuf+lcdelcdif.pixsize*(lcdelcdif.pwidth*y+x))=color;
    }else             //竖屏
    {
        *(u32*)((u32)elcdif_lcd_framebuf+lcdelcdif.pixsize*(lcdelcdif.pwidth*(lcdelcdif.pheight-x-1)+y))=color; 
    }
#else
    if(lcdelcdif.dir)    //横屏
    {
        *(u16*)((u32)elcdif_lcd_framebuf+lcdelcdif.pixsize*(lcdelcdif.pwidth*y+x))=color;
    }else             //竖屏
    {
        *(u16*)((u32)elcdif_lcd_framebuf+lcdelcdif.pixsize*(lcdelcdif.pwidth*(lcdelcdif.pheight-x-1)+y))=color; 
    }
#endif
}

//读点函数
//返回值:颜色值
inline u32 ELCDIF_Read_Point(u16 x,u16 y)
{ 
#if LCD_PIXFORMAT==LCD_PIXEL_FORMAT_XRGB8888||LCD_PIXFORMAT==LCD_PIXEL_FORMAT_RGB888
    if(lcdelcdif.dir)    //横屏
    {
        return *(u32*)((u32)elcdif_lcd_framebuf+lcdelcdif.pixsize*(lcdelcdif.pwidth*y+x));
    }else             //竖屏
    {
        return *(u32*)((u32)elcdif_lcd_framebuf+lcdelcdif.pixsize*(lcdelcdif.pwidth*(lcdelcdif.pheight-x-1)+y)); 
    }
#else
    if(lcdelcdif.dir)    //横屏
    {
        return *(u16*)((u32)elcdif_lcd_framebuf+lcdelcdif.pixsize*(lcdelcdif.pwidth*y+x));
    }else             //竖屏
    {
        return *(u16*)((u32)elcdif_lcd_framebuf+lcdelcdif.pixsize*(lcdelcdif.pwidth*(lcdelcdif.pheight-x-1)+y)); 
    }
#endif 
}

//RGBLCD填充矩形
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)   
//注意:sx,ex,不能大于lcddev.width-1;sy,ey,不能大于lcddev.height-1!!!
//color:要填充的颜色
void ELCDIF_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u32 color)
{ 
    u16 x,y;
    
    if(ex>=lcdelcdif.width) ex=lcdelcdif.width-1;
    else if(ey>=lcdelcdif.height) ey=lcdelcdif.height-1;
    
    for(y=sy;y<=ey;y++)
    {
        for(x=sx;x<=ex;x++)
           ELCDIF_Draw_Point(x,y,color); 
    }
}

//在指定区域内填充指定颜色块             
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)   
//color:要填充的颜色
void ELCDIF_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color)
{  
    u32 x,y;
    u32 ylen=ey-sy+1;
    u16 *p=color;
    if(ex>=lcdelcdif.width)ex=lcdelcdif.width-1;
    else if(ey>=lcdelcdif.height)ey=lcdelcdif.height-1; 
    for(y=sy;y<=ey;y++)
    {
        for(x=sx;x<=ex;x++)
        {
            ELCDIF_Draw_Point(x,y,p[x]); 
        }
        p+=ylen;        
    } 
}

//LCD清屏
//color:颜色值
void ELCDIF_Clear(u32 color)
{
    u32 tpval;
    u32 i=0; 
#if (LCD_PIXFORMAT==LCD_PIXEL_FORMAT_RAW8)||(LCD_PIXFORMAT==LCD_PIXEL_FORMAT_RGB565)
    u16 *p=(u16*)elcdif_lcd_framebuf;    //指向帧缓存首地址
#else
    u32 *p=(u32*)elcdif_lcd_framebuf;    //指向帧缓存首地址
#endif 
    tpval=(u32)lcdelcdif.width*lcdelcdif.height;
    for(i=0;i<tpval;i++)
    {
        p[i]=color;
    }
}
