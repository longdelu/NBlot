#ifndef _USART_H
#define _USART_H
#include "sys.h"
#include "stdio.h"    
#include "fsl_common.h"
#include "fsl_iomuxc.h"
//#include "pin_mux.h"
#include "fsl_debug_console.h"
//////////////////////////////////////////////////////////////////////////////////     
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK I.MX RT1052������
//����1��ʼ��           
//����ԭ��@ALIENTEK
//������̳:www.openedv.csom
//�޸�����:2017/12/10
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
//********************************************************************************
//V1.0�޸�˵�� 
//////////////////////////////////////////////////////////////////////////////////     

//��NXP�ٷ��������ṩ�ĸ�debug console�йصĺ궨��
#define BOARD_DEBUG_UART_TYPE         DEBUG_CONSOLE_DEVICE_TYPE_LPUART
#define BOARD_DEBUG_UART_BASEADDR     (uint32_t) LPUART1

#define LPUART_REC_LEN          200      //�����������ֽ��� 200
#define EN_LPUART1_RX             1        //ʹ�ܣ�1��/��ֹ��0������1����
          
extern u8  LPUART_RX_BUF[LPUART_REC_LEN]; //���ջ���,���LPUART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 LPUART_RX_STA;                 //����״̬���    

void LPUART1_Init(u32 bound);


#endif
