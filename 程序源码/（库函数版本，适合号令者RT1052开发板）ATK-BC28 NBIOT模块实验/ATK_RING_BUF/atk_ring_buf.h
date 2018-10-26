/*
 * Copyright (c) ATK
 * All rights reserved
 */
 
#ifndef _ATK_RING_BUF_H_
#define _ATK_RING_BUF_H_

#include "stdint.h"

#ifdef __cplusplus  
extern "C" {  
#endif  


/**
 * @brief  ���廷�λ��������ݻ������󳤶�
 */    
#define RING_BUF_LEN                  512         

    
/**
 * @brief  ���λ������ṹ��
 */    
typedef struct atk_ring_buf_t
{
    uint32_t head;                 
    uint32_t tail;
    uint32_t lenght;
    uint8_t  ring_buf[RING_BUF_LEN];     
}atk_ring_buf_t;
  

/**
 * @brief  ��ʼ�����λ������������Ϣ
 * @param  p_ring_buf : ָ���˻��λ�������ָ��
 * @retval 0
 */
int atk_ring_buf_init (atk_ring_buf_t *p_ring_buf);

/**
 * @brief �����λ�����д����
 * @param  p_ring_buf : ָ���˻��λ�������ָ��
 * @param  data       : д�������
 * @retval 0  д��ɹ�  -1 д��ʧ��
 */
int atk_ring_buf_write(atk_ring_buf_t *p_ring_buf, uint8_t data);


 /**
  * @brief  ��ȡ���λ�����������
  * @param  p_ring_buf : ָ���˻��λ�������ָ��
  * @param  data       : ��ȡ����
  * @retval 0  ��ȡ�ɹ�  -1 ��ȡʧ��
  */
int atk_ring_buf_read(atk_ring_buf_t *p_ring_buf, uint8_t *data);

/**
 * @brief ��ȡ���λ���������Ч���ݵĸ���
 * @param  p_ring_buf : ָ���˻��λ�������ָ��
 * @retval ���λ���������Ч���ݳ���
 */
int atk_ring_buf_avail_len (atk_ring_buf_t *p_ring_buf);

 /**
  * @brief  �ӽ��ջ������ȡָ�����ȵ����ݣ����ͷ�ռ�õĿռ�
  * @param  p_ring_buf : ָ���˻��λ�������ָ��
  * @param  data       : ��ȡ���ݵĻ�������ַ
  * @param  len        : ��ȡ���ݵĳ���
  * @retval 0  ��ȡ�ɹ�  -1 ��ȡʧ��   
  */
int atk_ring_buf_size_read(atk_ring_buf_t *p_ring_buf, uint8_t *data, int len);


 /**
  * @brief  д����ջ������ָ�����ȵ����ݣ���ռ�õĿռ�
  * @param  p_ring_buf : ָ���˻��λ�������ָ��
  * @param  data       : д�����ݵĻ�������ַ
  * @param  len        : д�����ݵ���Ч����
  * @retval 0  д��ɹ�  -1 д��ʧ��  
  */
int atk_ring_buf_size_write(atk_ring_buf_t *p_ring_buf, uint8_t *data, int len);  
                         

#ifdef __cplusplus
} 
#endif

#endif //end of _ATK_RING_BUF_H_
