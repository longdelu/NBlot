#include "remote.h"
#include "delay.h"
#include "lpuart.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK 号令者RT1052开发板
//红外遥控解码驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2018/5/12
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	


//红外遥控初始化
//设置IO以及GPT2_CH1的输入捕获
//psc : 预分频器,0~4095
//最大计数时间=(2^32)*(psc+1)/PERCLK_CLK_ROOT=234562.5秒,基本上不可能溢出
void Remote_Init(void)
{
    gpt_config_t gpt2_onfig;
    
    //配置GPT2_CAP1相关IO(GPIO_EMC_41)的功能
	//高转换速度,驱动R0/6,速度为200Mhz，关闭开路功能，使能pull/keepr
	//选择pull功能，上拉22K Ohm 
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_41_GPT2_CAPTURE1,0);	  
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_41_GPT2_CAPTURE1,0xF0F1);
    
    GPT_GetDefaultConfig(&gpt2_onfig);	//先初始化GPT2为默认值
    gpt2_onfig.divider=74;	            //设置分频值,1Mhz计数频率
	gpt2_onfig.clockSource=kGPT_ClockSource_Periph;	//设置时钟源
	GPT_Init(GPT2,&gpt2_onfig);         //初始化GPT2
  
    //设置CAP2为上升沿捕获
    GPT_SetOutputCompareValue(GPT2,kGPT_OutputCompare_Channel1,10000);	    //设置比较计数值,10ms中断
    GPT_SetInputOperationMode(GPT2,kGPT_InputCapture_Channel1,kGPT_InputOperation_RiseEdge);
    GPT_EnableInterrupts(GPT2,kGPT_InputCapture1InterruptEnable);   //使能GPT2捕获中断
    GPT_EnableInterrupts(GPT2,kGPT_OutputCompare1InterruptEnable);			//使能GPT比较通道1中断
    GPT_StartTimer(GPT2);	            //开始定时器
    
    //中断设置
    RT1052_NVIC_SetPriority(GPT2_IRQn,7,0);//设置GPT2中断优先级
    EnableIRQ(GPT2_IRQn);	            //使能GPT2中断
}


//遥控器接收状态
//[7]:收到了引导码标志
//[6]:得到了一个按键的所有信息
//[5]:保留	
//[4]:标记上升沿是否已经被捕获								   
//[3:0]:溢出计时器
u8 	RmtSta=0;	  	  
u16 Dval;		//下降沿时计数器的值
u32 RmtRec=0;	//红外接收到的数据	   		    
u8  RmtCnt=0;	//按键按下的次数	 

void GPT2_IRQHandler(void)
{
    //OCR1中断
    if(GPT_GetStatusFlags(GPT2,kGPT_OutputCompare1Flag))
    {
 		if(RmtSta&0x80)                         //上次有数据被接收到了
		{	
			RmtSta&=~0X10;						//取消上升沿已经被捕获标记
			if((RmtSta&0X0F)==0X00)RmtSta|=1<<6;//标记已经完成一次按键的键值信息采集
			if((RmtSta&0X0F)<14)RmtSta++;
			else
			{
				RmtSta&=~(1<<7);                //清空引导标识
				RmtSta&=0XF0;	                //清空计数器	
			}						 	   	
		}
        GPT_ClearStatusFlags(GPT2,kGPT_OutputCompare1Flag);//清除中断标志位
    }
    
    if(GPT_GetStatusFlags(GPT2,kGPT_InputCapture1Flag))  //捕获1发生捕获事件    
    {
        GPT_StopTimer(GPT2);
        REMOTE_IO_GPIO();
        if(RDATA)                               //上升沿捕获
        {	
            GPT_SetInputOperationMode(GPT2,kGPT_InputCapture_Channel1,kGPT_InputOperation_FallEdge);
            RmtSta|=0X10;					    //标记上升沿已经被捕获
        }
        else //下降沿捕获
        {
            Dval=GPT_GetInputCaptureValue(GPT2,kGPT_InputCapture_Channel1);	//读取捕获值	
            //设置捕获上升沿 
            GPT_SetInputOperationMode(GPT2,kGPT_InputCapture_Channel1,kGPT_InputOperation_RiseEdge);
            if(RmtSta&0X10)					    //完成一次高电平捕获 
            {
                if(RmtSta&0X80)                 //接收到了引导码
                {       
                    if(Dval>300&&Dval<800)      //560为标准值,560us
                    {
                        RmtRec<<=1;	            //左移一位.
                        RmtRec|=0;	            //接收到0	
                    }else if(Dval>1400&&Dval<1800)	//1680为标准值,1680us
                    {
                        RmtRec<<=1;	            //左移一位.
                        RmtRec|=1;	            //接收到1
                    }else if(Dval>2200&&Dval<2600)	//得到按键键值增加的信息 2500为标准值2.5ms
                    {
                        RmtCnt++; 		        //按键次数增加1次
                        RmtSta&=0XF0;	        //清空计时器		
                    }
                }
                else if(Dval>4200&&Dval<4700)   //4500为标准值4.5ms
                {
                    RmtSta|=1<<7;	            //标记成功接收到了引导码
                    RmtCnt=0;		            //清除按键次数计数器
                }						 
            }
            RmtSta&=~(1<<4);
        }
        REMOTE_IO_GPT();
        GPT_StartTimer(GPT2);
    }		    	     	    					   
    GPT_ClearStatusFlags(GPT2,kGPT_RollOverFlag);       //清除溢出中断标志位 
    GPT_ClearStatusFlags(GPT2,kGPT_InputCapture1Flag);  //清除捕获2中断标志位    
	__DSB();			//数据同步屏蔽指令 
}


//处理红外键盘
//返回值:
//	 0,没有任何按键按下
//其他,按下的按键键值.
u8 Remote_Scan(void)
{        
	u8 sta=0;       
    u8 t1,t2;  
	if(RmtSta&(1<<6))//得到一个按键的所有信息了
	{ 
	    t1=RmtRec>>24;			//得到地址码
	    t2=(RmtRec>>16)&0xff;	//得到地址反码 
 	    if((t1==(u8)~t2)&&t1==REMOTE_ID)//检验遥控识别码(ID)及地址 
	    { 
	        t1=RmtRec>>8;
	        t2=RmtRec; 	
	        if(t1==(u8)~t2)sta=t1;//键值正确	 
		}   
		if((sta==0)||((RmtSta&0X80)==0))//按键数据错误/遥控已经没有按下了
		{
		 	RmtSta&=~(1<<6);//清除接收到有效按键标识
			RmtCnt=0;		//清除按键次数计数器
		}
	} 
    //else printf("未得到按键信息\r\n"); 
    return sta;
}
