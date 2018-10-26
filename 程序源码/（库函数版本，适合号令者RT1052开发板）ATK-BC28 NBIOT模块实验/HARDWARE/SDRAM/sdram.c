#include "sdram.h"
#include "lpuart.h"
//////////////////////////////////////////////////////////////////////////////////     
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK I.MX RT1052������
//SDRAM��������       
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2018/1/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved                                      
//////////////////////////////////////////////////////////////////////////////////     

semc_config_t       semc_configure;     //semc���ýṹ��
semc_sdram_config_t sdram_config;       //SDRAM���ýṹ��

//��ʼ��SDRAM
void SDRAM_Init(void)
{
    u32 semcclock=0;

    //����SDRAM��ʹ�õ�IO
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_00_SEMC_DATA00,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_01_SEMC_DATA01,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_02_SEMC_DATA02,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_03_SEMC_DATA03,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_04_SEMC_DATA04,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_05_SEMC_DATA05,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_06_SEMC_DATA06,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_07_SEMC_DATA07,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_08_SEMC_DM00,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_09_SEMC_ADDR00,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_10_SEMC_ADDR01,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_11_SEMC_ADDR02,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_12_SEMC_ADDR03,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_13_SEMC_ADDR04,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_14_SEMC_ADDR05,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_15_SEMC_ADDR06,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_16_SEMC_ADDR07,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_17_SEMC_ADDR08,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_18_SEMC_ADDR09,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_19_SEMC_ADDR11,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_20_SEMC_ADDR12,0);
    
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_21_SEMC_BA0,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_22_SEMC_BA1,0);
    
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_23_SEMC_ADDR10,0);
    
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_24_SEMC_CAS,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_25_SEMC_RAS,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_26_SEMC_CLK,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_27_SEMC_CKE,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_28_SEMC_WE,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_29_SEMC_CS0,0);
    
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_30_SEMC_DATA08,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_31_SEMC_DATA09,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_32_SEMC_DATA10,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_33_SEMC_DATA11,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_34_SEMC_DATA12,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_35_SEMC_DATA13,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_36_SEMC_DATA14,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_37_SEMC_DATA15,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_38_SEMC_DM01,0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_39_SEMC_DQS,1);
  
    //��ת���ٶ�,�ر�����R0/7�ٶ�Ϊ200Mhz���رտ�·���ܣ�ʹ��pull/keepr
    //ѡ��keeper���ܣ�����100K Ohm��ʹ��Hyst   
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_00_SEMC_DATA00,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_01_SEMC_DATA01,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_02_SEMC_DATA02,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_03_SEMC_DATA03,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_04_SEMC_DATA04,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_05_SEMC_DATA05,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_06_SEMC_DATA06,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_07_SEMC_DATA07,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_08_SEMC_DM00,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_09_SEMC_ADDR00,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_10_SEMC_ADDR01,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_11_SEMC_ADDR02,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_12_SEMC_ADDR03,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_13_SEMC_ADDR04,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_14_SEMC_ADDR05,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_15_SEMC_ADDR06,0X110F9);   
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_16_SEMC_ADDR07,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_17_SEMC_ADDR08,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_18_SEMC_ADDR09,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_19_SEMC_ADDR11,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_20_SEMC_ADDR12,0X110F9);
    
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_21_SEMC_BA0,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_22_SEMC_BA1,0X110F9);
    
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_23_SEMC_ADDR10,0X110F9);
    
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_24_SEMC_CAS,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_25_SEMC_RAS,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_26_SEMC_CLK,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_27_SEMC_CKE,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_28_SEMC_WE,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_29_SEMC_CS0,0X110F9);
    
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_30_SEMC_DATA08,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_31_SEMC_DATA09,0X110F9);        
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_32_SEMC_DATA10,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_33_SEMC_DATA11,0x110F9); 
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_34_SEMC_DATA12,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_35_SEMC_DATA13,0X110F9);        
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_36_SEMC_DATA14,0X110F9);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_37_SEMC_DATA15,0x110F9); 
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_38_SEMC_DM01,0x110F9);  
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_39_SEMC_DQS,0x110F9); 
    
    //����SEMC��SDRAM������
    SEMC_GetDefaultConfig(&semc_configure);         //������ΪĬ��ֵ
    semc_configure.dqsMode=kSEMC_Loopbackdqspad;    //DQSʹ��EMC_39���ţ����ô����ŵĻ�SDRAM�ٶ��ϲ�ȥ��
    SEMC_Init(SEMC,&semc_configure);                //��ʼ��SEMC
    
    semcclock=CLOCK_GetFreq(kCLOCK_SemcClk);        //��ȡSEMCʱ�ӣ�132Mhz
    sdram_config.csxPinMux=kSEMC_MUXCSX0;           //ƬѡCSX0
    sdram_config.address=SDRAM_START_ADDR;          //SDRAM��ʼ��ַ
    sdram_config.memsize_kbytes=SDRAM_SIZE_KBYTES;  //SDRAM��С����λkBytes
    sdram_config.portSize=kSEMC_PortSize16Bit;      //SDRAMλ��16λ
    sdram_config.burstLen=kSEMC_Sdram_BurstLen1;    //ͻ������1
    sdram_config.columnAddrBitNum=kSEMC_SdramColunm_9bit;   //�е�ַ9λ
    sdram_config.casLatency=kSEMC_LatencyThree;     //CL=3
    sdram_config.tPrecharge2Act_Ns=20;              //tRP 
    sdram_config.tAct2ReadWrite_Ns=20;              //tRCD
    sdram_config.tRefreshRecovery_Ns=67;            //tRFC��tXSR�����ֵ
    sdram_config.tWriteRecovery_Ns=20;              //tWR
    sdram_config.tCkeOff_Ns=(1000000000/semcclock); //CLK��С�ر�ʱ�䣬Ҳ����CLK���ڣ���λns
    sdram_config.tAct2Prechage_Ns=50;               //tRAS
    sdram_config.tSelfRefRecovery_Ns=80;            //tXSR
    sdram_config.tRefresh2Refresh_Ns=100;           //tRFC
    sdram_config.tAct2Act_Ns=70;                    //tRC
    sdram_config.tPrescalePeriod_Ns=160*(1000000000/semcclock);//��Ƶ����,160��ʱ������
    sdram_config.refreshPeriod_nsPerRow=(64*1000000)/8192;//ˢ�����ڣ�ˢ��һ����Ҫ��ʱ�䣬��λns
    sdram_config.refreshUrgThreshold=sdram_config.refreshPeriod_nsPerRow;
    sdram_config.refreshBurstLen=1;
    
    SEMC_ConfigureSDRAM(SEMC,kSEMC_SDRAM_CS0,&sdram_config,semcclock);//����SDRAM   
}

//��ָ����ַ(WriteAddr+SDRAM_START_ADDR)��ʼ,����д��n���ֽ�.
//pBuffer:�ֽ�ָ��
//WriteAddr:Ҫд��ĵ�ַ
//n:Ҫд����ֽ���
void SEMC_SDRAM_WriteBuffer(u8 *pBuffer,u32 WriteAddr,u32 n)
{
    for(;n!=0;n--)
    {
        *(vu8*)(SDRAM_START_ADDR+WriteAddr)=*pBuffer;
        WriteAddr++;
        pBuffer++;
    }
}

//��ָ����ַ((WriteAddr+SDRAM_START_ADDR))��ʼ,��������n���ֽ�.
//pBuffer:�ֽ�ָ��
//ReadAddr:Ҫ��������ʼ��ַ
//n:Ҫд����ֽ���
void SEMC_SDRAM_ReadBuffer(u8 *pBuffer,u32 ReadAddr,u32 n)
{
    for(;n!=0;n--)
    {
        *pBuffer++=*(vu8*)(SDRAM_START_ADDR+ReadAddr);
        ReadAddr++;
    }
}

