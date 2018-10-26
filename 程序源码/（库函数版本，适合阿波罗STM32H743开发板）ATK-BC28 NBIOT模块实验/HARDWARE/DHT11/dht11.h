#ifndef __DS18B20_H
#define __DS18B20_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////     
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F7开发板
//DHT11驱动代码       
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/12/30
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved                                      
//////////////////////////////////////////////////////////////////////////////////  

/**
 * \brief inline 定义
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

//IO方向设置
#define DHT11_IO_IN()  {GPIOB->MODER&=~(3<<(12*2));GPIOB->MODER|=0<<(12*2);}    //PB12输入模式
#define DHT11_IO_OUT() {GPIOB->MODER&=~(3<<(12*2));GPIOB->MODER|=1<<(12*2);}     //PB12输出模式
 
////IO操作函数                                               
#define    DHT11_DQ_OUT(n) (n?HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_RESET)) //数据端口    PB12
#define    DHT11_DQ_IN     HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_12) //数据端口    PB12
       
u8 DHT11_Init(void);//初始化DHT11
u8 DHT11_Read_Data(u8 *temp,u8 *humi);//读取温湿度
u8 DHT11_Read_Byte(void);//读出一个字节
u8 DHT11_Read_Bit(void);//读出一个位
u8 DHT11_Check(void);//检测是否存在DHT11
void DHT11_Rst(void);//复位DHT11  




#endif
