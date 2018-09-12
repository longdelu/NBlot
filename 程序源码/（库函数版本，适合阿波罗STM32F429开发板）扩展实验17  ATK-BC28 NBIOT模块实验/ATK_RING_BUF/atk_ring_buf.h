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

//定义环形缓冲区最大接收字节数                               
#define RING_BUF_LEN                  512         

//环形缓冲区结构体
typedef struct atk_ring_buf_t
{
    uint32_t head;           
    uint32_t tail;
    uint32_t lenght;
    uint8_t  ring_buf[RING_BUF_LEN];  
    
}atk_ring_buf_t;
  

/**
 * @brief 初始化环形缓冲区的相关信息
 */
int atk_ring_buf_init (atk_ring_buf_t *p_ring_buf);

/**
 * @brief 往环形缓冲区写数据
 */
int atk_ring_buf_write(atk_ring_buf_t *p_ring_buf, uint8_t data);


 /**
 * @brief  读取环形缓冲区的数据
 */
int atk_ring_buf_read(atk_ring_buf_t *p_ring_buf, uint8_t *data);

/**
 * @brief 环形缓冲区中有效数据的个数
 */
int atk_ring_buf_avail_len (atk_ring_buf_t *p_ring_buf);

 /**
 * @brief  从接收缓存里读取指定长度的数据，并释放占用的空间
 */
int atk_ring_buf_size_read(atk_ring_buf_t *p_ring_buf, uint8_t *data, int len);


 /**
 * @brief  写入接收缓存里读指定长度的数据，并占用的空间
 */
int atk_ring_buf_size_write(atk_ring_buf_t *p_ring_buf, uint8_t *data, int len);  
                         

#ifdef __cplusplus
} 
#endif

#endif //end of _ATK_RING_BUF_H_
