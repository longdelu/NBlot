#ifndef _LPSPI_H
#define _LPSPI_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////     
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK I.MX RT1052������
//LPSPI��������       
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2018/3/7
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved                                      
//////////////////////////////////////////////////////////////////////////////////     

extern lpspi_master_config_t lpspi3_config;

void LPSPI3_Init(u8 baudrate);
u8 LPSPI3_ReadWriteByte(u8 TxData);
void SPI_Test(void);
#endif

