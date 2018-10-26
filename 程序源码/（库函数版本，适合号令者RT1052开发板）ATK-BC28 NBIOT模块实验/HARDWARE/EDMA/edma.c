#include "edma.h"
#include "lpuart.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK I.MX RT1052������
//DMA��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2018/5/10
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

edma_handle_t lpuart1_txdmahandle;             //DMA���

//��ʼ��DMA0ͨ��1
void MYEDMA_Init(void)
{
    edma_config_t edma_lpuartconfig;
       
    //����DMAMUX
    DMAMUX_Init(DMAMUX);                        //��ʼ��DMAMUX0
    DMAMUX_SetSource(DMAMUX,1,kDmaRequestMuxLPUART1Tx); //DMAͨ��1ӳ�䵽LPUART1 TXͨ��
    DMAMUX_EnableChannel(DMAMUX,1);
    
    //��ʼ��EDMAͨ��0
    EDMA_GetDefaultConfig(&edma_lpuartconfig);      //������ΪĬ��ֵ
    EDMA_Init(DMA0,&edma_lpuartconfig);             //��ʼ��DMA
    EDMA_CreateHandle(&lpuart1_txdmahandle,DMA0,1);//����DMA���,ͨ��1
}

//DMA����
//srcaddr:Դ��ַ
//destaddr:Ŀ�ĵ�ַ
//length�����䳤��
void EDMA_Transmit(void *srcaddr, void *destaddr, u32 length)
{
    edma_transfer_config_t trans_config;
    
    //׼������
    EDMA_PrepareTransfer(&trans_config,                 //�������ýṹ��
                        srcaddr,                        //Դ��ַ
                        1,                              //Դ��ȣ�1���ֽ�
                        destaddr,                       //Ŀ�ĵ�ַ
                        1,                              //Ŀ�Ŀ�ȣ�1���ֽ�
                        1,                              //ÿ����������ֽڳ��ȣ�1���ֽ�
                        length,                         //�����ܳ���
                        kEDMA_MemoryToPeripheral);      //�������ͣ��ڴ浽�ڴ�
    EDMA_SubmitTransfer(&lpuart1_txdmahandle,&trans_config);      //�ύ����
    EDMA_StartTransfer(&lpuart1_txdmahandle);                     //��ʼ����

}
