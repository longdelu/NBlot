#include "atk_key.h"
#include "atk_delay.h"
#include "atk_soft_timer.h"
//////////////////////////////////////////////////////////////////////////////////     
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F7开发板
//KEY驱动代码       
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/11/27
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved                                      
//////////////////////////////////////////////////////////////////////////////////

static key_dev_t key_dev = {0,0, NULL, NULL};

/**
  * @brief  按键初始化函数
  * @param  None
  * @retval 按键设备句柄的指针
  */
key_handle_t atk_key_exit_init(void)
{    
    gpio_pin_config_t int_config;    
    
    //IO功能设置
    IOMUXC_SetPinMux(IOMUXC_SNVS_WAKEUP_GPIO5_IO00,0);            //SNVS_WAKEUP配置为ALT5,即GPIO5_00
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_05_GPIO1_IO05,0);          //GPIO_AD_B0_05配置为ALT5,即GPIO1_IO05
    IOMUXC_SetPinMux(IOMUXC_SNVS_PMIC_ON_REQ_GPIO5_IO01,0);       //PMIC_ON_REQ,即GPIO5_IO01
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_40_GPIO3_IO26,0);            //GPIO_EMC_40配置为ALT5,即GPIO3_IO26
    
    //配置KEY相关IO的功能
    //低转换速度,关闭输出驱动,速度为100Mhz，关闭开路功能，使能pull/keepr
    //选择pull功能，上拉22K Ohm，关闭Hyst
    IOMUXC_SetPinConfig(IOMUXC_SNVS_WAKEUP_GPIO5_IO00,0xF080);      
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_05_GPIO1_IO05,0xF080);    
    IOMUXC_SetPinConfig(IOMUXC_SNVS_PMIC_ON_REQ_GPIO5_IO01,0xF080);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_40_GPIO3_IO26,0xF080);
    
    //KEY_UP
    int_config.direction=kGPIO_DigitalInput;          //输入
    int_config.interruptMode=kGPIO_IntFallingEdge;    //下降沿触发中断
    int_config.outputLogic=1;                         //默认高电平
    GPIO_PinInit(GPIO5,0,&int_config);                //初始化GPIO5_00 

    //KEY1
    int_config.direction=kGPIO_DigitalInput;          //输入
    int_config.interruptMode=kGPIO_IntFallingEdge;    //下降沿触发中断
    int_config.outputLogic=1;                         //默认高电平
    GPIO_PinInit(GPIO5,1,&int_config);                //初始化GPIO5_01

    //KEY0
    int_config.direction=kGPIO_DigitalInput;          //输入
    int_config.interruptMode=kGPIO_IntFallingEdge;    //下降沿触发中断
    int_config.outputLogic=1;                         //默认高电平
    GPIO_PinInit(GPIO1,5,&int_config);                //初始化GPIO1_05
    
    //KEY2
    int_config.direction=kGPIO_DigitalInput;          //输入
    int_config.interruptMode=kGPIO_IntFallingEdge;    //下降沿触发中断
    int_config.outputLogic=1;                         //默认高电平
    GPIO_PinInit(GPIO3,26,&int_config);               //初始化GPIO3_26
    
    //使能WKUP(GPIO5_00)和KEY1(GPIO5_01)中断
    GPIO_PortEnableInterrupts(GPIO5,1<<0);                //GPIO5_00中断使能
    GPIO_PortEnableInterrupts(GPIO5,1<<1);                //GPIO5_01中断使能
    RT1052_NVIC_SetPriority(GPIO5_Combined_0_15_IRQn,3,0);//抢占优先级位3，0位子优先级
    EnableIRQ(GPIO5_Combined_0_15_IRQn);                  //使能GPIO5_0~15IO的中断
    
    //使能KEY0(GPIO1_05)中断
    GPIO_PortEnableInterrupts(GPIO1,1<<5);                //GPIO1_05中断使能
    RT1052_NVIC_SetPriority(GPIO1_Combined_0_15_IRQn,4,0);//抢占优先级位3，0位子优先级
    EnableIRQ(GPIO1_Combined_0_15_IRQn);                  //使能GPIO1_0~15IO的中断

    //使能KEY2(GPIO3_26)中断
    GPIO_PortEnableInterrupts(GPIO3,1<<26);                 //GPIO3_26中断使能
    RT1052_NVIC_SetPriority(GPIO3_Combined_16_31_IRQn,5,0); //抢占优先级位5，0位子优先级
    EnableIRQ(GPIO3_Combined_16_31_IRQn);                   //使能GPIO3_16~31 IO的中断
        
    return &key_dev;
}




//GPIO1_0~15共用的中断服务函数
void GPIO1_Combined_0_15_IRQHandler(void)
{
    
    if (GPIO_GetPinsInterruptFlags(GPIO1)&(1<<5))  //判断是否为GPIO1_5中断
    {  
        if(KEY0==GPIO_PIN_RESET)  //
        {                
            key_dev.key_event  = KEY0_PRES;
            key_dev.start_tick = atk_soft_timer_getick();
        }      
    }
    GPIO_PortClearInterruptFlags(GPIO1,1<<5);        //清除中断标志位
    __DSB();                //数据同步屏蔽指令 
}


//GPIO3_16~31共用的中断服务函数
void GPIO3_Combined_16_31_IRQHandler(void)
{
    if(GPIO_GetPinsInterruptFlags(GPIO3)&(1<<26))    //判断是否为GPIO3_26中断
    {
        if(KEY2==GPIO_PIN_RESET)  //
        {                
            key_dev.key_event  = KEY2_PRES;
            key_dev.start_tick = atk_soft_timer_getick();
        } 
    }
    GPIO_PortClearInterruptFlags(GPIO3,1<<26);        //清除中断标志位
    __DSB();                //数据同步屏蔽指令  
}

//GPIO5_0~15共用的中断服务函数
void GPIO5_Combined_0_15_IRQHandler(void)
{
     
    if(GPIO_GetPinsInterruptFlags(GPIO5)&(1<<0))    //判断是否为GPIO5_0中断
    {
        if(KEY0==GPIO_PIN_RESET)  //
        {                
            key_dev.key_event  = KEY0_PRES;
            key_dev.start_tick = atk_soft_timer_getick();
        } 
    }
    if(GPIO_GetPinsInterruptFlags(GPIO5)&(1<<1))   //判断是否为GPIO5_1中断  
    {
        if(KEY1==GPIO_PIN_RESET)  //
        {                
            key_dev.key_event  = KEY1_PRES;
            key_dev.start_tick = atk_soft_timer_getick();
        } 
    }
     GPIO_PortClearInterruptFlags(GPIO5,1<<0);        //清除中断标志位
     GPIO_PortClearInterruptFlags(GPIO5,1<<1);        //清除中断标志位
    __DSB();                //数据同步屏蔽指令
}

/**
  * @brief  注册按键事件回调.
  * @param  key_handle : 指向按键设备句柄的指针.
  * @param  cb         : 回调函数.
  * @param  p_arg      : 回调函数参数
  * @retval None
  */
void atk_key_registercb (key_handle_t key_handle, key_cb cb, void *p_arg)
{
    if ((cb != NULL) && (key_handle != NULL))
    {
        key_handle->key_cb = cb;
        key_handle->p_arg  = p_arg;
    }
}



/**
  * @brief  轮询按钮事件
  * @param  key_handle : 指向按键设备句柄的指针.
  * @retval None
  */
void atk_key_event_poll(key_handle_t key_handle)
{
    uint8_t key_event = 0;

    //按键消抖    
    if(key_dev.key_event)
    { 
        if (atk_soft_timer_getick() - key_dev.start_tick >= KEY_DELAY_TICK )
        {
            if (key_dev.key_event & WKUP_PRES)
            {
                if (WK_UP == GPIO_PIN_SET)
                {
                    key_event |= WKUP_PRES;
                }

                key_dev.key_event ^= WKUP_PRES;
            }
          
            if(key_dev.key_event & KEY2_PRES)
            {
              
                if (KEY2 == GPIO_PIN_RESET)
                {
                     key_event |= KEY2_PRES;
                }
                key_dev.key_event ^= KEY2_PRES;
            }
          
           if (key_dev.key_event & KEY1_PRES)
           {
                if (KEY1 == GPIO_PIN_RESET)
                {
                     key_event |= KEY1_PRES;
                }
                key_dev.key_event ^= KEY1_PRES;
           }
          
           if (key_dev.key_event & KEY0_PRES)
           {
                if (KEY0 == GPIO_PIN_RESET)
                {
                    key_event |= KEY0_PRES;
                }
                key_dev.key_event ^= KEY0_PRES;
           }
        }
    }

    //如果真的有按钮按下，则执行回调函数
    if (key_event && key_handle->key_cb)
    {
        key_handle->key_cb(key_event, key_handle->p_arg);
    }
}
