#include "dht11.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////     
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK I.MXRT1052开发板
//DHT11驱动代码       
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/12/30
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved                                      
//////////////////////////////////////////////////////////////////////////////////     

static void __dht11_plfm_init(void)  
{
    PCF8574_Init();                 //初始化PCF8574       
    PCF8574_ReadBit(BEEP_IO);       //由于DHT11和PCF8574的中断引脚共用一个IO，
                                    //所以在初始化DHT11之前要先读取一次PCF8574的任意一个IO，
                                    //使其释放掉中断引脚所占用的IO(PB12引脚),否则初始化DS18B20会出问题     
}

static void __dht11_read_prepare(void)  
{
    PCF8574_ReadBit(BEEP_IO);       //由于DHT11和PCF8574的中断引脚共用一个IO，
                                    //所以在初始化DHT11之前要先读取一次PCF8574的任意一个IO，
                                    //使其释放掉中断引脚所占用的IO(PB12引脚),否则初始化DS18B20会出问题          
  
}


static void __dht11_dq_out_set(int level)
{
     DHT11_DQ_OUT(level);

}



//复位DHT11
void DHT11_Rst(void)       
{                 
    DHT11_IO_OUT();     //设置为输出
    __dht11_dq_out_set(0);     //拉低DQ
    delay_ms(20);        //拉低至少18ms
    __dht11_dq_out_set(1);     //DQ=1 
    delay_us(30);         //主机拉高20~40us
}

//等待DHT11的回应
//返回1:未检测到DHT11的存在
//返回0:存在
u8 DHT11_Check(void)        
{   
    u8 retry=0;
    DHT11_IO_IN();      //设置为输出     
    while (DHT11_DQ_IN&&retry<100)//DHT11会拉低40~80us
    {
        retry++;
        delay_us(1);
    };     
    if(retry>=100)return 1;
    else retry=0;
    while (!DHT11_DQ_IN&&retry<100)//DHT11拉低后会再次拉高40~80us
    {
        retry++;
        delay_us(1);
    };
    if(retry>=100)return 1;        
    return 0;
}

//从DHT11读取一个位
//返回值：1/0
u8 DHT11_Read_Bit(void)              
{
     u8 retry=0;
    while(DHT11_DQ_IN&&retry<100)//等待变为低电平
    {
        retry++;
        delay_us(1);
    }
    retry=0;
    while(!DHT11_DQ_IN&&retry<100)//等待变高电平
    {
        retry++;
        delay_us(1);
    }
    delay_us(40);//等待40us
    if(DHT11_DQ_IN)return 1;
    else return 0;           
}

//从DHT11读取一个字节
//返回值：读到的数据
u8 DHT11_Read_Byte(void)    
{        
    u8 i,dat;
    dat=0;
    for (i=0;i<8;i++) 
    {
           dat<<=1; 
        dat|=DHT11_Read_Bit();
    }                            
    return dat;
}

//从DHT11读取一次数据
//temp:温度值(范围:0~50°)
//humi:湿度值(范围:20%~90%)
//返回值：0,正常;1,读取失败
u8 DHT11_Read_Data(u8 *temp,u8 *humi)    
{        
    u8 buf[5];
    u8 i;
  
    __dht11_read_prepare();
  
    DHT11_Rst();
    if(DHT11_Check()==0)
    {
        for(i=0;i<5;i++)//读取40位数据
        {
            buf[i]=DHT11_Read_Byte();
        }
        if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
        {
            *humi=buf[0];
            *temp=buf[2];
        }
    }else return 1;
    return 0;        
}

//初始化DHT11的IO口 DQ 同时检测DHT11的存在
//返回1:不存在
//返回0:存在          
u8 DHT11_Init(void)
{
    gpio_pin_config_t io_config;
     
     __dht11_plfm_init();
    
    //IO功能设置
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_11_GPIO1_IO11,0);     
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_11_GPIO1_IO11,0x10B0);

    io_config.direction=kGPIO_DigitalOutput;    //输出
    io_config.interruptMode=kGPIO_NoIntmode;    //不使用中断功能
    io_config.outputLogic=1;                    //默认高电平
    GPIO_PinInit(GPIO1,11,&io_config);          
    
 
    DHT11_Rst();
  
    return DHT11_Check();
}
