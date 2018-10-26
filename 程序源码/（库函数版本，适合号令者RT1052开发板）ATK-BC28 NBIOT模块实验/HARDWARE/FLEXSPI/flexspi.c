#include "flexspi.h"
#include "lpuart.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK I.MX RT1052������
//FlexSPI��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2018/3/1
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

flexspi_config_t flexspi_norflash_config;   //FlexSPI Nor Flash���ýṹ��

__align(64) u8 s_nor_program_buffer[256];
__align(64) u8 s_nor_read_buffer[256];

//Flash�����ṹ��
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

//LUT��,FLASHΪW25Q64JV����Flash��ַΪ3�ֽڳ��ȡ�
const uint32_t FlexSPI_LUT[CUSTOM_LUT_LENGTH] = 
{
    //ͨ�ö�����-SDRģʽ��0X03Ϊ���0X18������ַΪ3�ֽ�
    [4 * NOR_CMD_LUT_SEQ_IDX_READ_NORMAL] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x03, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 0x18),
    [4 * NOR_CMD_LUT_SEQ_IDX_READ_NORMAL + 1] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0x04, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),

    //���ٶ�����-SDRģʽ��0X0BΪ���� 
    [4 * NOR_CMD_LUT_SEQ_IDX_READ_FAST] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x0B, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 0x18),
    [4 * NOR_CMD_LUT_SEQ_IDX_READ_FAST + 1] = FLEXSPI_LUT_SEQ(
        kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_1PAD, 0x08, kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0x04),

    //���߿��ٶ�����-SDRģʽ
    [4 * NOR_CMD_LUT_SEQ_IDX_READ_FAST_QUAD] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x6B, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 0x18),
    [4 * NOR_CMD_LUT_SEQ_IDX_READ_FAST_QUAD + 1] = FLEXSPI_LUT_SEQ(
        kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_4PAD, 0x08, kFLEXSPI_Command_READ_SDR, kFLEXSPI_4PAD, 0x04),

    //дʹ��: ����Ϊ0X06
    [4 * NOR_CMD_LUT_SEQ_IDX_WRITEENABLE] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x06, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),

    //��������������0X20
    [4 * NOR_CMD_LUT_SEQ_IDX_ERASESECTOR] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0X20, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 0x18),

    //ҳ��� - ����ģʽ 0x2Ϊ����ҳ������0X18��ʾFLASH��ַ��3�ֽڵ�
    [4 * NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_SINGLE] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x02, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 0x18),
    [4 * NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_SINGLE + 1] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_1PAD, 0x04, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),

    //ҳ��� - ����ģʽ 0x32Ϊ����ҳ�������
    [4 * NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_QUAD] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x32, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 0x18),
    [4 * NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_QUAD + 1] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_4PAD, 0x04, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),

    //����ƷID������0XAB 
    [4 * NOR_CMD_LUT_SEQ_IDX_READID] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0xAB, kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_1PAD, 0x18),
    [4 * NOR_CMD_LUT_SEQ_IDX_READID + 1] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0x04, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),

    //д״̬�Ĵ���1������0X01 
    [4 * NOR_CMD_LUT_SEQ_IDX_WRITESTATUSREG1] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x01, kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_1PAD, 0x04),

    //��ȡ״̬�Ĵ���1������0X05
    [4 * NOR_CMD_LUT_SEQ_IDX_READSTATUSREG1] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x05, kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0x04),
            
    // ������ID������0X90/
    [4 * NOR_CMD_LUT_SEQ_IDX_READMANUID] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x90, kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_1PAD, 0x18),
    [4 * NOR_CMD_LUT_SEQ_IDX_READMANUID + 1] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0x04, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),
                        
    //��ȡ״̬�Ĵ���2
    [4 * NOR_CMD_LUT_SEQ_IDX_READSTATUSREG2] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x35, kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0x04),
            
    //д״̬�Ĵ���2
    [4 * NOR_CMD_LUT_SEQ_IDX_WRITESTATUSREG2] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x31, kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_1PAD, 0x04),
            
};

//SPI Flashдʹ��
void FlexSPI_FlashWrite_Enable(void)
{
    flexspi_transfer_t FlashTransfer;
    
    FlashTransfer.deviceAddress=0;                          //��ַ
    FlashTransfer.port=kFLEXSPI_PortA1;                     //�˿�
    FlashTransfer.cmdType=kFLEXSPI_Command;                 //��������:��������
    FlashTransfer.SeqNumber=1;                              //���
    FlashTransfer.seqIndex=NOR_CMD_LUT_SEQ_IDX_WRITEENABLE; //LUT������������
    FLEXSPI_TransferBlocking(FLEXSPI,&FlashTransfer);
}

//��ȡ״̬�Ĵ���
//regno:Ҫ��ȡ�ļĴ������
//����ֵ����ȡ����ֵ
u8 FlexSPI_FlashRead_SR(u8 regno)
{
    u32 data;
    u8 index=0;
    flexspi_transfer_t FlashTransfer;
    
    if(regno==1) index=NOR_CMD_LUT_SEQ_IDX_READSTATUSREG1;
    else if(regno==2) index=NOR_CMD_LUT_SEQ_IDX_READSTATUSREG2;
    
    FlashTransfer.deviceAddress=0;                              //��ַ
    FlashTransfer.port=kFLEXSPI_PortA1;                         //�˿�
    FlashTransfer.cmdType=kFLEXSPI_Read;                        //�������ͣ�������
    FlashTransfer.SeqNumber=1;                                  //���     
    FlashTransfer.seqIndex=index;                               //LUT������������
    FlashTransfer.data=&data;                                   //����������
    FlashTransfer.dataSize=1;                                   //�������ݳ���
    FLEXSPI_TransferBlocking(FLEXSPI,&FlashTransfer);

    return (u8)data;  
}

//д״̬�Ĵ���
//regno: Ҫд�ļĴ���
//regdata��Ҫд�������ֵ
void FlexSPI_FlashWrite_SR(u8 regno,u8 regdata)
{
    u8 index=0;
    u32 value;
    flexspi_transfer_t FlashTransfer;
    
    if(regno==1) index=NOR_CMD_LUT_SEQ_IDX_READSTATUSREG1;
    else if(regno==2) index=NOR_CMD_LUT_SEQ_IDX_READSTATUSREG2;
    
    value=regdata;
    FlashTransfer.deviceAddress=0;                          //��ַ
    FlashTransfer.port=kFLEXSPI_PortA1;                     //�˿�
    FlashTransfer.cmdType=kFLEXSPI_Write;                   //�������ͣ�д����  
    FlashTransfer.SeqNumber=1;                              //���
    FlashTransfer.seqIndex=index;                           //LUT������������
    FlashTransfer.data=&value;                              //Ҫд������
    FlashTransfer.dataSize=1;                               //���ݳ���
    FLEXSPI_TransferBlocking(FLEXSPI,&FlashTransfer);
}

//�ȴ�����
void FlexSPI_FlashWait_Busy(void)
{
    while((FlexSPI_FlashRead_SR(1)&0x01)==0x01);   // �ȴ�BUSYλ���
}

//ʹ��4��QPIģʽ
void FlexSPI_QPI_Enable(void)
{
    u8 regdata=0;
    
    regdata=FlexSPI_FlashRead_SR(2);    //��ȡ״̬�Ĵ���2
    if((regdata&0x02)==0)                 //�ж��Ƿ���4��ģʽ
    {
        regdata|=1<<1;  //ʹ��QEλ 
        FlexSPI_FlashWrite_SR(2,regdata);        
    }
    FlexSPI_FlashWait_Busy();
}

//��ȡFlash��ƷID
//����ֵ����ƷID
u8 FlexSPI_FlashID_Get(void)
{
    u32 data;
    flexspi_transfer_t FlashTransfer;
    
    FlashTransfer.deviceAddress=0;                      //��ַ
    FlashTransfer.port=kFLEXSPI_PortA1;                 //�˿�
    FlashTransfer.cmdType=kFLEXSPI_Read;                //�������ͣ�������
    FlashTransfer.SeqNumber=1;                          //���
    FlashTransfer.seqIndex=NOR_CMD_LUT_SEQ_IDX_READID;  //LUT������������
    FlashTransfer.data=&data;                           //���ݻ�����
    FlashTransfer.dataSize=1;                           //���ݳ���
    FLEXSPI_TransferBlocking(FLEXSPI,&FlashTransfer);

    return (u8)data;
}

//����ָ��������
//sectorno:Ҫ������������ţ�����W25Q64������
//        һ��sector=4K��С��һ����8*1024*1024/4096=2048
//����ֵ��0 �ɹ�������ֵ ʧ��
u8 FlexSPI_FlashErase_Sector(u32 sectorno)
{
    u8 ret=0;
    flexspi_transfer_t FlashTransfer;

    FlexSPI_FlashWrite_Enable();                            //дʹ��
    FlexSPI_FlashWait_Busy();                               //�ȴ�Flash����
    
    FlashTransfer.deviceAddress=sectorno*SECTOR_SIZE;       //��ַ
    FlashTransfer.port=kFLEXSPI_PortA1;                     //�˿�
    FlashTransfer.cmdType=kFLEXSPI_Command;                 //�������ͣ�������
    FlashTransfer.SeqNumber=1;                              //���
    FlashTransfer.seqIndex=NOR_CMD_LUT_SEQ_IDX_ERASESECTOR; //LUT������������
    if(FLEXSPI_TransferBlocking(FLEXSPI, &FlashTransfer)!=kStatus_Success) ret=1;

    FlexSPI_FlashWait_Busy();                               //�ȴ�Flash����
    return ret;
}

//ҳ��̣���ָ����ַ��ʼд�����256�ֽڵ����ݣ�ҳ��СΪ256B
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(���32bit)
//NumByteToWrite:Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!
//����ֵ��0 �ɹ�������ֵ ʧ�� 
u8 FlexSPI_FlashPage_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
    u8 ret=0;
    flexspi_transfer_t FlashTransfer;
    
    FlexSPI_FlashWrite_Enable();                            //дʹ��
    FlexSPI_FlashWait_Busy();                               //�ȴ�Flash����
    
    FlashTransfer.deviceAddress=WriteAddr;                  //��ַ
    FlashTransfer.port=kFLEXSPI_PortA1;                     //�˿�
    FlashTransfer.cmdType=kFLEXSPI_Write;                   //�������ͣ�д����
    FlashTransfer.SeqNumber=1;                              //���
    FlashTransfer.seqIndex=NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_QUAD;    //LUT������������
    FlashTransfer.data=(u32 *)pBuffer;                      //Ҫд�������
    FlashTransfer.dataSize=NumByteToWrite;                  //���ݳ���
    if(FLEXSPI_TransferBlocking(FLEXSPI,&FlashTransfer)!=kStatus_Success) ret=1;

    FlexSPI_FlashWait_Busy();           //�ȴ�Flash����
    return ret;  
}

//��FlexSPI Flash�ж�ȡ����
//pBuffer:���ݴ洢��
//ReadAddr:��ʼ��ȡ�ĵ�ַ(���32bit)
//NumByteToRead:Ҫ��ȡ���ֽ���(���65535)
void FlexSPI_FlashRead(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)  
{
    u32 index=0;
    u32 startaddr=0;
    
    startaddr=FlexSPI_AMBA_BASE+ReadAddr;

    //��λAHB��������һ��Ҫ��AHB����������������������ȡ����
    FLEXSPI_SoftwareReset(FLEXSPI);     
    for(index=0;index<NumByteToRead;index++)
    {
        pBuffer[index]=*(u8*)(startaddr+index);
    }
}

//��һ������д������
//����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��! 
//��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
//pBuffer:���ݴ洢��
//setcorno:Ҫд�������(���32bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)
//CHECK OK
void FlexSPI_Sector_Write(u8* pBuffer,u32 setcorno)   
{ 			
    u8 i=0;
    u8 pagenum=SECTOR_SIZE/FLASH_PAGE_SIZE; //һ������16��page
    u32 WriteAddr=setcorno*SECTOR_SIZE;
    
    for(i=0;i<pagenum;i++)
    {
        FlexSPI_FlashPage_Write(pBuffer,WriteAddr,FLASH_PAGE_SIZE); 
        pBuffer+=FLASH_PAGE_SIZE;
        WriteAddr+=FLASH_PAGE_SIZE;
    }		
} 

//�޼���дFlexSPI FLASH 
//����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
//�����Զ���ҳ���� 
//��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(���32bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)
//CHECK OK
void FlexSPI_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 			 		 
	u16 pageremain;	   
	pageremain=256-WriteAddr%256; //��ҳʣ����ֽ���	
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//������256���ֽ�
	while(1)
	{	   
		FlexSPI_FlashPage_Write(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;//д�������
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //��ȥ�Ѿ�д���˵��ֽ���
			if(NumByteToWrite>256)pageremain=256; //һ�ο���д��256���ֽ�
			else pageremain=NumByteToWrite; 	  //����256���ֽ���
		}
	}   
} 

//дSPI FLASH  
//��ָ����ַ��ʼд��ָ�����ȵ�����
//�ú�������������!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(���32bit)						
//NumByteToWrite:Ҫд����ֽ���(���65535)   
__align(64) u8 FLEXSPI_FLASH_BUFFER[4096];	
void FlexSPI_FlashWrite(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 
	u32 secpos=0;
	u16 secoff=0;
	u16 secremain=0;	   
 	u16 i=0;    
	u8 *FLEXSPI_BUF;
    
   	FLEXSPI_BUF=FLEXSPI_FLASH_BUFFER;	     
 	secpos=WriteAddr/4096;//������ַ  
	secoff=WriteAddr%4096;//�������ڵ�ƫ��
	secremain=4096-secoff;//����ʣ��ռ��С

 	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//������4096���ֽ�
    while(1) 
	{	
		FlexSPI_FlashRead(FLEXSPI_BUF,secpos*4096,4096);//������������������
        FlexSPI_FlashErase_Sector(secpos);              //�����������
        for(i=0;i<secremain;i++)	                    //����
        {
            FLEXSPI_BUF[i+secoff]=pBuffer[i];	  
        }
        //FlexSPI_Sector_Write(FLEXSPI_BUF,secpos); //д����������  
        FlexSPI_Write_NoCheck(FLEXSPI_BUF,secpos*4096,4096);//д����������  
		if(NumByteToWrite==secremain)break;//д�������
		else//д��δ����
		{
			secpos++;//������ַ��1
			secoff=0;//ƫ��λ��Ϊ0 	 

		   	pBuffer+=secremain;  //ָ��ƫ��
			WriteAddr+=secremain;//д��ַƫ��	   
		   	NumByteToWrite-=secremain;				//�ֽ����ݼ�
			if(NumByteToWrite>4096)secremain=4096;	//��һ����������д����
			else secremain=NumByteToWrite;			//��һ����������д����
		}	 
	};	 
}

//��ʼ��FlexSPI Nor Flash
void FlexSPI_FlashInit(void)
{     
    //��ʼ��FlexSPIʱ�ӣ�W25Q128JV��߿��Ե�133MHz
    CLOCK_InitUsb1Pfd(kCLOCK_Pfd0,24);      //����PLL3��PFD0Ϊ480Mhz*18/24=360MHz 
    CLOCK_SetMux(kCLOCK_FlexspiMux,0x3);    //FlexSPIʱ��Դѡ��PLL3 PFD0 
    CLOCK_SetDiv(kCLOCK_FlexspiDiv,2);      //FlexSPIʱ������Ϊ360/(2+1)=120Mhz 
    
    //����FlexSPI��ʹ�õ�IO�ĸ��ù���
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_06_FLEXSPIA_SS0_B,0); 
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_07_FLEXSPIA_SCLK,0);            
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_08_FLEXSPIA_DATA00,0); 
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_09_FLEXSPIA_DATA01,0); 
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_10_FLEXSPIA_DATA02,0); 
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_11_FLEXSPIA_DATA03,0);    

    //��ת���ٶ�,��������ΪR0/6,�ٶ�Ϊ200Mhz���رտ�·���ܣ�ʹ��pull/keepr
	//ѡ��keeper���ܣ�����100K Ohm���ر�Hyst
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_06_FLEXSPIA_SS0_B,0x10F1); 
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_07_FLEXSPIA_SCLK,0x10F1);       
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_08_FLEXSPIA_DATA00,0x10F1); 
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_09_FLEXSPIA_DATA01,0x10F1); 
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_10_FLEXSPIA_DATA02,0x10F1); 
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_11_FLEXSPIA_DATA03,0x10F1);    
    
    SCB_DisableDCache();                    //�ر�Dcache
    
    FLEXSPI_GetDefaultConfig(&flexspi_norflash_config);         //�Ȼ�ȡĬ������
    flexspi_norflash_config.ahbConfig.enableAHBPrefetch=true;   //ʹ��AHBԤȡ
    FLEXSPI_Init(FLEXSPI,&flexspi_norflash_config);             //��ʼ��FlexSPI
    FLEXSPI_SetFlashConfig(FLEXSPI,&NorFlash_config,kFLEXSPI_PortA1); //����Flash
    FLEXSPI_UpdateLUT(FLEXSPI, 0, FlexSPI_LUT, CUSTOM_LUT_LENGTH);    //LUT��
    FlexSPI_FlashWrite_Enable();        //дʹ��
    FlexSPI_QPI_Enable();               //ʹ������ģʽ                    
}
