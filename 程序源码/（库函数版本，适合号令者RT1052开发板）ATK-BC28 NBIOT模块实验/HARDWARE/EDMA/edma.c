#include "edma.h"
#include "lpuart.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK I.MX RT1052开发板
//DMA驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2018/5/10
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

edma_handle_t lpuart1_txdmahandle;             //DMA句柄

//初始化DMA0通道1
void MYEDMA_Init(void)
{
    edma_config_t edma_lpuartconfig;
       
    //设置DMAMUX
    DMAMUX_Init(DMAMUX);                        //初始化DMAMUX0
    DMAMUX_SetSource(DMAMUX,1,kDmaRequestMuxLPUART1Tx); //DMA通道1映射到LPUART1 TX通道
    DMAMUX_EnableChannel(DMAMUX,1);
    
    //初始化EDMA通道0
    EDMA_GetDefaultConfig(&edma_lpuartconfig);      //先配置为默认值
    EDMA_Init(DMA0,&edma_lpuartconfig);             //初始化DMA
    EDMA_CreateHandle(&lpuart1_txdmahandle,DMA0,1);//创建DMA句柄,通道1
}

//DMA传输
//srcaddr:源地址
//destaddr:目的地址
//length：传输长度
void EDMA_Transmit(void *srcaddr, void *destaddr, u32 length)
{
    edma_transfer_config_t trans_config;
    
    //准备传输
    EDMA_PrepareTransfer(&trans_config,                 //传输配置结构体
                        srcaddr,                        //源地址
                        1,                              //源宽度：1个字节
                        destaddr,                       //目的地址
                        1,                              //目的宽度：1个字节
                        1,                              //每次请求传输的字节长度，1个字节
                        length,                         //传输总长度
                        kEDMA_MemoryToPeripheral);      //传输类型：内存到内存
    EDMA_SubmitTransfer(&lpuart1_txdmahandle,&trans_config);      //提交传输
    EDMA_StartTransfer(&lpuart1_txdmahandle);                     //开始传输

}
