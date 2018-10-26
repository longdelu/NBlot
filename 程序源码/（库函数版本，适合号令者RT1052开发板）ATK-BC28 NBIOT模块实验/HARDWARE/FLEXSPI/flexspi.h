#ifndef _FLEXSPI_H
#define _FLEXSPI_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK I.MX RT1052开发板
//FlexSPI驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2018/3/1
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

#define FLEXSPI_FLASH_SIZE      0x2000              //Flash大小为8MB
#define FLASH_PAGE_SIZE         256                 //编程页大小：256字节
#define SECTOR_SIZE             0x1000              //扇区大小4096
#define FLEXSPI_FLASHID         0X16                //Flash ID

//命令序列
#define NOR_CMD_LUT_SEQ_IDX_READ_NORMAL 0
#define NOR_CMD_LUT_SEQ_IDX_READ_FAST 1
#define NOR_CMD_LUT_SEQ_IDX_READ_FAST_QUAD 2
#define NOR_CMD_LUT_SEQ_IDX_READSTATUS 3
#define NOR_CMD_LUT_SEQ_IDX_WRITEENABLE 4
#define NOR_CMD_LUT_SEQ_IDX_ERASESECTOR 5
#define NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_SINGLE 6
#define NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_QUAD 7
#define NOR_CMD_LUT_SEQ_IDX_READID 8
#define NOR_CMD_LUT_SEQ_IDX_WRITESTATUSREG1 9
#define NOR_CMD_LUT_SEQ_IDX_WRITESTATUSREG2 10
#define NOR_CMD_LUT_SEQ_IDX_EXITQPI 11
#define NOR_CMD_LUT_SEQ_IDX_READSTATUSREG1 12
#define NOR_CMD_LUT_SEQ_IDX_READMANUID 13
#define NOR_CMD_LUT_SEQ_IDX_READSTATUSREG2 14

#define CUSTOM_LUT_LENGTH 60

void FlexSPI_FlashInit(void);
void FlexSPI_FlashWrite_Enable(void);
u8 FlexSPI_FlashRead_SR(u8 regno);
void FlexSPI_FlashWrite_SR(u8 regno,u8 regdata);
void FlexSPI_FlashWait_Busy(void);
void FlexSPI_QPI_Enable(void);
u8 FlexSPI_FlashID_Get(void);
u8 FlexSPI_FlashErase_Sector(u32 sectorno);
u8 FlexSPI_FlashPage_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);
void FlexSPI_FlashRead(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead);
void FlexSPI_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);
void FlexSPI_FlashWrite(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);
void FlexSPI_Sector_Write(u8* pBuffer,u32 setcorno);

#endif
