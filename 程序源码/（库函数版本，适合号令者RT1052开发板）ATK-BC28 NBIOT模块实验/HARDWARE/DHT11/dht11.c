#include "dht11.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////     
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK I.MXRT1052������
//DHT11��������       
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/12/30
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved                                      
//////////////////////////////////////////////////////////////////////////////////     

static void __dht11_plfm_init(void)  
{
    PCF8574_Init();                 //��ʼ��PCF8574       
    PCF8574_ReadBit(BEEP_IO);       //����DHT11��PCF8574���ж����Ź���һ��IO��
                                    //�����ڳ�ʼ��DHT11֮ǰҪ�ȶ�ȡһ��PCF8574������һ��IO��
                                    //ʹ���ͷŵ��ж�������ռ�õ�IO(PB12����),�����ʼ��DS18B20�������     
}

static void __dht11_read_prepare(void)  
{
    PCF8574_ReadBit(BEEP_IO);       //����DHT11��PCF8574���ж����Ź���һ��IO��
                                    //�����ڳ�ʼ��DHT11֮ǰҪ�ȶ�ȡһ��PCF8574������һ��IO��
                                    //ʹ���ͷŵ��ж�������ռ�õ�IO(PB12����),�����ʼ��DS18B20�������          
  
}


static void __dht11_dq_out_set(int level)
{
     DHT11_DQ_OUT(level);

}



//��λDHT11
void DHT11_Rst(void)       
{                 
    DHT11_IO_OUT();     //����Ϊ���
    __dht11_dq_out_set(0);     //����DQ
    delay_ms(20);        //��������18ms
    __dht11_dq_out_set(1);     //DQ=1 
    delay_us(30);         //��������20~40us
}

//�ȴ�DHT11�Ļ�Ӧ
//����1:δ��⵽DHT11�Ĵ���
//����0:����
u8 DHT11_Check(void)        
{   
    u8 retry=0;
    DHT11_IO_IN();      //����Ϊ���     
    while (DHT11_DQ_IN&&retry<100)//DHT11������40~80us
    {
        retry++;
        delay_us(1);
    };     
    if(retry>=100)return 1;
    else retry=0;
    while (!DHT11_DQ_IN&&retry<100)//DHT11���ͺ���ٴ�����40~80us
    {
        retry++;
        delay_us(1);
    };
    if(retry>=100)return 1;        
    return 0;
}

//��DHT11��ȡһ��λ
//����ֵ��1/0
u8 DHT11_Read_Bit(void)              
{
     u8 retry=0;
    while(DHT11_DQ_IN&&retry<100)//�ȴ���Ϊ�͵�ƽ
    {
        retry++;
        delay_us(1);
    }
    retry=0;
    while(!DHT11_DQ_IN&&retry<100)//�ȴ���ߵ�ƽ
    {
        retry++;
        delay_us(1);
    }
    delay_us(40);//�ȴ�40us
    if(DHT11_DQ_IN)return 1;
    else return 0;           
}

//��DHT11��ȡһ���ֽ�
//����ֵ������������
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

//��DHT11��ȡһ������
//temp:�¶�ֵ(��Χ:0~50��)
//humi:ʪ��ֵ(��Χ:20%~90%)
//����ֵ��0,����;1,��ȡʧ��
u8 DHT11_Read_Data(u8 *temp,u8 *humi)    
{        
    u8 buf[5];
    u8 i;
  
    __dht11_read_prepare();
  
    DHT11_Rst();
    if(DHT11_Check()==0)
    {
        for(i=0;i<5;i++)//��ȡ40λ����
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

//��ʼ��DHT11��IO�� DQ ͬʱ���DHT11�Ĵ���
//����1:������
//����0:����          
u8 DHT11_Init(void)
{
    gpio_pin_config_t io_config;
     
     __dht11_plfm_init();
    
    //IO��������
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_11_GPIO1_IO11,0);     
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_11_GPIO1_IO11,0x10B0);

    io_config.direction=kGPIO_DigitalOutput;    //���
    io_config.interruptMode=kGPIO_NoIntmode;    //��ʹ���жϹ���
    io_config.outputLogic=1;                    //Ĭ�ϸߵ�ƽ
    GPIO_PinInit(GPIO1,11,&io_config);          
    
 
    DHT11_Rst();
  
    return DHT11_Check();
}
