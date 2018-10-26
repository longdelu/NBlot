#include "rs485.h"
#include "pcf8574.h"
#include "lpuart.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK I.MXRT1052������
//RS485��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/12/29
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

static lpuart_config_t lpuart4_config;

#if EN_USART4_RX   		//���ʹ���˽���   	  
//���ջ����� 	
u8 RS485_RX_BUF[64];  	//���ջ���,���64���ֽ�.
//���յ������ݳ���
u8 RS485_RX_CNT=0;  

void LPUART4_IRQHandler(void)
{
    u8 res;	  

    if((LPUART4->STAT)&kLPUART_RxDataRegFullFlag)   //�����ж�
    {
        res=LPUART_ReadByte(LPUART4);  		        //��ȡ����			
        if(RS485_RX_CNT<64)
		{
			RS485_RX_BUF[RS485_RX_CNT]=res;		    //��¼���յ���ֵ
			RS485_RX_CNT++;						    //������������1 
		} 
    }
    __DSB();				                        //����ͬ������ָ��
}    
#endif

//��ȡLPUART��ʱ��ԴƵ�ʣ�����ǰ�����õ���80MHz
//����ֵ��LPUARTʱ��ԴƵ��,�������ǵ�ֻ��һ����80Mhz
u32 LPUART4_SrcFreqGet(void)
{
    uint32_t freq;
	
    if (CLOCK_GetMux(kCLOCK_UartMux)==0) 	//LPUART��ʱ��Դѡ��PLL3/6
    {
        freq=(CLOCK_GetPllFreq(kCLOCK_PllUsb1)/6)/(CLOCK_GetDiv(kCLOCK_UartDiv)+1);
    }
    else									//LPUART��ʱ��Դѡ��OSC
    {
        freq=CLOCK_GetOscFreq()/(CLOCK_GetDiv(kCLOCK_UartDiv)+1);
    }
    return freq;
}


//��ʼ��IO ����8
//bound:������
void RS485_Init(u32 bound)
{
	u32 freq=0;							//���ڵ�ʱ��ԴƵ��
	
    PCF8574_Init();                     //��ʼ��PCF8574�����ڿ���RE��
    
//  LPUART1�Ѿ�������UARTʱ��Դ    
//	CLOCK_EnableClock(kCLOCK_Lpuart4);	//ʹ��LPUART8ʱ��
//	CLOCK_SetMux(kCLOCK_UartMux,0); 	//����UARTʱ��ԴΪPLL3 80Mhz��PLL3/6=480/6=80MHz
//  CLOCK_SetDiv(kCLOCK_UartDiv,0); 	//����UARTʱ��1��Ƶ����UARTʱ��Ϊ80Mhz
	
	//LPUART4��ʹ�õ�IO�������ã�������ALT0~ALT7ѡ����ʵĹ��ܡ�
	IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_00_LPUART4_TX,0);	//GPIO_SD_B1_00����ΪLPUART4_TX
	IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_01_LPUART4_RX,0);	//GPIO_AD_B1_11����ΪLPUART4_RX

	//����IO����GPIO_SD_B1_00��GPIO_SD_B1_01�Ĺ���
	//��ת���ٶ�,��������ΪR0/6,�ٶ�Ϊ100Mhz���رտ�·���ܣ�ʹ��pull/keepr
	//ѡ��keeper���ܣ�����100K Ohm���ر�Hyst
	IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_00_LPUART4_TX,0x10B0); 
	IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_01_LPUART4_RX,0x10B0); 

	freq=LPUART4_SrcFreqGet();	
	LPUART_GetDefaultConfig(&lpuart4_config); 				//������ΪĬ�����ã������ڸ���ʵ���������
	
	lpuart4_config.baudRate_Bps=bound;						//������
	lpuart4_config.dataBitsCount=kLPUART_EightDataBits;		//8λ
	lpuart4_config.stopBitCount=kLPUART_OneStopBit;			//1λֹͣλ
	lpuart4_config.parityMode=kLPUART_ParityDisabled;		//����żУ��
	lpuart4_config.enableRx=true;							//ʹ�ܽ���
	lpuart4_config.enableTx=true;							//ʹ�ܷ���
	
	LPUART_Init(LPUART4,&lpuart4_config,freq);				//��ʼ��LPUART4
	
	//LPUART�ж�����
#if EN_USART4_RX
	LPUART_EnableInterrupts(LPUART4,kLPUART_RxDataRegFullInterruptEnable); //ʹ�ܽ����ж�
	RT1052_NVIC_SetPriority(LPUART4_IRQn,6,0);				//��ռ���ȼ�6�������ȼ�0
	EnableIRQ(LPUART4_IRQn);	//ʹ��LPUART1�ж�
#endif	

    RS485_TX_Set(0);                                        //����Ϊ����ģʽ	
}

//RS485����len���ֽ�.
//buf:�������׵�ַ
//len:���͵��ֽ���(Ϊ�˺ͱ�����Ľ���ƥ��,���ｨ�鲻Ҫ����64���ֽ�)
void RS485_Send_Data(u8 *buf,u8 len)
{
	RS485_TX_Set(1);			            //����Ϊ����ģʽ
    LPUART_WriteBlocking(LPUART4,buf,len);  //����4��������
	RS485_RX_CNT=0;	  
	RS485_TX_Set(0);			            //����Ϊ����ģʽ	
}
//RS485��ѯ���յ�������
//buf:���ջ����׵�ַ
//len:���������ݳ���
void RS485_Receive_Data(u8 *buf,u8 *len)
{
	u8 rxlen=RS485_RX_CNT;
	u8 i=0;
	*len=0;				//Ĭ��Ϊ0
	delay_ms(10);		//�ȴ�10ms,��������10msû�н��յ�һ������,����Ϊ���ս���
	if(rxlen==RS485_RX_CNT&&rxlen)//���յ�������,�ҽ��������
	{
		for(i=0;i<rxlen;i++)
		{
			buf[i]=RS485_RX_BUF[i];	
		}		
		*len=RS485_RX_CNT;	//��¼�������ݳ���
		RS485_RX_CNT=0;		//����
	}
} 
//RS485ģʽ����.
//en:0,����;1,����.
void RS485_TX_Set(u8 en)
{
	PCF8574_WriteBit(RS485_RE_IO,en);
}
