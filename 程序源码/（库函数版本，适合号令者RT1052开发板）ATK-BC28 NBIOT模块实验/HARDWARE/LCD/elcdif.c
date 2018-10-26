#include "elcdif.h"
#include "lpuart.h"
#include "delay.h"
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

//���ݲ�ͬ����ɫ��ʽ,����֡�������飬һ��Ҫ˫�ֶ��䣬Ҳ����64λ���䣡����
#if (LCD_PIXFORMAT==LCD_PIXEL_FORMAT_XRGB8888)||(LCD_PIXFORMAT==LCD_PIXEL_FORMAT_RGB888)
__align(64)    u32 elcdif_lcd_framebuf[1366][768] __attribute__((at(LCD_FRAME_BUF_ADDR)));    //����������ֱ���ʱ,LCD�����֡���������С
#else
__align(64)    u16 elcdif_lcd_framebuf[1366][768] __attribute__((at(LCD_FRAME_BUF_ADDR)));    //����������ֱ���ʱ,LCD�����֡���������С
#endif

_elcdif_dev lcdelcdif;                    //����LCD LTDC����Ҫ����

//����LCD��ʾ����
//dir:0,������1,����
void ELCDIF_Display_Dir(u8 dir)
{
    lcdelcdif.dir=dir;     //��ʾ����
    if(dir==0)            //����
    {
        lcdelcdif.width=lcdelcdif.pheight;
        lcdelcdif.height=lcdelcdif.pwidth;    
    }else if(dir==1)    //����
    {
        lcdelcdif.width=lcdelcdif.pwidth;
        lcdelcdif.height=lcdelcdif.pheight;
    }
}

//��LCD����
//lcd_switch:1 ��,0���ر�
void ELCDIF_Switch(u8 sw)
{
    if(sw==1) 
    {
    
    }
    else if(sw==0)
    {
    
    }
}

//��ȡ������
//GPIO_B1_03=R7(M0);GPIO_B0_14=G7(M1);GPIO_B0_08=B7(M2);
//M2:M1:M0
//0 :0 :0    //4.3��480*272 RGB��,ID=0X4342
//0 :0 :1    //7��800*480 RGB��,ID=0X7084
//0 :1 :0    //7��1024*600 RGB��,ID=0X7016
//0 :1 :1    //7��1280*800 RGB��,ID=0X7018
//1 :0 :0    //8��1024*768 RGB��,ID=0X8017 
//����ֵ:LCD ID:0,�Ƿ�;����ֵ,ID;
u16 ELCDIF_PanelID_Read(void)
{
    u8 idx=0;
    
    gpio_pin_config_t idpiin_config;
    
    //IO��������
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_01_GPIO2_IO01,0);   //GPIO_B1_01����ΪALT5,��GPIO2_1
    IOMUXC_SetPinMux(IOMUXC_GPIO_B1_03_GPIO2_IO19,0);    //GPIO_B1_03����ΪALT5,��GPIO2_19
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_14_GPIO2_IO14,0);    //GPIO_B0_14����ΪALT5,��GPIO2_14
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_08_GPIO2_IO08,0);    //GPIO_B0_08����ΪALT5,��GPIO2_08
    
    //����GPIO2_18,GPIO2_IO13��GPIO2_IO08�Ĺ���
    //��ת���ٶ�,�ر��������,�ٶ�Ϊ100Mhz���رտ�·���ܣ�ʹ��pull/keepr
    //ѡ��pull���ܣ�����22K Ohm���ر�Hyst
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_01_GPIO2_IO01,0x10B0);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_03_GPIO2_IO19,0xF080);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_14_GPIO2_IO14,0xF080);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_08_GPIO2_IO08,0xF080);

    idpiin_config.direction=kGPIO_DigitalInput; //����
    GPIO_PinInit(GPIO2,19,&idpiin_config);         //��ʼ��GPIO2_18  
    GPIO_PinInit(GPIO2,14,&idpiin_config);         //��ʼ��GPIO2_13  
    GPIO_PinInit(GPIO2,8,&idpiin_config);       //��ʼ��GPIO2_08 
    
    idpiin_config.direction=kGPIO_DigitalOutput;    //���
    idpiin_config.interruptMode=kGPIO_NoIntmode;    //��ʹ���жϹ���
    idpiin_config.outputLogic=1;                    
    GPIO_PinInit(GPIO2,1,&idpiin_config);             //��ʼ��GPIO2_1
    GPIO_PinWrite(GPIO2,1,1);                       //GPIO2_IO01(LCD_DE)���1,ʹ��SGM3157ѡ��NOͨ��,�Ӷ�����������ȡRGBLCD��ID
   
    idx=(u8)GPIO_PinRead(GPIO2,19);                 //��ȡM0
    idx|=(u8)GPIO_PinRead(GPIO2,14)<<1;             //��ȡM1
    idx|=(u8)GPIO_PinRead(GPIO2,8)<<2;              //��ȡM2
    switch(idx)
    {
        case 0:return 0X4342;                        //4.3����,480*272�ֱ���
        case 1:return 0X7084;                        //7����,800*480�ֱ���
        case 2:return 0X7016;                        //7����,1024*600�ֱ���
        case 3:return 0X7018;                        //7����,1280*800�ֱ���
        case 4:return 0X8016;                         //8����,1024*600�ֱ���
        case 5:return 0X1018;                         //10.1����,1280*800�ֱ���
        default:return 0;
    }
}


//LCDʱ������,LCD CLK=24Mhz*loopdiv/postdiv/lcdif1prediv/lcdif1div
//��Ҫ����LCD CLK=9.3Mhz����ôLCD CLK=24*31/8/5/2=9.3Mhz
//loopdiv: PLL5(VIDEO PLL)��loop Divider����ѡ��Χ27~54
//postdiv: PLL5(VIDEO PLL)��post Divider����ѡ��Χ1��2��4��8
//lcdif1prediv��LCDIF PRE��Ƶֵ����ѡ��Χ1~8
//lcdif1div��   LCDIF ��Ƶֵ����ѡ��Χ1~8
void ELCDIF_ClockInit(u8 loopdiv,u8 postdiv,u8 lcdif1prediv,u8 lcdif1div)
{
    clock_video_pll_config_t video_config;
    
    video_config.src=kCLOCK_PllClkSrc24M;  
    video_config.loopDivider=loopdiv;
    video_config.postDivider=postdiv;
    video_config.numerator=0;
    video_config.denominator=0;

    //����Video PLL(PLL5)
    CLOCK_InitVideoPll(&video_config);
    
    //����eLCDIF Preʱ��Դ������ѡ��PLL5(Video PLL)��ΪeLCDIFʱ��Դ����ѡ��Χ��
    //  0     PLL2
    //  1     PLL3 PFD3
    //  2     PLL5
    //  3     PLL2 PFD0
    //  4     PLL2 PFD1
    //  5     PLL3 PFD1
    CLOCK_SetMux(kCLOCK_LcdifPreMux,2);
    CLOCK_SetDiv(kCLOCK_LcdifPreDiv,lcdif1prediv-1);   //LCDIF_PRE��Ƶֵ����,������ֵ:0~7����ӦΪ1~8��Ƶ      
    CLOCK_SetDiv(kCLOCK_LcdifDiv,lcdif1div-1);         //LCDIF��Ƶֵ���ã�������ֵ:0~7,��ӦΪ1~8��Ƶ�� 
}

//��ʼ��LCD
void ELCDIF_Init(void)
{
    u16 lcdid=0;
    elcdif_rgb_mode_config_t elcdif_config; //eLCDIF���ýṹ�� 
    
    //�Ȼ�ȡLCD ID!LCD ID���ź���ᱻ���³�ʼ��ΪeLCDIF����
    lcdid=ELCDIF_PanelID_Read();
    ELCDIF_PinInit();                   //��ʼ��LCD����
    
    if(lcdid==0X4342)
    {
        lcdelcdif.pwidth=480;            //�����,��λ:����
        lcdelcdif.pheight=272;            //���߶�,��λ:����
        lcdelcdif.hsw=1;                //ˮƽͬ�����
        lcdelcdif.vsw=1;                //��ֱͬ�����
        lcdelcdif.hbp=40;                //ˮƽ����
        lcdelcdif.vbp=8;                //��ֱ����
        lcdelcdif.hfp=5;                //ˮƽǰ��
        lcdelcdif.vfp=8;                //��ֱǰ��
        ELCDIF_ClockInit(31,8,5,2);     //����LCDʱ��Ϊ9.3Mhz
        //������������.
    }else if(lcdid==0X7084)
    {
        lcdelcdif.pwidth=800;            //�����,��λ:����
        lcdelcdif.pheight=480;            //���߶�,��λ:����
        lcdelcdif.hsw=1;                //ˮƽͬ�����
        lcdelcdif.vsw=1;                //��ֱͬ�����
        lcdelcdif.hbp=46;                //ˮƽ����
        lcdelcdif.vbp=23;                //��ֱ����
        lcdelcdif.hfp=210;                //ˮƽǰ��
        lcdelcdif.vfp=22;                //��ֱǰ��
        ELCDIF_ClockInit(33,4,6,1);     //����LCDʱ��Ϊ33Mhz
    }else if(lcdid==0X7016)        
    {
        lcdelcdif.pwidth=1024;            //�����,��λ:����
        lcdelcdif.pheight=600;            //���߶�,��λ:����
        lcdelcdif.hsw=20;                //ˮƽͬ�����
        lcdelcdif.vsw=3;                //��ֱͬ�����
        lcdelcdif.hbp=140;                //ˮƽ����
        lcdelcdif.vbp=20;                //��ֱ����
        lcdelcdif.hfp=160;                //ˮƽǰ��
        lcdelcdif.vfp=12;                //��ֱǰ��
        ELCDIF_ClockInit(30,4,4,1);     //����LCDʱ��Ϊ45Mhz
        //������������.
    }else if(lcdid==0X7018)        
    {
        lcdelcdif.pwidth=1280;            //�����,��λ:����
        lcdelcdif.pheight=800;            //���߶�,��λ:����
        //������������.
    }else if(lcdid==0X8017)        
    {
        lcdelcdif.pwidth=1024;            //�����,��λ:����
        lcdelcdif.pheight=768;            //���߶�,��λ:����
        //������������.
    }else if(lcdid==0X1018)                //10.1��1280*800 RGB��    
    {
        lcdelcdif.pwidth=1280;            //�����,��λ:����
        lcdelcdif.pheight=800;            //���߶�,��λ:����
        lcdelcdif.hbp=140;                //ˮƽ����
        lcdelcdif.hfp=10;                //ˮƽǰ��
        lcdelcdif.hsw=10;                //ˮƽͬ�����
        lcdelcdif.vbp=10;                //��ֱ����
        lcdelcdif.vfp=10;                //��ֱǰ��
        lcdelcdif.vsw=3;                //��ֱͬ����� 
        ELCDIF_ClockInit(35,2,5,0);     //����LCDʱ��Ϊ70Mhz  
    } 
    
    lcdelcdif.width=lcdelcdif.pwidth;
    lcdelcdif.height=lcdelcdif.pheight;
    
#if LCD_PIXFORMAT==LCD_PIXFORMAT_ARGB8888||LCD_PIXFORMAT==LCD_PIXFORMAT_RGB888 
    lcdelcdif.pixsize=4;                //ÿ������ռ4���ֽ�
#else 
    lcdelcdif.pixsize=2;                //ÿ������ռ2���ֽ�
#endif     
    
    //����LCD
    elcdif_config.panelWidth=lcdelcdif.pwidth;          //LCD���
    elcdif_config.panelHeight=lcdelcdif.pheight;        //LCD�߶�
    elcdif_config.hsw=lcdelcdif.hsw;
    elcdif_config.hfp=lcdelcdif.hfp;
    elcdif_config.hbp=lcdelcdif.hbp;
    elcdif_config.vsw=lcdelcdif.vsw;
    elcdif_config.vfp=lcdelcdif.vfp;
    elcdif_config.vbp=lcdelcdif.vbp;

    //�����źż���
    elcdif_config.polarityFlags=kELCDIF_DataEnableActiveHigh|\
                                kELCDIF_VsyncActiveLow|\
                                kELCDIF_HsyncActiveLow|\
                                kELCDIF_DriveDataOnRisingClkEdge;
    elcdif_config.bufferAddr=(uint32_t)elcdif_lcd_framebuf;    //LCD�������׵�ַ
    elcdif_config.pixelFormat=kELCDIF_PixelFormatRGB565;    //�������ظ�ʽ
    elcdif_config.dataBus=kELCDIF_DataBus16Bit;             //������ظ�ʽ
    ELCDIF_RgbModeInit(LCDIF,&elcdif_config);               //��ʼ��eLCDIFΪRGBģʽ
    
    ELCDIF_RgbModeStart(LCDIF);                             //����eLCDIF RGBģʽ
    ELCDIF_LED(1);                                          //��LCD����
    ELCDIF_Display_Dir(0);                                  //����
    ELCDIF_Clear(0XFFFFFFFF);                               //����
}

//RGB LCD���ų�ʼ��
void ELCDIF_PinInit(void)
{
    gpio_pin_config_t lcd_config;
    
    //����RGB LCD��ʹ�õ�IO�ĸ��ù���
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
    
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_02_GPIO3_IO02,0);       //LCD����ʹ������
    
    //����RGB LCD��ʹ�õ�IO����ϸ����
    //��ת���ٶ�,��������ΪR0/2,�ٶ�Ϊ100Mhz���رտ�·���ܣ�ʹ��pull/keepr
    //ѡ��pull���ܣ�����100K Ohm������Hyst
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

    lcd_config.direction=kGPIO_DigitalOutput;    //���
    lcd_config.interruptMode=kGPIO_NoIntmode;    //��ʹ���жϹ���
    lcd_config.outputLogic=0;                    //Ĭ�ϵ͵�ƽ��LCD��������
    GPIO_PinInit(GPIO3,2,&lcd_config);             //��ʼ��GPIO2_31
}

//���㺯��
//x,y:����
//color:��ɫֵ
inline void ELCDIF_Draw_Point(u16 x,u16 y,u32 color)
{ 
#if LCD_PIXFORMAT==LCD_PIXEL_FORMAT_XRGB8888||LCD_PIXFORMAT==LCD_PIXEL_FORMAT_RGB888
    if(lcdelcdif.dir)    //����
    {
        *(u32*)((u32)elcdif_lcd_framebuf+lcdelcdif.pixsize*(lcdelcdif.pwidth*y+x))=color;
    }else             //����
    {
        *(u32*)((u32)elcdif_lcd_framebuf+lcdelcdif.pixsize*(lcdelcdif.pwidth*(lcdelcdif.pheight-x-1)+y))=color; 
    }
#else
    if(lcdelcdif.dir)    //����
    {
        *(u16*)((u32)elcdif_lcd_framebuf+lcdelcdif.pixsize*(lcdelcdif.pwidth*y+x))=color;
    }else             //����
    {
        *(u16*)((u32)elcdif_lcd_framebuf+lcdelcdif.pixsize*(lcdelcdif.pwidth*(lcdelcdif.pheight-x-1)+y))=color; 
    }
#endif
}

//���㺯��
//����ֵ:��ɫֵ
inline u32 ELCDIF_Read_Point(u16 x,u16 y)
{ 
#if LCD_PIXFORMAT==LCD_PIXEL_FORMAT_XRGB8888||LCD_PIXFORMAT==LCD_PIXEL_FORMAT_RGB888
    if(lcdelcdif.dir)    //����
    {
        return *(u32*)((u32)elcdif_lcd_framebuf+lcdelcdif.pixsize*(lcdelcdif.pwidth*y+x));
    }else             //����
    {
        return *(u32*)((u32)elcdif_lcd_framebuf+lcdelcdif.pixsize*(lcdelcdif.pwidth*(lcdelcdif.pheight-x-1)+y)); 
    }
#else
    if(lcdelcdif.dir)    //����
    {
        return *(u16*)((u32)elcdif_lcd_framebuf+lcdelcdif.pixsize*(lcdelcdif.pwidth*y+x));
    }else             //����
    {
        return *(u16*)((u32)elcdif_lcd_framebuf+lcdelcdif.pixsize*(lcdelcdif.pwidth*(lcdelcdif.pheight-x-1)+y)); 
    }
#endif 
}

//RGBLCD������
//(sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex-sx+1)*(ey-sy+1)   
//ע��:sx,ex,���ܴ���lcddev.width-1;sy,ey,���ܴ���lcddev.height-1!!!
//color:Ҫ������ɫ
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

//��ָ�����������ָ����ɫ��             
//(sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex-sx+1)*(ey-sy+1)   
//color:Ҫ������ɫ
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

//LCD����
//color:��ɫֵ
void ELCDIF_Clear(u32 color)
{
    u32 tpval;
    u32 i=0; 
#if (LCD_PIXFORMAT==LCD_PIXEL_FORMAT_RAW8)||(LCD_PIXFORMAT==LCD_PIXEL_FORMAT_RGB565)
    u16 *p=(u16*)elcdif_lcd_framebuf;    //ָ��֡�����׵�ַ
#else
    u32 *p=(u32*)elcdif_lcd_framebuf;    //ָ��֡�����׵�ַ
#endif 
    tpval=(u32)lcdelcdif.width*lcdelcdif.height;
    for(i=0;i<tpval;i++)
    {
        p[i]=color;
    }
}
