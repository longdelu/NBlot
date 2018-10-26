#include "flexspi.h"
#include "lpuart.h"
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

flexspi_config_t flexspi_norflash_config;   //FlexSPI Nor Flash配置结构体

__align(64) u8 s_nor_program_buffer[256];
__align(64) u8 s_nor_read_buffer[256];

//Flash参数结构体
flexspi_device_config_t NorFlash_config = {
    .flexspiRootClk=120000000,
    .flashSize=FLEXSPI_FLASH_SIZE,
    .CSIntervalUnit=kFLEXSPI_CsIntervalUnit1SckCycle,
    .CSInterval=10,
    .CSHoldTime=10,
    .CSSetupTime=10,
    .dataValidTime=0,
    .columnspace=0,
    .enableWordAddress=0,
    .AWRSeqIndex=0,
    .AWRSeqNumber=0,
    .ARDSeqIndex=NOR_CMD_LUT_SEQ_IDX_READ_FAST_QUAD,
    .ARDSeqNumber=1,
    .AHBWriteWaitUnit=kFLEXSPI_AhbWriteWaitUnit2AhbCycle,
    .AHBWriteWaitInterval=0,
};

//LUT表,FLASH为W25Q64JV，此Flash地址为3字节长度。
const uint32_t FlexSPI_LUT[CUSTOM_LUT_LENGTH] = 
{
    //通用读命令-SDR模式，0X03为命令，0X18表明地址为3字节
    [4 * NOR_CMD_LUT_SEQ_IDX_READ_NORMAL] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x03, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 0x18),
    [4 * NOR_CMD_LUT_SEQ_IDX_READ_NORMAL + 1] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0x04, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),

    //快速读命令-SDR模式，0X0B为命令 
    [4 * NOR_CMD_LUT_SEQ_IDX_READ_FAST] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x0B, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 0x18),
    [4 * NOR_CMD_LUT_SEQ_IDX_READ_FAST + 1] = FLEXSPI_LUT_SEQ(
        kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_1PAD, 0x08, kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0x04),

    //四线快速读命令-SDR模式
    [4 * NOR_CMD_LUT_SEQ_IDX_READ_FAST_QUAD] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x6B, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 0x18),
    [4 * NOR_CMD_LUT_SEQ_IDX_READ_FAST_QUAD + 1] = FLEXSPI_LUT_SEQ(
        kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_4PAD, 0x08, kFLEXSPI_Command_READ_SDR, kFLEXSPI_4PAD, 0x04),

    //写使能: 命令为0X06
    [4 * NOR_CMD_LUT_SEQ_IDX_WRITEENABLE] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x06, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),

    //擦出扇区，命令0X20
    [4 * NOR_CMD_LUT_SEQ_IDX_ERASESECTOR] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0X20, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 0x18),

    //页编程 - 单线模式 0x2为单线页编程命令，0X18表示FLASH地址是3字节的
    [4 * NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_SINGLE] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x02, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 0x18),
    [4 * NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_SINGLE + 1] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_1PAD, 0x04, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),

    //页编程 - 四线模式 0x32为四线页编程命令
    [4 * NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_QUAD] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x32, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 0x18),
    [4 * NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_QUAD + 1] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_4PAD, 0x04, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),

    //读产品ID，命令0XAB 
    [4 * NOR_CMD_LUT_SEQ_IDX_READID] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0xAB, kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_1PAD, 0x18),
    [4 * NOR_CMD_LUT_SEQ_IDX_READID + 1] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0x04, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),

    //写状态寄存器1，命令0X01 
    [4 * NOR_CMD_LUT_SEQ_IDX_WRITESTATUSREG1] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x01, kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_1PAD, 0x04),

    //读取状态寄存器1，命令0X05
    [4 * NOR_CMD_LUT_SEQ_IDX_READSTATUSREG1] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x05, kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0x04),
            
    // 读厂商ID，命令0X90/
    [4 * NOR_CMD_LUT_SEQ_IDX_READMANUID] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x90, kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_1PAD, 0x18),
    [4 * NOR_CMD_LUT_SEQ_IDX_READMANUID + 1] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0x04, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),
                        
    //读取状态寄存器2
    [4 * NOR_CMD_LUT_SEQ_IDX_READSTATUSREG2] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x35, kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0x04),
            
    //写状态寄存器2
    [4 * NOR_CMD_LUT_SEQ_IDX_WRITESTATUSREG2] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x31, kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_1PAD, 0x04),
            
};

//SPI Flash写使能
void FlexSPI_FlashWrite_Enable(void)
{
    flexspi_transfer_t FlashTransfer;
    
    FlashTransfer.deviceAddress=0;                          //地址
    FlashTransfer.port=kFLEXSPI_PortA1;                     //端口
    FlashTransfer.cmdType=kFLEXSPI_Command;                 //操作类型:发送命令
    FlashTransfer.SeqNumber=1;                              //序号
    FlashTransfer.seqIndex=NOR_CMD_LUT_SEQ_IDX_WRITEENABLE; //LUT表中命令索引
    FLEXSPI_TransferBlocking(FLEXSPI,&FlashTransfer);
}

//读取状态寄存器
//regno:要读取的寄存器编号
//返回值：读取到的值
u8 FlexSPI_FlashRead_SR(u8 regno)
{
    u32 data;
    u8 index=0;
    flexspi_transfer_t FlashTransfer;
    
    if(regno==1) index=NOR_CMD_LUT_SEQ_IDX_READSTATUSREG1;
    else if(regno==2) index=NOR_CMD_LUT_SEQ_IDX_READSTATUSREG2;
    
    FlashTransfer.deviceAddress=0;                              //地址
    FlashTransfer.port=kFLEXSPI_PortA1;                         //端口
    FlashTransfer.cmdType=kFLEXSPI_Read;                        //操作类型：读数据
    FlashTransfer.SeqNumber=1;                                  //序号     
    FlashTransfer.seqIndex=index;                               //LUT表中命令索引
    FlashTransfer.data=&data;                                   //读到的数据
    FlashTransfer.dataSize=1;                                   //读的数据长度
    FLEXSPI_TransferBlocking(FLEXSPI,&FlashTransfer);

    return (u8)data;  
}

//写状态寄存器
//regno: 要写的寄存器
//regdata：要写入的数据值
void FlexSPI_FlashWrite_SR(u8 regno,u8 regdata)
{
    u8 index=0;
    u32 value;
    flexspi_transfer_t FlashTransfer;
    
    if(regno==1) index=NOR_CMD_LUT_SEQ_IDX_READSTATUSREG1;
    else if(regno==2) index=NOR_CMD_LUT_SEQ_IDX_READSTATUSREG2;
    
    value=regdata;
    FlashTransfer.deviceAddress=0;                          //地址
    FlashTransfer.port=kFLEXSPI_PortA1;                     //端口
    FlashTransfer.cmdType=kFLEXSPI_Write;                   //操作类型：写数据  
    FlashTransfer.SeqNumber=1;                              //序号
    FlashTransfer.seqIndex=index;                           //LUT表中命令索引
    FlashTransfer.data=&value;                              //要写的数据
    FlashTransfer.dataSize=1;                               //数据长度
    FLEXSPI_TransferBlocking(FLEXSPI,&FlashTransfer);
}

//等待空闲
void FlexSPI_FlashWait_Busy(void)
{
    while((FlexSPI_FlashRead_SR(1)&0x01)==0x01);   // 等待BUSY位清空
}

//使能4线QPI模式
void FlexSPI_QPI_Enable(void)
{
    u8 regdata=0;
    
    regdata=FlexSPI_FlashRead_SR(2);    //读取状态寄存器2
    if((regdata&0x02)==0)                 //判断是否是4线模式
    {
        regdata|=1<<1;  //使能QE位 
        FlexSPI_FlashWrite_SR(2,regdata);        
    }
    FlexSPI_FlashWait_Busy();
}

//读取Flash产品ID
//返回值：产品ID
u8 FlexSPI_FlashID_Get(void)
{
    u32 data;
    flexspi_transfer_t FlashTransfer;
    
    FlashTransfer.deviceAddress=0;                      //地址
    FlashTransfer.port=kFLEXSPI_PortA1;                 //端口
    FlashTransfer.cmdType=kFLEXSPI_Read;                //操作类型，读数据
    FlashTransfer.SeqNumber=1;                          //序号
    FlashTransfer.seqIndex=NOR_CMD_LUT_SEQ_IDX_READID;  //LUT表中命令索引
    FlashTransfer.data=&data;                           //数据缓冲区
    FlashTransfer.dataSize=1;                           //数据长度
    FLEXSPI_TransferBlocking(FLEXSPI,&FlashTransfer);

    return (u8)data;
}

//擦除指定的扇区
//sectorno:要擦除的扇区编号，对于W25Q64来讲，
//        一个sector=4K大小，一共有8*1024*1024/4096=2048
//返回值：0 成功，其他值 失败
u8 FlexSPI_FlashErase_Sector(u32 sectorno)
{
    u8 ret=0;
    flexspi_transfer_t FlashTransfer;

    FlexSPI_FlashWrite_Enable();                            //写使能
    FlexSPI_FlashWait_Busy();                               //等待Flash空闲
    
    FlashTransfer.deviceAddress=sectorno*SECTOR_SIZE;       //地址
    FlashTransfer.port=kFLEXSPI_PortA1;                     //端口
    FlashTransfer.cmdType=kFLEXSPI_Command;                 //操作类型：发命令
    FlashTransfer.SeqNumber=1;                              //序号
    FlashTransfer.seqIndex=NOR_CMD_LUT_SEQ_IDX_ERASESECTOR; //LUT表中命令索引
    if(FLEXSPI_TransferBlocking(FLEXSPI, &FlashTransfer)!=kStatus_Success) ret=1;

    FlexSPI_FlashWait_Busy();                               //等待Flash空闲
    return ret;
}

//页编程，在指定地址开始写入最大256字节的数据，页大小为256B
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(最大32bit)
//NumByteToWrite:要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!
//返回值：0 成功，其他值 失败 
u8 FlexSPI_FlashPage_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
    u8 ret=0;
    flexspi_transfer_t FlashTransfer;
    
    FlexSPI_FlashWrite_Enable();                            //写使能
    FlexSPI_FlashWait_Busy();                               //等待Flash空闲
    
    FlashTransfer.deviceAddress=WriteAddr;                  //地址
    FlashTransfer.port=kFLEXSPI_PortA1;                     //端口
    FlashTransfer.cmdType=kFLEXSPI_Write;                   //操作类型：写数据
    FlashTransfer.SeqNumber=1;                              //序号
    FlashTransfer.seqIndex=NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_QUAD;    //LUT表中命令索引
    FlashTransfer.data=(u32 *)pBuffer;                      //要写入的数据
    FlashTransfer.dataSize=NumByteToWrite;                  //数据长度
    if(FLEXSPI_TransferBlocking(FLEXSPI,&FlashTransfer)!=kStatus_Success) ret=1;

    FlexSPI_FlashWait_Busy();           //等待Flash空闲
    return ret;  
}

//从FlexSPI Flash中读取数据
//pBuffer:数据存储区
//ReadAddr:开始读取的地址(最大32bit)
//NumByteToRead:要读取的字节数(最大65535)
void FlexSPI_FlashRead(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)  
{
    u32 index=0;
    u32 startaddr=0;
    
    startaddr=FlexSPI_AMBA_BASE+ReadAddr;

    //复位AHB缓冲区，一定要复AHB缓冲区，否则数据连续读取出错！
    FLEXSPI_SoftwareReset(FLEXSPI);     
    for(index=0;index<NumByteToRead;index++)
    {
        pBuffer[index]=*(u8*)(startaddr+index);
    }
}

//向一个扇区写入数据
//必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败! 
//在指定地址开始写入指定长度的数据,但是要确保地址不越界!
//pBuffer:数据存储区
//setcorno:要写入的扇区(最大32bit)
//NumByteToWrite:要写入的字节数(最大65535)
//CHECK OK
void FlexSPI_Sector_Write(u8* pBuffer,u32 setcorno)   
{ 			
    u8 i=0;
    u8 pagenum=SECTOR_SIZE/FLASH_PAGE_SIZE; //一个扇区16个page
    u32 WriteAddr=setcorno*SECTOR_SIZE;
    
    for(i=0;i<pagenum;i++)
    {
        FlexSPI_FlashPage_Write(pBuffer,WriteAddr,FLASH_PAGE_SIZE); 
        pBuffer+=FLASH_PAGE_SIZE;
        WriteAddr+=FLASH_PAGE_SIZE;
    }		
} 

//无检验写FlexSPI FLASH 
//必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
//具有自动换页功能 
//在指定地址开始写入指定长度的数据,但是要确保地址不越界!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(最大32bit)
//NumByteToWrite:要写入的字节数(最大65535)
//CHECK OK
void FlexSPI_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 			 		 
	u16 pageremain;	   
	pageremain=256-WriteAddr%256; //单页剩余的字节数	
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//不大于256个字节
	while(1)
	{	   
		FlexSPI_FlashPage_Write(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;//写入结束了
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //减去已经写入了的字节数
			if(NumByteToWrite>256)pageremain=256; //一次可以写入256个字节
			else pageremain=NumByteToWrite; 	  //不够256个字节了
		}
	}   
} 

//写SPI FLASH  
//在指定地址开始写入指定长度的数据
//该函数带擦除操作!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(最大32bit)						
//NumByteToWrite:要写入的字节数(最大65535)   
__align(64) u8 FLEXSPI_FLASH_BUFFER[4096];	
void FlexSPI_FlashWrite(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 
	u32 secpos=0;
	u16 secoff=0;
	u16 secremain=0;	   
 	u16 i=0;    
	u8 *FLEXSPI_BUF;
    
   	FLEXSPI_BUF=FLEXSPI_FLASH_BUFFER;	     
 	secpos=WriteAddr/4096;//扇区地址  
	secoff=WriteAddr%4096;//在扇区内的偏移
	secremain=4096-secoff;//扇区剩余空间大小

 	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//不大于4096个字节
    while(1) 
	{	
		FlexSPI_FlashRead(FLEXSPI_BUF,secpos*4096,4096);//读出整个扇区的内容
        FlexSPI_FlashErase_Sector(secpos);              //擦除这个扇区
        for(i=0;i<secremain;i++)	                    //复制
        {
            FLEXSPI_BUF[i+secoff]=pBuffer[i];	  
        }
        //FlexSPI_Sector_Write(FLEXSPI_BUF,secpos); //写入整个扇区  
        FlexSPI_Write_NoCheck(FLEXSPI_BUF,secpos*4096,4096);//写入整个扇区  
		if(NumByteToWrite==secremain)break;//写入结束了
		else//写入未结束
		{
			secpos++;//扇区地址增1
			secoff=0;//偏移位置为0 	 

		   	pBuffer+=secremain;  //指针偏移
			WriteAddr+=secremain;//写地址偏移	   
		   	NumByteToWrite-=secremain;				//字节数递减
			if(NumByteToWrite>4096)secremain=4096;	//下一个扇区还是写不完
			else secremain=NumByteToWrite;			//下一个扇区可以写完了
		}	 
	};	 
}

//初始化FlexSPI Nor Flash
void FlexSPI_FlashInit(void)
{     
    //初始化FlexSPI时钟，W25Q128JV最高可以到133MHz
    CLOCK_InitUsb1Pfd(kCLOCK_Pfd0,24);      //设置PLL3的PFD0为480Mhz*18/24=360MHz 
    CLOCK_SetMux(kCLOCK_FlexspiMux,0x3);    //FlexSPI时钟源选择PLL3 PFD0 
    CLOCK_SetDiv(kCLOCK_FlexspiDiv,2);      //FlexSPI时钟设置为360/(2+1)=120Mhz 
    
    //配置FlexSPI所使用的IO的复用功能
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_06_FLEXSPIA_SS0_B,0); 
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_07_FLEXSPIA_SCLK,0);            
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_08_FLEXSPIA_DATA00,0); 
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_09_FLEXSPIA_DATA01,0); 
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_10_FLEXSPIA_DATA02,0); 
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_11_FLEXSPIA_DATA03,0);    

    //高转换速度,驱动能力为R0/6,速度为200Mhz，关闭开路功能，使能pull/keepr
	//选择keeper功能，下拉100K Ohm，关闭Hyst
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_06_FLEXSPIA_SS0_B,0x10F1); 
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_07_FLEXSPIA_SCLK,0x10F1);       
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_08_FLEXSPIA_DATA00,0x10F1); 
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_09_FLEXSPIA_DATA01,0x10F1); 
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_10_FLEXSPIA_DATA02,0x10F1); 
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_11_FLEXSPIA_DATA03,0x10F1);    
    
    SCB_DisableDCache();                    //关闭Dcache
    
    FLEXSPI_GetDefaultConfig(&flexspi_norflash_config);         //先获取默认配置
    flexspi_norflash_config.ahbConfig.enableAHBPrefetch=true;   //使能AHB预取
    FLEXSPI_Init(FLEXSPI,&flexspi_norflash_config);             //初始化FlexSPI
    FLEXSPI_SetFlashConfig(FLEXSPI,&NorFlash_config,kFLEXSPI_PortA1); //配置Flash
    FLEXSPI_UpdateLUT(FLEXSPI, 0, FlexSPI_LUT, CUSTOM_LUT_LENGTH);    //LUT表
    FlexSPI_FlashWrite_Enable();        //写使能
    FlexSPI_QPI_Enable();               //使能四线模式                    
}
