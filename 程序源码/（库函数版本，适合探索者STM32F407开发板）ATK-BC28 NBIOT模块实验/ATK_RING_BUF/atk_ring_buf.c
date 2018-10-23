/*
 * Copyright (c) atk
 * All rights reserved
 */
#include "atk_sys.h"
#include "atk_ring_buf.h"

/**
 * @brief 初始化环形缓冲区的相关信息
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
 * @brief 往环形缓冲区写数据
 */
int atk_ring_buf_write(atk_ring_buf_t *p_ring_buf, uint8_t data)
{
    if(p_ring_buf->lenght >= RING_BUF_LEN)          //判断缓冲区是否已满
    {      
        printf("the ring buf is full\r\n");
        
        return -1;
    } 
    
    //加入临界区保护
//    INTX_DISABLE();
    
    p_ring_buf->ring_buf[p_ring_buf->tail] = data;

    //防止越界非法访问
    p_ring_buf->tail = (p_ring_buf->tail+1) % RING_BUF_LEN;
    
    p_ring_buf->lenght++;

//   INTX_ENABLE();     
    
    return 0;
}

/**
 * @brief 环形缓冲区中有效数据的个数
 */
int atk_ring_buf_avail_len (atk_ring_buf_t *p_ring_buf)
{
     return p_ring_buf->lenght;
}

 /**
 * @brief  读取环形缓冲区的数据
 */
int atk_ring_buf_read(atk_ring_buf_t *p_ring_buf, uint8_t *data)
{
   if (p_ring_buf->lenght == 0)    //判断非空
   {
       return -1;
   }
   
   //加入临界区保护
//   INTX_DISABLE();   
   
   //先进先出FIFO，从缓冲区头出
   *data = p_ring_buf->ring_buf[p_ring_buf->head];
   
   //防止越界非法访问
   p_ring_buf->head = (p_ring_buf->head+1) % RING_BUF_LEN; 
   
   p_ring_buf->lenght--;
   
//   INTX_ENABLE(); 
   
   return 0;
}

 /**
 * @brief  从接收缓存里读取指定长度的数据，并释放占用的空间
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
 * @brief  写入接收缓存里读指定长度的数据，并占用的空间
 */
int atk_ring_buf_size_write(atk_ring_buf_t *p_ring_buf, uint8_t *data, int len)
{
    
    int i = 0; 
    
    //缓存空余个数小于写入的长度    
    if (len > (RING_BUF_LEN - p_ring_buf->lenght))
    {
        return -1; 
    }
    
    for (i = 0; i <  len; i++) {
        
         atk_ring_buf_write(p_ring_buf, data[i]);
    }

    return 0;
}




