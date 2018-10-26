#ifndef _ATK_SYS_H
#define _ATK_SYS_H


#include <stdio.h>

#include "MIMXRT1052.h"
#include "core_cm7.h"
#include "MIMXRT1052_features.h"
#include "fsl_adc.h"
#include "fsl_adc_etc.h"
#include "fsl_aipstz.h"
#include "fsl_aoi.h"
#include "fsl_bee.h"
#include "fsl_cache.h"
#include "fsl_clock.h"
#include "fsl_cmp.h"
#include "fsl_common.h"
#include "fsl_csi.h"
#include "fsl_dcdc.h"
#include "fsl_dmamux.h"
#include "fsl_edma.h"
#include "fsl_elcdif.h"
#include "fsl_enc.h"
#include "fsl_enet.h"
#include "fsl_ewm.h"
#include "fsl_flexcan.h"
#include "fsl_flexio.h"
#include "fsl_flexio_i2c_master.h"
#include "fsl_flexio_i2s.h"
#include "fsl_flexio_i2s_edma.h"
#include "fsl_flexio_uart.h"
#include "fsl_flexio_uart_edma.h"
#include "fsl_flexram.h"
#include "fsl_flexspi.h"
#include "fsl_gpc.h"
#include "fsl_gpio.h"
#include "fsl_gpt.h"
#include "fsl_iomuxc.h"
#include "fsl_kpp.h"
#include "fsl_lpi2c.h"
#include "fsl_lpi2c_edma.h"
#include "fsl_lpspi.h"
#include "fsl_lpspi_edma.h"
#include "fsl_lpuart.h"
#include "fsl_lpuart_edma.h"
#include "fsl_pit.h"
#include "fsl_pmu.h"
#include "fsl_pwm.h"
#include "fsl_pxp.h"
#include "fsl_qtmr.h"
#include "fsl_rtwdog.h"
#include "fsl_sai.h"
#include "fsl_sai_edma.h"
#include "fsl_semc.h"
#include "fsl_snvs_hp.h"
#include "fsl_snvs_lp.h"
#include "fsl_spdif.h"
#include "fsl_src.h"
#include "fsl_trng.h"
#include "fsl_tsc.h"
#include "fsl_usdhc.h"
#include "fsl_wdog.h"
#include "fsl_xbara.h"
#include "fsl_xbarb.h"
//////////////////////////////////////////////////////////////////////////////////     
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK I.MX RT1052������
//ϵͳʱ�ӳ�ʼ��    
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2017/12/10
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved
//********************************************************************************
//�޸�˵��
//��
//////////////////////////////////////////////////////////////////////////////////  

//0,��֧��os
//1,֧��os
#define SYSTEM_SUPPORT_OS        0        //����ϵͳ�ļ����Ƿ�֧��OS    
///////////////////////////////////////////////////////////////////////////////////
//����һЩ���õ��������Ͷ̹ؼ��� 
typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef __IO uint32_t  vu32;
typedef __IO uint16_t vu16;
typedef __IO uint8_t  vu8;

//�ж��������
#define NVIC_PRIORITYGROUP_0       ((u8)0x7)                                                               
#define NVIC_PRIORITYGROUP_1    ((u8)0x6)
#define NVIC_PRIORITYGROUP_2       ((u8)0x5)
#define NVIC_PRIORITYGROUP_3       ((u8)0x4) 
#define NVIC_PRIORITYGROUP_4       ((u8)0x3) 

void MPU_Memory_Protection(void);    //����RT1052��MPU
void Sys_Soft_Reset(void);          //�����λRT1052
void RT1052_Clock_Init(void);         //����ϵͳʱ��
void RT1052_NVIC_SetPriorityGrouping(u8 grpx);
void RT1052_NVIC_SetPriority(IRQn_Type IRQn,u32 prep,u32 subp);
void MY_NVIC_SetVectorTable(u32 vtab,u32 offset);

//����Ϊ��ຯ��
void WFI_SET(void);        //ִ��WFIָ��
void INTX_DISABLE(void);//�ر������ж�
void INTX_ENABLE(void);    //���������ж�
void MSR_MSP(u32 addr);    //���ö�ջ��ַ 


#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif


/** 
  * @brief  GPIO Bit SET and Bit RESET enumeration 
  */ 
#ifndef  GPIO_PIN_RESET
#define  GPIO_PIN_RESET   0
#endif

#ifndef  GPIO_PIN_SET
#define  GPIO_PIN_SET     1
#endif

#endif

