
/**
  *
  * @file           : demo_led_entry.c
  * @brief          : 串口轮询收发数据实验
  */
/* Includes ------------------------------------------------------------------*/
#include "sys.h"
#include "led.h"
#include "delay.h"
#include "nblot_usart.h"
#include "stm32l4xx_hal.h"

/**
  * @brief  The uart poll application entry point.
  *
  * @retval None
  */
void demo_uart_poll_entry(void)
{ 
  uint8_t buf[32];  
    
  lpuart1_init(115200);  
    
  uart_data_tx_poll(&hlpuart1, (uint8_t *)"nblot_uart test\r\n", sizeof("nblot_uart test\r\n"), HAL_MAX_DELAY);   
        
  uart_data_rx_poll(&hlpuart1, buf, sizeof("nblot_uart rx tx test ok") - 1, HAL_MAX_DELAY);   
    
  uart_data_tx_poll(&hlpuart1, buf, sizeof("nblot_uart rx tx test ok") - 1, HAL_MAX_DELAY);
    
  while (1)
  {
     LED0_Toggle;  
     delay_ms(100);  
  }
  
  /* USER CODE END3 */

}




/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
