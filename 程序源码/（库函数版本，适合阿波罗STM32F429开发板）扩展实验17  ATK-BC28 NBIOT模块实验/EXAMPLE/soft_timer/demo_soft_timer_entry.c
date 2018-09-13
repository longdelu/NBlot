 /************************************************
 Copyright (c) 广州市星翼电子科技有限公司 2014-2024
 All rights reserved 
 ALIENTEK 阿波罗STM32F429开发板 
 NBIOT 软件定时器实验
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/ 

#include "atk_sys.h"
#include "atk_soft_timer.h"

static struct atk_soft_timer timer1;
static struct atk_soft_timer timer2;

static void timer1_callback (void *p_arg)
{
    printf("timer1 timeout!\r\n");
}

static void timer2_callback (void *p_arg)
{
    printf("timer2 timeout!\r\n");
}

/**
  * @brief  The demo soft timer  entry entry point.
  *
  * @retval None
  */
void demo_soft_timer_entry(void)
{
    atk_soft_timer_init(&timer1, timer1_callback, NULL, 1000, 1000); //1s loop
    atk_soft_timer_start(&timer1);
    
    atk_soft_timer_init(&timer2, timer2_callback, NULL, 500, 0);     //50ms delay
    atk_soft_timer_start(&timer2);
    
    while(1) 
    {        
        ;
    }
}
