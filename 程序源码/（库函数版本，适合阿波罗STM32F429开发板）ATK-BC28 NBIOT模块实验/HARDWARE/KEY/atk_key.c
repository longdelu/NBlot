#include "atk_key.h"
#include "atk_delay.h"

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
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOA_CLK_ENABLE();               //����GPIOAʱ��
    __HAL_RCC_GPIOC_CLK_ENABLE();               //����GPIOCʱ��
    __HAL_RCC_GPIOH_CLK_ENABLE();               //����GPIOHʱ��
    
    GPIO_Initure.Pin=GPIO_PIN_0;                //PA0
    GPIO_Initure.Mode=GPIO_MODE_IT_RISING;      //�����ش���
    GPIO_Initure.Pull=GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
    
    GPIO_Initure.Pin=GPIO_PIN_13;               //PC13
    GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;     //�½��ش���
    GPIO_Initure.Pull=GPIO_PULLUP;
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);
    
    GPIO_Initure.Pin=GPIO_PIN_2|GPIO_PIN_3;     //PH2,3
    HAL_GPIO_Init(GPIOH,&GPIO_Initure);
    
    //�ж���0-PA0
    HAL_NVIC_SetPriority(EXTI0_IRQn,12,0);       //��ռ���ȼ�Ϊ12�������ȼ�Ϊ0
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);              //ʹ���ж���0
    
    //�ж���2-PH2
    HAL_NVIC_SetPriority(EXTI2_IRQn,12,1);       //��ռ���ȼ�Ϊ12�������ȼ�Ϊ1
    HAL_NVIC_EnableIRQ(EXTI2_IRQn);              //ʹ���ж���2
    
    //�ж���3-PH3
    HAL_NVIC_SetPriority(EXTI3_IRQn,12,2);       //��ռ���ȼ�Ϊ12�������ȼ�Ϊ2
    HAL_NVIC_EnableIRQ(EXTI3_IRQn);              //ʹ���ж���2
    
    //�ж���13-PC13
    HAL_NVIC_SetPriority(EXTI15_10_IRQn,12,3);   //��ռ���ȼ�Ϊ12�������ȼ�Ϊ3
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);          //ʹ���ж���13  
        
    return &key_dev;
}


/**
  * @brief  �жϷ�����.
  */
void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);//�����жϴ����ú���
}

/**
  * @brief  �жϷ�����.
  */
void EXTI2_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);//�����жϴ����ú���
}

/**
  * @brief  �жϷ�����.
  */
void EXTI3_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);//�����жϴ����ú���
}

/**
  * @brief  �жϷ�����.
  */
void EXTI15_10_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);//�����жϴ����ú���
}

/**
  * @brief  HAL���ⲿ�жϷ�����������ô˺���.
  * @param  GPIO_Pin : �ж����ź�.
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    switch(GPIO_Pin)
    {
        case GPIO_PIN_0:
            if(WK_UP==GPIO_PIN_SET) 
            {
                key_dev.key_event  = WKUP_PRES;
                key_dev.start_tick = HAL_GetTick();
            }
            break;
            
        case GPIO_PIN_13:
            if (KEY2==GPIO_PIN_RESET)  //
            {
                key_dev.key_event  = KEY2_PRES;
                key_dev.start_tick = HAL_GetTick();           
                
            }
            break;
            
        case GPIO_PIN_2:
            if(KEY1==GPIO_PIN_RESET)  
            {
                key_dev.key_event  = KEY1_PRES;
                key_dev.start_tick = HAL_GetTick();
                
            }
            break;
            
        case GPIO_PIN_3:
            
            if(KEY0==GPIO_PIN_RESET)  //
            {
                
                key_dev.key_event  = KEY0_PRES;
                key_dev.start_tick = HAL_GetTick();
            }
            break;
            
        default:
                
            break;
    }
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
        if (HAL_GetTick() - key_dev.start_tick >= KEY_DELAY_TICK )
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
