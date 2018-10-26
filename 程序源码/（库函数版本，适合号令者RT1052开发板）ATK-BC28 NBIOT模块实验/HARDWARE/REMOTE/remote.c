#include "remote.h"
#include "delay.h"
#include "lpuart.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK ������RT1052������
//����ң�ؽ�����������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2018/5/12
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	


//����ң�س�ʼ��
//����IO�Լ�GPT2_CH1�����벶��
//psc : Ԥ��Ƶ��,0~4095
//������ʱ��=(2^32)*(psc+1)/PERCLK_CLK_ROOT=234562.5��,�����ϲ��������
void Remote_Init(void)
{
    gpt_config_t gpt2_onfig;
    
    //����GPT2_CAP1���IO(GPIO_EMC_41)�Ĺ���
	//��ת���ٶ�,����R0/6,�ٶ�Ϊ200Mhz���رտ�·���ܣ�ʹ��pull/keepr
	//ѡ��pull���ܣ�����22K Ohm 
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_41_GPT2_CAPTURE1,0);	  
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_41_GPT2_CAPTURE1,0xF0F1);
    
    GPT_GetDefaultConfig(&gpt2_onfig);	//�ȳ�ʼ��GPT2ΪĬ��ֵ
    gpt2_onfig.divider=74;	            //���÷�Ƶֵ,1Mhz����Ƶ��
	gpt2_onfig.clockSource=kGPT_ClockSource_Periph;	//����ʱ��Դ
	GPT_Init(GPT2,&gpt2_onfig);         //��ʼ��GPT2
  
    //����CAP2Ϊ�����ز���
    GPT_SetOutputCompareValue(GPT2,kGPT_OutputCompare_Channel1,10000);	    //���ñȽϼ���ֵ,10ms�ж�
    GPT_SetInputOperationMode(GPT2,kGPT_InputCapture_Channel1,kGPT_InputOperation_RiseEdge);
    GPT_EnableInterrupts(GPT2,kGPT_InputCapture1InterruptEnable);   //ʹ��GPT2�����ж�
    GPT_EnableInterrupts(GPT2,kGPT_OutputCompare1InterruptEnable);			//ʹ��GPT�Ƚ�ͨ��1�ж�
    GPT_StartTimer(GPT2);	            //��ʼ��ʱ��
    
    //�ж�����
    RT1052_NVIC_SetPriority(GPT2_IRQn,7,0);//����GPT2�ж����ȼ�
    EnableIRQ(GPT2_IRQn);	            //ʹ��GPT2�ж�
}


//ң��������״̬
//[7]:�յ����������־
//[6]:�õ���һ��������������Ϣ
//[5]:����	
//[4]:����������Ƿ��Ѿ�������								   
//[3:0]:�����ʱ��
u8 	RmtSta=0;	  	  
u16 Dval;		//�½���ʱ��������ֵ
u32 RmtRec=0;	//������յ�������	   		    
u8  RmtCnt=0;	//�������µĴ���	 

void GPT2_IRQHandler(void)
{
    //OCR1�ж�
    if(GPT_GetStatusFlags(GPT2,kGPT_OutputCompare1Flag))
    {
 		if(RmtSta&0x80)                         //�ϴ������ݱ����յ���
		{	
			RmtSta&=~0X10;						//ȡ���������Ѿ���������
			if((RmtSta&0X0F)==0X00)RmtSta|=1<<6;//����Ѿ����һ�ΰ����ļ�ֵ��Ϣ�ɼ�
			if((RmtSta&0X0F)<14)RmtSta++;
			else
			{
				RmtSta&=~(1<<7);                //���������ʶ
				RmtSta&=0XF0;	                //��ռ�����	
			}						 	   	
		}
        GPT_ClearStatusFlags(GPT2,kGPT_OutputCompare1Flag);//����жϱ�־λ
    }
    
    if(GPT_GetStatusFlags(GPT2,kGPT_InputCapture1Flag))  //����1���������¼�    
    {
        GPT_StopTimer(GPT2);
        REMOTE_IO_GPIO();
        if(RDATA)                               //�����ز���
        {	
            GPT_SetInputOperationMode(GPT2,kGPT_InputCapture_Channel1,kGPT_InputOperation_FallEdge);
            RmtSta|=0X10;					    //����������Ѿ�������
        }
        else //�½��ز���
        {
            Dval=GPT_GetInputCaptureValue(GPT2,kGPT_InputCapture_Channel1);	//��ȡ����ֵ	
            //���ò��������� 
            GPT_SetInputOperationMode(GPT2,kGPT_InputCapture_Channel1,kGPT_InputOperation_RiseEdge);
            if(RmtSta&0X10)					    //���һ�θߵ�ƽ���� 
            {
                if(RmtSta&0X80)                 //���յ���������
                {       
                    if(Dval>300&&Dval<800)      //560Ϊ��׼ֵ,560us
                    {
                        RmtRec<<=1;	            //����һλ.
                        RmtRec|=0;	            //���յ�0	
                    }else if(Dval>1400&&Dval<1800)	//1680Ϊ��׼ֵ,1680us
                    {
                        RmtRec<<=1;	            //����һλ.
                        RmtRec|=1;	            //���յ�1
                    }else if(Dval>2200&&Dval<2600)	//�õ�������ֵ���ӵ���Ϣ 2500Ϊ��׼ֵ2.5ms
                    {
                        RmtCnt++; 		        //������������1��
                        RmtSta&=0XF0;	        //��ռ�ʱ��		
                    }
                }
                else if(Dval>4200&&Dval<4700)   //4500Ϊ��׼ֵ4.5ms
                {
                    RmtSta|=1<<7;	            //��ǳɹ����յ���������
                    RmtCnt=0;		            //�����������������
                }						 
            }
            RmtSta&=~(1<<4);
        }
        REMOTE_IO_GPT();
        GPT_StartTimer(GPT2);
    }		    	     	    					   
    GPT_ClearStatusFlags(GPT2,kGPT_RollOverFlag);       //�������жϱ�־λ 
    GPT_ClearStatusFlags(GPT2,kGPT_InputCapture1Flag);  //�������2�жϱ�־λ    
	__DSB();			//����ͬ������ָ�� 
}


//����������
//����ֵ:
//	 0,û���κΰ�������
//����,���µİ�����ֵ.
u8 Remote_Scan(void)
{        
	u8 sta=0;       
    u8 t1,t2;  
	if(RmtSta&(1<<6))//�õ�һ��������������Ϣ��
	{ 
	    t1=RmtRec>>24;			//�õ���ַ��
	    t2=(RmtRec>>16)&0xff;	//�õ���ַ���� 
 	    if((t1==(u8)~t2)&&t1==REMOTE_ID)//����ң��ʶ����(ID)����ַ 
	    { 
	        t1=RmtRec>>8;
	        t2=RmtRec; 	
	        if(t1==(u8)~t2)sta=t1;//��ֵ��ȷ	 
		}   
		if((sta==0)||((RmtSta&0X80)==0))//�������ݴ���/ң���Ѿ�û�а�����
		{
		 	RmtSta&=~(1<<6);//������յ���Ч������ʶ
			RmtCnt=0;		//�����������������
		}
	} 
    //else printf("δ�õ�������Ϣ\r\n"); 
    return sta;
}
