#ifndef _ATK_KEY_H
#define _ATK_KEY_H
#include "atk_sys.h"

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


#define KEY0        HAL_GPIO_ReadPin(GPIOH,GPIO_PIN_3)  //KEY0����PH3
#define KEY1        HAL_GPIO_ReadPin(GPIOH,GPIO_PIN_2)  //KEY1����PH2
#define KEY2        HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13) //KEY2����PC13
#define WK_UP       HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)  //WKUP����PA0

/**
  * @brief  ������������¼�
  */
#define KEY0_PRES   1
#define KEY1_PRES   2
#define KEY2_PRES   4
#define WKUP_PRES   8

//��������ʱ��
#define KEY_DELAY_TICK   15

/**
  * @brief  ���尴���ص�����ָ��
  */
typedef void (*key_cb)(u32 key_event, void *p_arg);


/**
  * @brief  �����ṹ��
  */
typedef struct key_dev
{
  
    uint8_t key_event;    //�����¼�
    
    int     start_tick;   //������ʼʱ��
    
    key_cb  key_cb;       //�����ػغ���
    
    void   *p_arg;        //�ػغ�������       
    
}key_dev_t;


/**
  * @brief  �����豸���
  */
typedef key_dev_t *key_handle_t;


/**
  * @brief  ������ʼ������
  * @param  None
  * @retval �����豸���
  */
key_handle_t atk_key_exit_init(void);



/**
  * @brief  ע�ᰴ���¼��ص�.
  * @param  key_handle : ָ�򰴼��豸�����ָ��.
  * @param  cb         : �ص�����.
  * @param  p_arg      : �ص���������
  * @retval None
  */
void atk_key_registercb (key_handle_t key_handle, key_cb cb, void *p_arg);

/**
  * @brief  ��ѯ��ť�¼�
  * @param  key_handle : ָ�򰴼��豸�����ָ��.
  * @retval None
  */
void atk_key_event_poll(key_handle_t key_handle);

#endif
