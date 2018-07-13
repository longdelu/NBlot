/*
 * Copyright (c) atk
 * All rights reserved
 */
#include "sys.h"
#include "atk_soft_timer.h"

//atk_soft_timer handle list head.
static struct atk_soft_timer *head_handle = NULL;

//atk_soft_timer ticks
volatile static uint32_t _timer_ticks = 0;

/**
  * @brief  Initializes the atk_soft timer struct handle.
  * @param  handle    : the atk soft timer handle strcut.
  * @param  timeout_cb: timeout callback.
  * @param  p_arg     : timeout callback par.
  * @param  repeat    : repeat interval time.
  * @retval None
  */
void atk_soft_timer_init(struct atk_soft_timer *handle, 
                         void (*timeout_cb)(void *p_arg),                            
                         void  *p_arg, 
                         uint32_t timeout, 
                         uint32_t repeat)
{
    // memset(handle, sizeof(struct Timer), 0);
    handle->timeout_cb = timeout_cb;
    handle->p_arg   = p_arg;
    handle->timeout = timeout;
    handle->repeat  = repeat;
}


/**
  * @brief  change the soft timer timeout val
  * @param  handle: target handle strcut.
  * @retval 0: succeed. -1: already exist.
  * @note   the api will add the soft timer into list when change the soft timer timeout val
  */
int atk_soft_timer_timeout_change(struct atk_soft_timer *handle, uint32_t timeout)
{   
    struct atk_soft_timer *target = head_handle;
    
    if (handle == NULL) {
        
        return -1;
        
    }
    
    //改变当前的超时值
    handle->timeout = _timer_ticks + timeout; 
    
    
    //如果重置超时，软定时器并没有加入链表，则加入链表
    while(target) {
        
        if(target == handle) return -1;    //already exist.
        
        target = target->next;
    }
    
    //加入链表元素使用临界区保护
    INTX_DISABLE();
    

    //元素添加在链表尾部，从左往右链表长度增加
    handle->next = head_handle;
    
    //确保这个指针指向表头,即链表最左边的第一个元素
    head_handle = handle;
    
    INTX_ENABLE();     
    
    
    return 0;       
}


/**
  * @brief  Start the atk soft timer work, add the handle into work list.
  * @param  btn: target handle strcut.
  * @retval 0: succeed. -1: already exist.
  */
int atk_soft_timer_start(struct atk_soft_timer *handle)
{
    struct atk_soft_timer *target = head_handle;
    
    //确保启动时的延时正确
    handle->timeout = _timer_ticks + handle->timeout;
        
    //确保不会重复注册
    while(target) {
        
        if(target == handle) return -1;    //already exist.
        
        target = target->next;
    }
    
    //加入链表元素使用临界区保护
    INTX_DISABLE();
    

    //元素添加在链表尾部，从左往右链表长度增加
    handle->next = head_handle;
    
    //确保这个指针指向表头,即链表最左边的第一个元素
    head_handle = handle;
    
    INTX_ENABLE();    
    
    return 0;
}

/**
  * @brief  Stop the atk soft timer work, remove the handle off work list.
  * @param  handle: target handle strcut.
  * @retval None
  */
void atk_soft_timer_stop(struct atk_soft_timer *handle)
{
    struct atk_soft_timer **curr = NULL;
    
    for(curr = &head_handle; *curr; ) {
        struct atk_soft_timer *entry = *curr;
        if (entry == handle) {
            
            //临界区保护
            INTX_DISABLE();
            //移除链表元素,强制改变相等那个元素的next指向
            *curr = entry->next;
//              free(entry);
            INTX_ENABLE();

//            break;            
            
        } else {
            curr = &entry->next;
        }
    }
}

/**
  * @brief  atk soft timer poll.
  * @param  None.
  * @retval None
  */
void atk_soft_timer_poll()
{
    struct atk_soft_timer *target = NULL;
    
    for(target=head_handle; target; target=target->next) {
        
        //fix bug when ticks overflow
        if((int)((uint32_t)(target->timeout -_timer_ticks)) <= 0) {
            if(target->repeat == 0) {
                atk_soft_timer_stop(target);
            } else {
                target->timeout = _timer_ticks + target->repeat;
            }
            target->timeout_cb(target->p_arg);
        }
    }
}

/**
  * @brief  background ticks, atk soft timer repeat invoking interval 1ms.
  * @param  None.
  * @retval None.
  * @note   This fuction must call by a tick nvic handler
  */
void atk_soft_timer_ticks()
{
    _timer_ticks++;
   
    // 处理超时事件    
    atk_soft_timer_poll();
}




