#include "sys.h"
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

void demo_soft_timer_entry(void)
{
    atk_soft_timer_init(&timer1, timer1_callback, NULL, 1000, 1000); //1s loop
    atk_soft_timer_start(&timer1);
    
    atk_soft_timer_init(&timer2, timer2_callback, NULL, 50, 0); //50ms delay
    atk_soft_timer_start(&timer2);
    
    while(1) {        
        ;
    }
}
