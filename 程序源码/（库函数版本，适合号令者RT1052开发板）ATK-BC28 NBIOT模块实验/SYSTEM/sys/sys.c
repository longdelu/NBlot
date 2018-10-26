#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////     
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK I.MX RT1052开发板
//系统时钟初始化    
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2017/12/10
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved
//********************************************************************************
//修改说明
//无
////////////////////////////////////////////////////////////////////////////////// 

//ARM PLL(PLL1)配置,配置为1200MHz
const clock_arm_pll_config_t armPllConfig =     
{
    //取值范围54-108.
    //Fout=Fin*div_selcet/2=24*100/2=1200Mhz;
    .loopDivider = 100,        
};

//SYS PLL(PLL2)配置，配置为528MHz
const clock_sys_pll_config_t sysPllConfig = 
{
    //SYS PLL倍频因子，可选择选项如下：
    //Fout = Fin * ( 20 + loopDivider*2 + numerator / denominator) = 24*(20+1*2+0/1)=24*22=528MHz
    .loopDivider = 1,
    .numerator = 0,
    .denominator = 1,
};

//USB1 PLL(PLL3)配置，配置为480MHz
const clock_usb_pll_config_t usb1PllConfig = 
{
    //USB1 PLL倍频因子，可选择选项如下：
    //0: Fout=Fref*20=24*20=480M;
    //1: Fout=Fref*22=24*22=528M
    .loopDivider = 0,
};

//系统软复位   
void Sys_Soft_Reset(void)
{   
    SCB->AIRCR =0X05FA0000|(u32)0x04;      
}     

//设置RT1052的系统时钟
void RT1052_Clock_Init(void)
{
    CLOCK_SetXtalFreq(24000000);            //芯片主晶振24MHz
    CLOCK_SetRtcXtalFreq(32768);            //RTC晶振32.728KHz

    //初始化时钟阶段先设置为低速的24M时钟，
    //后面会重新将PLL1设置为1200MHz，主频设置为600MHz
    CLOCK_SetMux(kCLOCK_PeriphClk2Mux,1);   //设置PERIPH_CLK2的时钟源为OSC时钟，也就是24M晶振 
    CLOCK_SetMux(kCLOCK_PeriphMux,1);       //设置PERIPH_CLK为PERIPH_CLK2=24MHz 

    //设置VDD_SOC电压为1.25V，这样的话AHB就可以到600MHz
    DCDC->REG3=(DCDC->REG3&(~DCDC_REG3_TRG_MASK))|DCDC_REG3_TRG(0x12);

    //等待DCDC的稳定，即DCDC->REG0寄存器的bit31(STS_DC_OK)为1
    while(DCDC_REG0_STS_DC_OK_MASK!=(DCDC_REG0_STS_DC_OK_MASK&DCDC->REG0)){};

    //下面配置中的1200Mhz，528Mhz和480Mhz等频率均按照PLL1为1200Mhz来配置的
    //因为后面会重新配置PLL1的，不要纠结于上面配置的24MHz！！！！
    CLOCK_InitArmPll(&armPllConfig);         //配置PLL1(ARM PLL)为1200MHz
    CLOCK_InitSysPll(&sysPllConfig);        //配置PLL2(SYS PLL)为528MHz
    CLOCK_InitUsb1Pll(&usb1PllConfig);         //配置PLL3(USB1 PLL)为480MHz

//            CCM_ANALOG->PLL_AUDIO |= CCM_ANALOG_PLL_AUDIO_ENABLE_MASK;
//    /* Enbale Video PLL output. */
//    CCM_ANALOG->PLL_VIDEO |= CCM_ANALOG_PLL_VIDEO_ENABLE_MASK;
//    /* Enable ENET PLL output. */
//    CCM_ANALOG->PLL_ENET |= CCM_ANALOG_PLL_ENET_ENABLE_MASK;
//    CCM_ANALOG->PLL_ENET |= CCM_ANALOG_PLL_ENET_ENET_25M_REF_EN_MASK;
        
        
    CLOCK_SetDiv(kCLOCK_ArmDiv,1);             //ARM PODF 2分频，可选择0~7分别对应1~8分频，1200Mhz/2=600MHz
    CLOCK_SetMux(kCLOCK_PrePeriphMux,3);     //设置PRE_PERIPH_CLK的时钟源为PLL1经过ARM PODF分频后的时钟
                                            //即：PRE_PERIPH_CLK=PLL1/ARM PODF=1200MHz/2=600MHz
    CLOCK_SetMux(kCLOCK_PeriphMux,0);        //设置PERIPH_CLK为PRE_PERIPH_CLK=600MHz 
        
    CLOCK_SetDiv(kCLOCK_AhbDiv,0);             //AHB_CLK_ROOT=PERIPH_CLK/1=600/1=600Mhz
    CLOCK_SetDiv(kCLOCK_IpgDiv,3);             //IPG_CLK_ROOT=AHB_CLK_ROOT/4=600/4=150MHz
    CLOCK_SetDiv(kCLOCK_PerclkDiv,1);         //PER_CLK_ROOT=IPG_CLK_ROOT/2=150/2=75MHz

    
    //设置PLL2(SYS PLL)各个PFD
    CLOCK_InitSysPfd(kCLOCK_Pfd0,27);       //PLL2_PFD0=528*18/27=352Mhz
    CLOCK_InitSysPfd(kCLOCK_Pfd1,16);       //PLL2_PFD1=528*18/16=594Mhz
    CLOCK_InitSysPfd(kCLOCK_Pfd2,24);       //PLL2_PFD2=528*18/24=396Mhz
    CLOCK_InitSysPfd(kCLOCK_Pfd3,32);       //PLL2_PFD3=528*18/32=297Mhz
    
    //设置PLL3(USB1 PLL)各个PFD
    CLOCK_InitUsb1Pfd(kCLOCK_Pfd0,15);      //PLL3_PFD0=480*18/12=720Mhz
    CLOCK_InitUsb1Pfd(kCLOCK_Pfd1,13);      //PLL3_PFD1=480*18/13=664.62Mhz
    CLOCK_InitUsb1Pfd(kCLOCK_Pfd2,17);      //PLL3_PFD2=480*18/17=508.24Mhz
    CLOCK_InitUsb1Pfd(kCLOCK_Pfd3,19);      //PLL3_PFD3=480*18/19=454.74Mhz
    
    //一共7个PLL，上面初始化了PLL1(ARM PLL)、PLL2(SYS PLL)和USB1 PLL(USB1 PLL)
    //其它四个PLL后面根据实际使用情况在做初始化。
    CLOCK_DeinitAudioPll();    //关闭AUDIO PLL (PLL4)
    CLOCK_DeinitVideoPll();    //关闭VIDEO PLL (PLL5)
    CLOCK_DeinitEnetPll();    //关闭ENET  PLL (PLL6)
    CLOCK_DeinitUsb2Pll();    //关闭USB2  PLL (PLL7)
    
    //更新内核时钟，此函数计算全局变量SystemCoreClock的值，
    //SystemCoreClock就是内核时钟，按照上面的配置此时钟就是600MHz
    SystemCoreClockUpdate(); 
        
    //使能SNVS时钟
    CLOCK_EnableClock(kCLOCK_IomuxcSnvs);     
    
    CLOCK_SetMux(kCLOCK_SemcMux,0); //PERIPH_CLK作为SEMC时钟源,PERIPH_CLK=600M
    CLOCK_SetDiv(kCLOCK_SemcDiv,3); //设置SEMC时钟由PERIPH_CLK四分频得到，即SEMC Clock=600/4=150Mhz
}

//设置RT1052需要保护的存储块
//必须对部分存储区域进行MPU保护,否则可能导致程序运行异常 
//Cortex-M7内核的MPU一共有16个region，每个region的起始地址、属性和大小等都可以设置，
//MPU的详细信息以及寄存器内容请参考《Cortex-M7编程手册(ST官方)》中的第4.6节
void MPU_Memory_Protection(void)
{
    SCB_DisableICache();                        //关闭I Cache
    SCB_DisableDCache();                        //关闭D Cache 
    ARM_MPU_Disable();                            //关闭MPU
 
    //区域0~8配置，RBAR寄存器设置region基地址，BASR设置region的属性和大小
    MPU->RBAR=ARM_MPU_RBAR(0,0XC0000000);                                            //region 0,起始地址0XC000 0000
    MPU->RASR=ARM_MPU_RASR(0,ARM_MPU_AP_FULL,2,0,0,0,0,ARM_MPU_REGION_SIZE_512MB);    //可执行,全访问,设备,禁止共享,禁止cache,禁止缓存,禁止子region,512MB大小

    MPU->RBAR=ARM_MPU_RBAR(1,0X80000000);                                            //region 1,起始地址0X8000 0000,SDRAM首地址
    MPU->RASR=ARM_MPU_RASR(0,ARM_MPU_AP_FULL,2,0,0,0,0,ARM_MPU_REGION_SIZE_1GB);    //可执行,全访问,设备,禁止共享,禁止cache,禁止缓存,禁止子region,1GB大小

    MPU->RBAR=ARM_MPU_RBAR(2,0X60000000);                                            //region 2,起始地址0X6000 0000,FlexSPI首地址    
    MPU->RASR=ARM_MPU_RASR(0,ARM_MPU_AP_FULL,0,0,1,1,0,ARM_MPU_REGION_SIZE_512MB);    //可执行,全访问,写回,禁止共享,允许cache,允许缓存,禁止子region,512MB大小

    MPU->RBAR=ARM_MPU_RBAR(3,0X00000000);                                            //region 3,起始地址0X0000 0000,ITCM首地址(包含ROMCP,DTCM,OCRAM)
    MPU->RASR=ARM_MPU_RASR(0,ARM_MPU_AP_FULL,2,0,0,0,0,ARM_MPU_REGION_SIZE_1GB);    //可执行,全访问,设备,禁止共享,禁止cache,禁止缓存,禁止子region,1GB大小

    MPU->RBAR=ARM_MPU_RBAR(4,0X00000000);                                            //region 4,起始地址0X0000 0000,ITCM首地址,相同地址段设置,region大的覆盖小的
    MPU->RASR=ARM_MPU_RASR(0,ARM_MPU_AP_FULL,0,0,1,1,0,ARM_MPU_REGION_SIZE_128KB);    //可执行,全访问,写回,禁止共享,允许cache,允许缓存,禁止子region,128KB大小

    MPU->RBAR=ARM_MPU_RBAR(5,0X20000000);                                            //region 5,起始地址0X2000 0000,DTCM首地址,相同地址段设置,region大的覆盖小的
    MPU->RASR=ARM_MPU_RASR(0,ARM_MPU_AP_FULL,0,0,1,1,0,ARM_MPU_REGION_SIZE_128KB);    //可执行,全访问,写回,禁止共享,允许cache,允许缓存,禁止子region,128KB大小

    MPU->RBAR=ARM_MPU_RBAR(6,0X20200000);                                            //region 6,起始地址0X2020 0000,OCRAM首地址,相同地址段设置,region大的覆盖小的
    MPU->RASR=ARM_MPU_RASR(0,ARM_MPU_AP_FULL,0,1,1,0,0,ARM_MPU_REGION_SIZE_256KB);    //可执行,全访问,写回,允许共享,允许cache,禁止缓存,禁止子region,256KB大小

    MPU->RBAR=ARM_MPU_RBAR(7,0X80000000);                                            //region 7,起始地址0X8000 0000,SDRAM首地址,相同地址段设置,region大的覆盖小的
    MPU->RASR=ARM_MPU_RASR(0,ARM_MPU_AP_FULL,0,0,1,1,0,ARM_MPU_REGION_SIZE_2MB);    //可执行,全访问,写回,禁止共享,允许cache,允许缓存,禁止子region,2MB大小 
    
    ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);    //使能MPU,开启背景region
    SCB_EnableDCache();                            //使能D Cache
    SCB_EnableICache();                            //使能I Cache
}

//中断优先级分组设置
//grpx: 优先级组，可选择如下5组中的一组
//NVIC_PRIORITYGROUP_0: 0位抢占优先级
//                      4位子优先级
//NVIC_PRIORITYGROUP_1: 1位抢占优先级
//                      3位子优先级
//NVIC_PRIORITYGROUP_2: 2位抢占优先级
//                      2位子优先级
//NVIC_PRIORITYGROUP_3: 3位抢占优先级
//                      1位子优先级
//NVIC_PRIORITYGROUP_4: 4位抢占优先级
//                      0位子优先级
//
void RT1052_NVIC_SetPriorityGrouping(u8 grpx)
{
    NVIC_SetPriorityGrouping((uint32_t)grpx);
}

//设置中断优先级
//IRQn:    中断源
//prep：抢占优先级
//subp：子优先级
void RT1052_NVIC_SetPriority(IRQn_Type IRQn,u32 prep,u32 subp)
{ 
    u32 prioritygroup = 0x00;
  
    prioritygroup=NVIC_GetPriorityGrouping();
    NVIC_SetPriority(IRQn,NVIC_EncodePriority(prioritygroup,prep,subp));
}

//设置向量表偏移地址
//vtab:基址
//offset:偏移量         
void MY_NVIC_SetVectorTable(u32 vtab,u32 offset)     
{              
    SCB->VTOR=vtab|(offset&(u32)0xFFFFFE00);    //设置NVIC的向量表偏移寄存器,VTOR低9位保留,即[8:0]保留。
}

//THUMB指令不支持汇编内联
//采用如下方法实现执行汇编指令WFI  
__asm void WFI_SET(void)
{
    WFI;          
}
//关闭所有中断(但是不包括fault和NMI中断)
__asm void INTX_DISABLE(void)
{
    CPSID   I
    BX      LR      
}
//开启所有中断
__asm void INTX_ENABLE(void)
{
    CPSIE   I
    BX      LR  
}
//设置栈顶地址
//addr:栈顶地址
__asm void MSR_MSP(u32 addr) 
{
    MSR MSP, r0             //set Main Stack value
    BX r14
}
