#include "sim7020.h"
#include "delay.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F429开发板
//串口1初始化		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2015/9/7
//版本：V1.5
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//********************************************************************************


//定义全局缓存变量
#define NB_UART_RECE_BUF_MAX_LEN    512
#define NB_UART_SEND_BUF_MAX_LEN    512

//接收缓存空间
struct sim7020_recv
{
  char      buf[NB_UART_RECE_BUF_MAX_LEN];    //接收数据缓冲区
  uint16_t  len;                              //有效数据长度
}sim7020_recv_t;

//接收缓存空间
struct sim7020_send
{
  char      buf[NB_UART_SEND_BUF_MAX_LEN];    //发送数据缓冲区
  uint16_t  len;                              //有效数据长度
}sim7020_send_t;



void sim7020_sm_event (int sim7020_event)
{
    
    switch(sim7020_event) {
    case 0: 

        break;


    case 1:

        break; 
    
   default:
        
        break;
   }    
    
}




