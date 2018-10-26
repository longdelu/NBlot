/*
 * Copyright (c) atk
 * All rights reserved
 */
#include "atk_sys.h"
#include "atk_ring_buf.h"

/**
 * @brief ��ʼ�����λ������������Ϣ
 */
int atk_ring_buf_init (atk_ring_buf_t *p_ring_buf)
{    
    if (p_ring_buf == NULL) {        
        return -1;       
    }
    
    p_ring_buf->head   = 0;
    p_ring_buf->tail   = 0;
    p_ring_buf->lenght = 0;
    
    return 0;
}

/**
 * @brief �����λ�����д����
 */
int atk_ring_buf_write(atk_ring_buf_t *p_ring_buf, uint8_t data)
{
    if(p_ring_buf->lenght >= RING_BUF_LEN)          //�жϻ������Ƿ�����
    {      
        printf("the ring buf is full\r\n");
        
        return -1;
    } 
    
    //�����ٽ�������
//    INTX_DISABLE();
    
    p_ring_buf->ring_buf[p_ring_buf->tail] = data;

    //��ֹԽ��Ƿ�����
    p_ring_buf->tail = (p_ring_buf->tail+1) % RING_BUF_LEN;
    
    p_ring_buf->lenght++;

//   INTX_ENABLE();     
    
    return 0;
}

/**
 * @brief ���λ���������Ч���ݵĸ���
 */
int atk_ring_buf_avail_len (atk_ring_buf_t *p_ring_buf)
{
     return p_ring_buf->lenght;
}

 /**
 * @brief  ��ȡ���λ�����������
 */
int atk_ring_buf_read(atk_ring_buf_t *p_ring_buf, uint8_t *data)
{
   if (p_ring_buf->lenght == 0)    //�жϷǿ�
   {
       return -1;
   }
   
   //�����ٽ�������
//   INTX_DISABLE();   
   
   //�Ƚ��ȳ�FIFO���ӻ�����ͷ��
   *data = p_ring_buf->ring_buf[p_ring_buf->head];
   
   //��ֹԽ��Ƿ�����
   p_ring_buf->head = (p_ring_buf->head+1) % RING_BUF_LEN; 
   
   p_ring_buf->lenght--;
   
//   INTX_ENABLE(); 
   
   return 0;
}

 /**
 * @brief  �ӽ��ջ������ȡָ�����ȵ����ݣ����ͷ�ռ�õĿռ�
 */
int atk_ring_buf_size_read(atk_ring_buf_t *p_ring_buf, uint8_t *data, int len)
{   
    int i = 0; 
        
    if (len > atk_ring_buf_avail_len(p_ring_buf))
    {
        return -1; 
    }
    
    for (i = 0; i <  len; i++) {
        
         atk_ring_buf_read(p_ring_buf, &data[i]);
    }

    return 0;
}


 /**
 * @brief  д����ջ������ָ�����ȵ����ݣ���ռ�õĿռ�
 */
int atk_ring_buf_size_write(atk_ring_buf_t *p_ring_buf, uint8_t *data, int len)
{
    
    int i = 0; 
    
    //����������С��д��ĳ���    
    if (len > (RING_BUF_LEN - p_ring_buf->lenght))
    {
        return -1; 
    }
    
    for (i = 0; i <  len; i++) {
        
         atk_ring_buf_write(p_ring_buf, data[i]);
    }

    return 0;
}




