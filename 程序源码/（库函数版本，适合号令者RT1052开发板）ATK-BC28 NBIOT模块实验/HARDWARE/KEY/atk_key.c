#include "atk_key.h"
#include "atk_delay.h"
#include "atk_soft_timer.h"
//////////////////////////////////////////////////////////////////////////////////     
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F7������
//KEY��������       
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/11/27
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved                                      
//////////////////////////////////////////////////////////////////////////////////

static key_dev_t key_dev = {0,0, NULL, NULL};

/**
  * @brief  ������ʼ������
  * @param  None
  * @retval �����豸�����ָ��
  */
key_handle_t atk_key_exit_init(void)
{    
    gpio_pin_config_t int_config;    
    
    //IO��������
    IOMUXC_SetPinMux(IOMUXC_SNVS_WAKEUP_GPIO5_IO00,0);            //SNVS_WAKEUP����ΪALT5,��GPIO5_00
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_05_GPIO1_IO05,0);          //GPIO_AD_B0_05����ΪALT5,��GPIO1_IO05
    IOMUXC_SetPinMux(IOMUXC_SNVS_PMIC_ON_REQ_GPIO5_IO01,0);       //PMIC_ON_REQ,��GPIO5_IO01
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_40_GPIO3_IO26,0);            //GPIO_EMC_40����ΪALT5,��GPIO3_IO26
    
    //����KEY���IO�Ĺ���
    //��ת���ٶ�,�ر��������,�ٶ�Ϊ100Mhz���رտ�·���ܣ�ʹ��pull/keepr
    //ѡ��pull���ܣ�����22K Ohm���ر�Hyst
    IOMUXC_SetPinConfig(IOMUXC_SNVS_WAKEUP_GPIO5_IO00,0xF080);      
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_05_GPIO1_IO05,0xF080);    
    IOMUXC_SetPinConfig(IOMUXC_SNVS_PMIC_ON_REQ_GPIO5_IO01,0xF080);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_40_GPIO3_IO26,0xF080);
    
    //KEY_UP
    int_config.direction=kGPIO_DigitalInput;          //����
    int_config.interruptMode=kGPIO_IntFallingEdge;    //�½��ش����ж�
    int_config.outputLogic=1;                         //Ĭ�ϸߵ�ƽ
    GPIO_PinInit(GPIO5,0,&int_config);                //��ʼ��GPIO5_00 

    //KEY1
    int_config.direction=kGPIO_DigitalInput;          //����
    int_config.interruptMode=kGPIO_IntFallingEdge;    //�½��ش����ж�
    int_config.outputLogic=1;                         //Ĭ�ϸߵ�ƽ
    GPIO_PinInit(GPIO5,1,&int_config);                //��ʼ��GPIO5_01

    //KEY0
    int_config.direction=kGPIO_DigitalInput;          //����
    int_config.interruptMode=kGPIO_IntFallingEdge;    //�½��ش����ж�
    int_config.outputLogic=1;                         //Ĭ�ϸߵ�ƽ
    GPIO_PinInit(GPIO1,5,&int_config);                //��ʼ��GPIO1_05
    
    //KEY2
    int_config.direction=kGPIO_DigitalInput;          //����
    int_config.interruptMode=kGPIO_IntFallingEdge;    //�½��ش����ж�
    int_config.outputLogic=1;                         //Ĭ�ϸߵ�ƽ
    GPIO_PinInit(GPIO3,26,&int_config);               //��ʼ��GPIO3_26
    
    //ʹ��WKUP(GPIO5_00)��KEY1(GPIO5_01)�ж�
    GPIO_PortEnableInterrupts(GPIO5,1<<0);                //GPIO5_00�ж�ʹ��
    GPIO_PortEnableInterrupts(GPIO5,1<<1);                //GPIO5_01�ж�ʹ��
    RT1052_NVIC_SetPriority(GPIO5_Combined_0_15_IRQn,3,0);//��ռ���ȼ�λ3��0λ�����ȼ�
    EnableIRQ(GPIO5_Combined_0_15_IRQn);                  //ʹ��GPIO5_0~15IO���ж�
    
    //ʹ��KEY0(GPIO1_05)�ж�
    GPIO_PortEnableInterrupts(GPIO1,1<<5);                //GPIO1_05�ж�ʹ��
    RT1052_NVIC_SetPriority(GPIO1_Combined_0_15_IRQn,4,0);//��ռ���ȼ�λ3��0λ�����ȼ�
    EnableIRQ(GPIO1_Combined_0_15_IRQn);                  //ʹ��GPIO1_0~15IO���ж�

    //ʹ��KEY2(GPIO3_26)�ж�
    GPIO_PortEnableInterrupts(GPIO3,1<<26);                 //GPIO3_26�ж�ʹ��
    RT1052_NVIC_SetPriority(GPIO3_Combined_16_31_IRQn,5,0); //��ռ���ȼ�λ5��0λ�����ȼ�
    EnableIRQ(GPIO3_Combined_16_31_IRQn);                   //ʹ��GPIO3_16~31 IO���ж�
        
    return &key_dev;
}




//GPIO1_0~15���õ��жϷ�����
void GPIO1_Combined_0_15_IRQHandler(void)
{
    
    if (GPIO_GetPinsInterruptFlags(GPIO1)&(1<<5))  //�ж��Ƿ�ΪGPIO1_5�ж�
    {  
        if(KEY0==GPIO_PIN_RESET)  //
        {                
            key_dev.key_event  = KEY0_PRES;
            key_dev.start_tick = atk_soft_timer_getick();
        }      
    }
    GPIO_PortClearInterruptFlags(GPIO1,1<<5);        //����жϱ�־λ
    __DSB();                //����ͬ������ָ�� 
}


//GPIO3_16~31���õ��жϷ�����
void GPIO3_Combined_16_31_IRQHandler(void)
{
    if(GPIO_GetPinsInterruptFlags(GPIO3)&(1<<26))    //�ж��Ƿ�ΪGPIO3_26�ж�
    {
        if(KEY2==GPIO_PIN_RESET)  //
        {                
            key_dev.key_event  = KEY2_PRES;
            key_dev.start_tick = atk_soft_timer_getick();
        } 
    }
    GPIO_PortClearInterruptFlags(GPIO3,1<<26);        //����жϱ�־λ
    __DSB();                //����ͬ������ָ��  
}

//GPIO5_0~15���õ��жϷ�����
void GPIO5_Combined_0_15_IRQHandler(void)
{
     
    if(GPIO_GetPinsInterruptFlags(GPIO5)&(1<<0))    //�ж��Ƿ�ΪGPIO5_0�ж�
    {
        if(KEY0==GPIO_PIN_RESET)  //
        {                
            key_dev.key_event  = KEY0_PRES;
            key_dev.start_tick = atk_soft_timer_getick();
        } 
    }
    if(GPIO_GetPinsInterruptFlags(GPIO5)&(1<<1))   //�ж��Ƿ�ΪGPIO5_1�ж�  
    {
        if(KEY1==GPIO_PIN_RESET)  //
        {                
            key_dev.key_event  = KEY1_PRES;
            key_dev.start_tick = atk_soft_timer_getick();
        } 
    }
     GPIO_PortClearInterruptFlags(GPIO5,1<<0);        //����жϱ�־λ
     GPIO_PortClearInterruptFlags(GPIO5,1<<1);        //����жϱ�־λ
    __DSB();                //����ͬ������ָ��
}

/**
  * @brief  ע�ᰴ���¼��ص�.
  * @param  key_handle : ָ�򰴼��豸�����ָ��.
  * @param  cb         : �ص�����.
  * @param  p_arg      : �ص���������
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
  * @brief  ��ѯ��ť�¼�
  * @param  key_handle : ָ�򰴼��豸�����ָ��.
  * @retval None
  */
void atk_key_event_poll(key_handle_t key_handle)
{
    uint8_t key_event = 0;

    //��������    
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

    //�������а�ť���£���ִ�лص�����
    if (key_event && key_handle->key_cb)
    {
        key_handle->key_cb(key_event, key_handle->p_arg);
    }
}
