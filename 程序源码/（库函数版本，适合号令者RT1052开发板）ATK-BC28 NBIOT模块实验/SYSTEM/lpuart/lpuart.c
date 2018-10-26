#include "lpuart.h"
//////////////////////////////////////////////////////////////////////////////////      
//���ʹ��os,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"                    //os ʹ��      
#endif
//////////////////////////////////////////////////////////////////////////////////     
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK I.MX RT1052������
//����1��ʼ��           
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2017/12/10
//�汾��V1.5
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//********************************************************************************
//V1.0�޸�˵�� 
//////////////////////////////////////////////////////////////////////////////////      

static lpuart_config_t lpuart1_config;

//�������´���,֧��printf����,������Ҫѡ��use MicroLIB      
//#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)    
#if 1
//#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
    int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
    x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{     
    while((LPUART1->STAT&LPUART_STAT_TDRE_MASK)==0);
    LPUART1->DATA=(u8)ch;    
    
    return ch;
}
#endif 

#if EN_LPUART1_RX                        //���ʹ���˽���,��������´���
//����1�жϷ������      
u8 LPUART_RX_BUF[LPUART_REC_LEN];     //���ջ���,���LPUART_REC_LEN���ֽ�.
//����״̬
//bit15��    ������ɱ�־
//bit14��    ���յ�0x0d
//bit13~0��    ���յ�����Ч�ֽ���Ŀ
u16 LPUART_RX_STA=0;       //����״̬���    

//LPUART1�жϷ�������Ϊ�˽���Ч��ֱ�Ӳ����Ĵ���
void LPUART1_IRQHandler(void)
{
    u8 res=0;
    
    if((LPUART1->STAT)&kLPUART_RxDataRegFullFlag) //�����ж�
    {
        res=LPUART1->DATA;                    //��ȡ����
        if((LPUART_RX_STA&0x8000)==0)        //����δ���
        {
            if(LPUART_RX_STA&0x4000)            //���յ���0x0d
            {
                if(res!=0x0a)LPUART_RX_STA=0;//���մ���,���¿�ʼ
                else LPUART_RX_STA|=0x8000;    //��������� 
            }
            else                             //��û�յ�0X0D
            {    
                if(res==0x0d)LPUART_RX_STA|=0x4000;
                else
                {
                    LPUART_RX_BUF[LPUART_RX_STA&0X3FFF]=res ;
                    LPUART_RX_STA++;
                    if(LPUART_RX_STA>(LPUART_REC_LEN-1))LPUART_RX_STA=0;//�������ݴ���,���¿�ʼ����      
                }         
            }
        } 
    }
    __DSB();                //����ͬ������ָ��
}

//void LPUART1_IRQHandler(void)
//{
//    u8 res=0;
//    
//    if(LPUART_GetStatusFlags(LPUART1)&kLPUART_RxDataRegFullFlag) //�����ж�
//    {
//        res=LPUART_ReadByte(LPUART1);          //��ȡ����
//        if((LPUART_RX_STA&0x8000)==0)        //����δ���
//        {
//            if(LPUART_RX_STA&0x4000)            //���յ���0x0d
//            {
//                if(res!=0x0a)LPUART_RX_STA=0;//���մ���,���¿�ʼ
//                else LPUART_RX_STA|=0x8000;    //��������� 
//            }
//            else                             //��û�յ�0X0D
//            {    
//                if(res==0x0d)LPUART_RX_STA|=0x4000;
//                else
//                {
//                    LPUART_RX_BUF[LPUART_RX_STA&0X3FFF]=res ;
//                    LPUART_RX_STA++;
//                    if(LPUART_RX_STA>(LPUART_REC_LEN-1))LPUART_RX_STA=0;//�������ݴ���,���¿�ʼ����      
//                }         
//            }
//        } 
//    }
//}
#endif    

//��ȡLPUART��ʱ��ԴƵ�ʣ�����ǰ�����õ���80MHz
//����ֵ��LPUARTʱ��ԴƵ��,�������ǵ�ֻ��һ����80Mhz
static u32 LPUART_SrcFreqGet(void)
{
    uint32_t freq;
    
    if(CLOCK_GetMux(kCLOCK_UartMux)==0)     //LPUART��ʱ��Դѡ��PLL3/6
    {
        freq=(CLOCK_GetPllFreq(kCLOCK_PllUsb1)/6U)/(CLOCK_GetDiv(kCLOCK_UartDiv)+1U);
    }
    else                                    //LPUART��ʱ��Դѡ��OSC
    {
        freq=CLOCK_GetOscFreq()/(CLOCK_GetDiv(kCLOCK_UartDiv)+1U);
    }
    return freq;
}

//��ʼ��IO ����1 
//bound:������
void LPUART1_Init(u32 bound)
{    
    u32 freq=0;                            //���ڵ�ʱ��ԴƵ��
    
    CLOCK_EnableClock(kCLOCK_Lpuart1);    //ʹ��LPUART1ʱ��
    
    CLOCK_SetMux(kCLOCK_UartMux,0);     //����UARTʱ��ԴΪPLL3 80Mhz��PLL3/6=480/6=80MHz
    CLOCK_SetDiv(kCLOCK_UartDiv,0);     //����UARTʱ��1��Ƶ����UARTʱ��Ϊ80Mhz
    
    //LPUART1��ʹ�õ�IO�������ã�������ALT0~ALT7ѡ����ʵĹ��ܡ�
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_12_LPUART1_TX,0U);    //GPIO_AD_B0_12����ΪLPUART1_TX
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_13_LPUART1_RX,0U);    //GPIO_AD_B0_13����ΪLPUART1_RX

    //����IO����GPIO_AD_B0_12��GPIO_AD_B0_13�Ĺ���
    //��ת���ٶ�,��������ΪR0/6,�ٶ�Ϊ100Mhz���رտ�·���ܣ�ʹ��pull/keepr
    //ѡ��keeper���ܣ�����100K Ohm���ر�Hyst
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_12_LPUART1_TX,0x10B0u); 
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_13_LPUART1_RX,0x10B0u); 

    freq=LPUART_SrcFreqGet();    
    
    LPUART_GetDefaultConfig(&lpuart1_config);                 //������ΪĬ�����ã������ڸ���ʵ���������
    
    //��ʼ��NXP�ٷ��ṩ��debug console���˺��������³�ʼ��LPUART1���������Ǻ����
    //������ʾ�ĳ�ʼ��һ��LPUART1��DbgConsole_Init()��Ҫ�Ǹ���Щ��Ҫʹ��NXP�ٷ�
    //���Թ��ܵĿ�����ʹ�õģ�����Ҫʹ�õĻ��Ϳ��Խ��������ע�͵�
    DbgConsole_Init(BOARD_DEBUG_UART_BASEADDR,bound,BOARD_DEBUG_UART_TYPE,freq);
    
    lpuart1_config.baudRate_Bps=bound;                        //������
    lpuart1_config.dataBitsCount=kLPUART_EightDataBits;        //8λ
    lpuart1_config.stopBitCount=kLPUART_OneStopBit;            //1λֹͣλ
    lpuart1_config.parityMode=kLPUART_ParityDisabled;        //����żУ��
    lpuart1_config.enableRx=true;                            //ʹ�ܽ���
    lpuart1_config.enableTx=true;                            //ʹ�ܷ���
    
    LPUART_Init(LPUART1,&lpuart1_config,freq);                //��ʼ��LPUART1 
    
#if EN_LPUART1_RX                                            //�Ƿ���Ҫ�����ж�?    
    //LPUART�ж�����
    LPUART_EnableInterrupts(LPUART1,kLPUART_RxDataRegFullInterruptEnable); //ʹ�ܽ����ж�
    RT1052_NVIC_SetPriority(LPUART1_IRQn,5,0);                //��ռ���ȼ�5�������ȼ�0
    EnableIRQ(LPUART1_IRQn);                                //ʹ��LPUART1�ж�
#endif
}    
//void LPUART1_IRQHandler(void)
//{
//    u8 res=0;
//    
//    if(LPUART_GetStatusFlags(LPUART1)&kLPUART_RxDataRegFullFlag) //�����ж�
//    {
//        res=LPUART_ReadByte(LPUART1);          //��ȡ����
//        if((LPUART_RX_STA&0x8000)==0)        //����δ���
//        {
//            if(LPUART_RX_STA&0x4000)            //���յ���0x0d
//            {
//                if(res!=0x0a)LPUART_RX_STA=0;//���մ���,���¿�ʼ
//                else LPUART_RX_STA|=0x8000;    //��������� 
//            }
//            else                             //��û�յ�0X0D
//            {    
//                if(res==0x0d)LPUART_RX_STA|=0x4000;
//                else
//                {
//                    LPUART_RX_BUF[LPUART_RX_STA&0X3FFF]=res ;
//                    LPUART_RX_STA++;
//                    if(LPUART_RX_STA>(LPUART_REC_LEN-1))LPUART_RX_STA=0;//�������ݴ���,���¿�ʼ����      
//                }         
//            }
//        } 
//    }
//}








 




