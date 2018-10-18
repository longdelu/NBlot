/*
 * Copyright (c) ATK
 * All rights reserved
 */
 
#ifndef _ATK_SOFT_TIMER_H_
#define _ATK_SOFT_TIMER_H_

#include "stdint.h"

#ifdef __cplusplus  
extern "C" {  
#endif  
  
typedef struct atk_soft_timer {
    uint32_t timeout;                 //��ʱ
    uint32_t repeat;                  //���¼��� 
    void (*timeout_cb)(void *p_arg);  //�ص�����
    void  *p_arg;                     //�ص���������
    struct atk_soft_timer *next;
}atk_soft_timer_t;  
  

void atk_soft_timer_init (struct atk_soft_timer  *handle, 
                          void(*timeout_cb)(void *p_arg),                            
                          void *p_arg, 
                          uint32_t timeout, 
                          uint32_t repeat);
                          
/**
  * @brief  change the soft timer timeout val
  * @param  handle: target handle strcut.
  * @retval 0: succeed. -1: already exist.
  * @note   the api will add the soft timer into list when change the soft timer timeout val
  */
int atk_soft_timer_timeout_change(struct atk_soft_timer *handle, uint32_t timeout);                        
                         
int  atk_soft_timer_start (struct atk_soft_timer *handle);
void atk_soft_timer_stop (struct atk_soft_timer  *handle);
void atk_soft_timer_ticks (void);
void atk_soft_timer_poll (void);
                         

#ifdef __cplusplus
} 
#endif

#endif //end of _ATK_SOFT_TIMER_H_
