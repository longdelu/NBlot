#ifndef __DS18B20_H
#define __DS18B20_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////     
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F7������
//DHT11��������       
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/12/30
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved                                      
//////////////////////////////////////////////////////////////////////////////////  

/**
 * \brief inline ����
 */
#if   defined (__CC_ARM)     

typedef	 long		off_t;	

#define am_inline   __inline             /**< inline for ARMCC    */
#define am_static_inline  static  __inline  
    
#elif defined (__ICCARM__)
#define am_inline   inline               /**< inline for IAR      */
#define am_static_inline  static inline 

#elif defined   (__GNUC__)
#define am_inline   inline               /**< inline for GNU      */
#define am_static_inline  static inline  

#elif defined   (__TASKING__)
#define am_inline   inline               /**< inline for TASKING  */
#define am_static_inline  static inline  

#endif /* __CC_ARM */

//IO��������
#define DHT11_IO_IN()  {GPIOB->MODER&=~(3<<(12*2));GPIOB->MODER|=0<<(12*2);}    //PB12����ģʽ
#define DHT11_IO_OUT() {GPIOB->MODER&=~(3<<(12*2));GPIOB->MODER|=1<<(12*2);}     //PB12���ģʽ
 
////IO��������                                               
#define    DHT11_DQ_OUT(n) (n?HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_RESET)) //���ݶ˿�    PB12
#define    DHT11_DQ_IN     HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_12) //���ݶ˿�    PB12
       
u8 DHT11_Init(void);//��ʼ��DHT11
u8 DHT11_Read_Data(u8 *temp,u8 *humi);//��ȡ��ʪ��
u8 DHT11_Read_Byte(void);//����һ���ֽ�
u8 DHT11_Read_Bit(void);//����һ��λ
u8 DHT11_Check(void);//����Ƿ����DHT11
void DHT11_Rst(void);//��λDHT11  




#endif
