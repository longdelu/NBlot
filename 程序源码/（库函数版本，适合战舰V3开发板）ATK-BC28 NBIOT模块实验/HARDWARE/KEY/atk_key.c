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

//������ʼ������

/**
  * @brief  ������ʼ������
  * @param  None
  * @retval �����豸�����ָ��
  */
key_handle_t atk_key_exit_init(void)
{    
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    GPIO_InitTypeDef GPIO_InitStructure;
  
    //ʹ��PORTA,PORTEʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOE,ENABLE);
  
    //ʹ�ܸ��ù���ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);                      

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;//KEY0-KEY2
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;                   //���ó���������
    GPIO_Init(GPIOE, &GPIO_InitStructure);                          //��ʼ��GPIOE2,3,4

    //��ʼ�� WK_UP-->GPIOA.0      ��������
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0���ó����룬Ĭ������      
    GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.0
    

    //GPIOE.2 �ж����Լ��жϳ�ʼ������   �½��ش���
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource2);

    EXTI_InitStructure.EXTI_Line=EXTI_Line2;  //KEY2
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;  
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);     //����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���

   //GPIOE.3    �ж����Լ��жϳ�ʼ������ �½��ش��� //KEY1
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource3);
    EXTI_InitStructure.EXTI_Line=EXTI_Line3;
    EXTI_Init(&EXTI_InitStructure);      //����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���

   //GPIOE.4    �ж����Լ��жϳ�ʼ������  �½��ش���  //KEY0
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource4);
    EXTI_InitStructure.EXTI_Line=EXTI_Line4;
    EXTI_Init(&EXTI_InitStructure);      //����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���


   //GPIOA.0    �ж����Լ��жϳ�ʼ������ �����ش��� PA0  WK_UP
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0); 

    EXTI_InitStructure.EXTI_Line=EXTI_Line0;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_Init(&EXTI_InitStructure);    //����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���


    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;      //ʹ�ܰ���WK_UP���ڵ��ⲿ�ж�ͨ��
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;  //��ռ���ȼ�2�� 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;          //�����ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                //ʹ���ⲿ�ж�ͨ��
    NVIC_Init(&NVIC_InitStructure); 

    NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;      //ʹ�ܰ���KEY2���ڵ��ⲿ�ж�ͨ��
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;  //��ռ���ȼ�2�� 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;          //�����ȼ�2
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                //ʹ���ⲿ�ж�ͨ��
    NVIC_Init(&NVIC_InitStructure);


    NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;      //ʹ�ܰ���KEY1���ڵ��ⲿ�ж�ͨ��
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;  //��ռ���ȼ�2 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;          //�����ȼ�1 
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                //ʹ���ⲿ�ж�ͨ��
    NVIC_Init(&NVIC_InitStructure);      //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;      //ʹ�ܰ���KEY0���ڵ��ⲿ�ж�ͨ��
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;  //��ռ���ȼ�2 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;          //�����ȼ�0 
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                //ʹ���ⲿ�ж�ͨ��
    NVIC_Init(&NVIC_InitStructure);      //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
        
    return &key_dev;
}


/**
  * @brief  �жϷ�����.
  */
void EXTI0_IRQHandler(void)
{
  
    if(WK_UP==1) 
    {
        key_dev.key_event  = WKUP_PRES;
        key_dev.start_tick = atk_soft_timer_getick();
    }
   
    EXTI_ClearITPendingBit(EXTI_Line0); //���LINE0�ϵ��жϱ�־λ  
}
 
/**
  * @brief  �жϷ�����.
  */
void EXTI2_IRQHandler(void)
{
  
  if (KEY2==0)    //����KEY2
  {
      key_dev.key_event  = KEY2_PRES;
      key_dev.start_tick = atk_soft_timer_getick();    
  }
  
  EXTI_ClearITPendingBit(EXTI_Line2);  //���LINE2�ϵ��жϱ�־λ  
}
/**
  * @brief  �жϷ�����.
  */
void EXTI3_IRQHandler(void)
{

  if (KEY1==0)   //����KEY1
  {         
     key_dev.key_event  = KEY1_PRES;
     key_dev.start_tick = atk_soft_timer_getick();
  }     
  EXTI_ClearITPendingBit(EXTI_Line3);  //���LINE3�ϵ��жϱ�־λ  
}

/**
  * @brief  �жϷ�����.
  */
void EXTI4_IRQHandler(void)
{

    if (KEY0==0)   //����KEY0
    {
       key_dev.key_event  = KEY0_PRES;
       key_dev.start_tick = atk_soft_timer_getick();
    }
    
    EXTI_ClearITPendingBit(EXTI_Line4);  //���LINE4�ϵ��жϱ�־λ  
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
                if (WK_UP == 1)
                {
                    key_event |= WKUP_PRES;
                }

                key_dev.key_event ^= WKUP_PRES;
            }
          
            if(key_dev.key_event & KEY2_PRES)
            {
              
                if (KEY2 == 0)
                {
                     key_event |= KEY2_PRES;
                }
                key_dev.key_event ^= KEY2_PRES;
            }
          
           if (key_dev.key_event & KEY1_PRES)
           {
                if (KEY1 == 0)
                {
                     key_event |= KEY1_PRES;
                }
                key_dev.key_event ^= KEY1_PRES;
           }
          
           if (key_dev.key_event & KEY0_PRES)
           {
                if (KEY0 == 0)
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
