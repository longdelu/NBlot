/*
 * Copyright (c) atk
 * All rights reserved
 */
#include "sys.h"
#include "atk_soft_timer.h"

//timer handle list head.
static struct Timer* head_handle = NULL;

//Timer ticks
static uint32_t _timer_ticks = 0;

/**
  * @brief  Initializes the timer struct handle.
  * @param  handle: the timer handle strcut.
  * @param  timeout_cb: timeout callback.
  * @param  repeat: repeat interval time.
  * @retval None
  */
void atk_soft_timer_init(struct Timer* handle, 
                         void(*timeout_cb)(void *p_arg),                            
                         void *p_arg, 
                         uint32_t timeout, 
                         uint32_t repeat)
{
	// memset(handle, sizeof(struct Timer), 0);
	handle->timeout_cb = timeout_cb;
    handle->p_arg   = p_arg;
	handle->timeout = _timer_ticks + timeout;
	handle->repeat  = repeat;
}

/**
  * @brief  Start the timer work, add the handle into work list.
  * @param  btn: target handle strcut.
  * @retval 0: succeed. -1: already exist.
  */
int atk_soft_timer_start(struct Timer* handle)
{
	struct Timer* target = head_handle;
    
    //确保不会重复注册
	while(target) {
        
		if(target == handle) return -1;	//already exist.
        
		target = target->next;
	}
    
    //临界区保护
    INTX_DISABLE();
    
    //元素添加在链表尾部，从左往右链表长度增加
	handle->next = head_handle;
    
    //确保这个指针指向表头,即链表最左边的第一个元素
	head_handle = handle;
    
    INTX_ENABLE();    
    
	return 0;
}

/**
  * @brief  Stop the timer work, remove the handle off work list.
  * @param  handle: target handle strcut.
  * @retval None
  */
void atk_soft_timer_stop(struct Timer* handle)
{
	struct Timer** curr;
	for(curr = &head_handle; *curr; ) {
		struct Timer* entry = *curr;
		if (entry == handle) {
            
            //临界区保护
            INTX_DISABLE();
            //移除链表元素,强制改变相等那个元素的next指向
			*curr = entry->next;
//            break;
//			  free(entry);
            INTX_ENABLE(); 
            
		} else {
			curr = &entry->next;
        }
	}
}

/**
  * @brief  atk soft timer main loop.
  * @param  None.
  * @retval None
  */
void atk_soft_timer_loop()
{
	struct Timer* target;
    
	for(target=head_handle; target; target=target->next) {
        
		if((int)((u32)(target->timeout -_timer_ticks)) <= 0) {
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
  * @brief  background ticks, timer repeat invoking interval 1ms.
  * @param  None.
  * @retval None.
  */
void atk_soft_timer_ticks()
{
	_timer_ticks++;
    
    atk_soft_timer_loop();
}




