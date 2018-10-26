#ifndef _I2C_H
#define _I2C_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////     
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK I.MX RT1052������
//I2C��������       
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2018/1/9
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved                                      
//////////////////////////////////////////////////////////////////////////////////     

extern lpi2c_master_config_t    lpi2c1_config;      //I2C1(����)���ýṹ��
extern lpi2c_master_handle_t    lpi2c1_handle;      //I2C1(����)���
extern volatile bool            lpi2c1_compflag;    //��ɱ�־
extern volatile bool            lpi2c1_nakflag;     //nak��־

void LPI2C1_Init(u32 baudrate);
#endif
