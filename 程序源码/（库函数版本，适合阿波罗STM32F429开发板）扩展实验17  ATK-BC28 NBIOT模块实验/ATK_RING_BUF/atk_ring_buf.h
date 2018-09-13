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
 * @brief  定义环形缓冲区数据缓存的最大长度
 */    
#define RING_BUF_LEN                  512         

    
/**
 * @brief  环形缓冲区结构体
 */    
typedef struct atk_ring_buf_t
{
    uint32_t head;                 
    uint32_t tail;
    uint32_t lenght;
    uint8_t  ring_buf[RING_BUF_LEN];     
}atk_ring_buf_t;
  

/**
 * @brief  初始化环形缓冲区的相关信息
 * @param  p_ring_buf : 指向了缓形缓冲区的指针
 * @retval 0
 */
int atk_ring_buf_init (atk_ring_buf_t *p_ring_buf);

/**
 * @brief 往环形缓冲区写数据
 * @param  p_ring_buf : 指向了缓形缓冲区的指针
 * @param  data       : 写入的数据
 * @retval 0  写入成功  -1 写入失败
 */
int atk_ring_buf_write(atk_ring_buf_t *p_ring_buf, uint8_t data);


 /**
  * @brief  读取环形缓冲区的数据
  * @param  p_ring_buf : 指向了缓形缓冲区的指针
  * @param  data       : 读取数据
  * @retval 0  读取成功  -1 读取失败
  */
int atk_ring_buf_read(atk_ring_buf_t *p_ring_buf, uint8_t *data);

/**
 * @brief 读取环形缓冲区中有效数据的个数
 * @param  p_ring_buf : 指向了缓形缓冲区的指针
 * @retval 缓形缓冲区的有效数据长度
 */
int atk_ring_buf_avail_len (atk_ring_buf_t *p_ring_buf);

 /**
  * @brief  从接收缓存里读取指定长度的数据，并释放占用的空间
  * @param  p_ring_buf : 指向了缓形缓冲区的指针
  * @param  data       : 读取数据的缓冲区地址
  * @param  len        : 读取数据的长度
  * @retval 0  读取成功  -1 读取失败   
  */
int atk_ring_buf_size_read(atk_ring_buf_t *p_ring_buf, uint8_t *data, int len);


 /**
  * @brief  写入接收缓存里读指定长度的数据，并占用的空间
  * @param  p_ring_buf : 指向了缓形缓冲区的指针
  * @param  data       : 写入数据的缓冲区地址
  * @param  len        : 写入数据的有效长度
  * @retval 0  写入成功  -1 写入失败  
  */
int atk_ring_buf_size_write(atk_ring_buf_t *p_ring_buf, uint8_t *data, int len);  
                         

#ifdef __cplusplus
} 
#endif

#endif //end of _ATK_RING_BUF_H_
