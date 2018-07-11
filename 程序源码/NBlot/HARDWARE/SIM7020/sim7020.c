#include "sim7020.h"
#include "delay.h"

////////////////////////////////////////////////////////////////////////////////// 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F429开发板
//串口1初始化           
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2015/9/7
//版本：V1.5
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//********************************************************************************



//定义sim7020数据收发描述结构体
static struct sim7020_recv  sim7020_recv_desc;
static struct sim7020_send  sim7020_send_desc;

//定义用于保存当前正在执行的AT指令的述结构体
static at_cmd_info_t g_at_cmd;

//定义sim7020设备结构体
static struct sim7020_dev       g_sim7020_dev;

//定义sim7020状态信息结构体
static sim7020_status_nest_t    g_sim702_status;

//定义sim7020固件信息结构体
static sim020_firmware_info_t   g_firmware_info;  

//函数声明
static int  __sim7020_uart_data_tx (void *p_arg, uint8_t *pData, uint16_t size, uint32_t Timeout);
static int  __sim7020_uart_data_rx (void *p_arg, uint8_t *pData, uint16_t size, uint32_t Timeout);
static void __send_uart_callback_handle(void *p_arg);

static struct sim7020_drv_funcs drv_funcs = {
    
    __sim7020_uart_data_tx,
    __sim7020_uart_data_rx,        
};


//设置sim7020事件
void sim7020_event_set (int sim7020_event)
{ 
    g_sim7020_dev.sim7020_event |= sim7020_event;   
}

//获取sim7020事件
int sim7020_event_get (int sim7020_event)
{ 
    return (g_sim7020_dev.sim7020_event & sim7020_event); 
}

//清除sim7020事件
void sim7020_event_clr (int sim7020_event)
{ 
    g_sim7020_dev.sim7020_event ^= sim7020_event;
}

static void __uart_event_cb_handle(void *p_arg)
{    
    uart_dev_t *p_uart_dev = (uart_dev_t *)p_arg; 
    
    if (p_uart_dev->uart_event & UART_NONE_EVENT) {
        
    } 

    if (p_uart_dev->uart_event & UART_TX_EVENT) {
        printf("tx data ok\r\n"); 
        lpuart_event_clr(UART_TX_EVENT); 
    }

    if (p_uart_dev->uart_event & UART_RX_EVENT) {
        printf("rx data ok\r\n");
        
        lpuart_event_clr(UART_RX_EVENT); 
    } 

    if (p_uart_dev->uart_event & UART_TX_TIMEOUT_EVENT) {
        printf("tx data timeout\r\n");
        
        lpuart_event_clr(UART_TX_TIMEOUT_EVENT); 
    } 

    if (p_uart_dev->uart_event & UART_RX_TIMEOUT_EVENT) {
        printf("rx data timeout\r\n");
        
        lpuart_event_clr(UART_RX_TIMEOUT_EVENT); 
    }            
}


//sim7020事件处理函数
void sim7020_event_poll (sim7020_handle_t sim7020_handle)
{          
    //回调注册进来的SIM7020事件处理函数 
    sim7020_handle->sim7020_cb(sim7020_handle->p_arg);   
}

//sim7020 状态处理函数
//sim7020_main_status；sim7020所处的主状态阶段
void sim7020_app_status_poll(int  *sim7020_main_status)
{    
    switch(*sim7020_main_status)
    {
    case SIM7020_NONE:
      {
        
      }
      break;
      
    case SIM7020_INIT:
      {
        printf("sim7020 init start\r\n");

        *sim7020_main_status = SIM7020_END;
      }
      break;
      
    case SIM7020_MODULE_INFO:
      {
         printf("sim7020 get signal start\r\n");

         *sim7020_main_status = SIM7020_END;
      }
      break;
      
    case SIM7020_SIGN:
      {
        printf("sim7020 module info start\r\n");
        *sim7020_main_status = SIM7020_END;
      }
      break;
      
    case SIM7020_UDP_CR:
      {
        printf("udp socket creat start\r\n");  
        //do nothing
        *sim7020_main_status = SIM7020_END;
      }
      break;
      
    case SIM7020_UDP_CL:
      {          
        //do nothing
        printf("udp socket close start\r\n");   
          
        *sim7020_main_status = SIM7020_END;
      }
      break;
      
    case SIM7020_UDP_SEND:
      {
          
        printf("udp send start\r\n");     
        //do nothing
        *sim7020_main_status = SIM7020_END;
      }
      break;
      
    case SIM7020_UDP_RECV:
      {
          
        printf("udp recv start\r\n");    
        //do nothing
        *sim7020_main_status = SIM7020_END; 
      }
      break;
      
    case SIM7020_TCP_CR:
      {
          
        printf("tcp socket creat start\r\n");  
          
        //do nothing
        *sim7020_main_status = SIM7020_END;
      }
      break;
      
    case SIM7020_TCP_CL:
      {
          
        printf("tcp socket close start\r\n");    
        //do nothing
        *sim7020_main_status = SIM7020_END;
      }
      break; 
      
    case SIM7020_TCP_SEND:
      {
        //do nothing
        printf("tcp send start\r\n");    
        *sim7020_main_status = SIM7020_END;
      }
      break;
      
    case SIM7020_TCP_RECV:
      {
          
        printf("tcp recv start\r\n");  
        //do nothing
        *sim7020_main_status = SIM7020_END; 
      }
      break;      
      
    case SIM7020_CoAP_SEVER:
      {
        printf("CoAP Server set start\r\n");

        *sim7020_main_status = SIM7020_END;
      }
      break;
      
    case SIM7020_CoAP_SEND:
      {
        printf("CoAP send start\r\n");
        *sim7020_main_status = SIM7020_END;
      }
      break;
      
    case SIM7020_CoAP_RECV:
      {
        printf("CoAP recv start\r\n");
        *sim7020_main_status = SIM7020_END;        
      }
      break;  
      
    default:
      {
        
      }
      break;
    }
}


static int  __sim7020_uart_data_tx (void *p_arg, uint8_t *pData, uint16_t size, uint32_t Timeout)
{      
    sim7020_handle_t  sim7020_handle = (sim7020_handle_t)p_arg;
    
    UART_HandleTypeDef *p_huart = sim7020_handle->p_uart_dev->p_huart; 
    
    uart_data_tx_poll(p_huart, pData, size, Timeout);           
}

static int  __sim7020_uart_data_rx (void *p_arg, uint8_t *pData, uint16_t size, uint32_t Timeout)
{           
    sim7020_handle_t  sim7020_handle = (sim7020_handle_t)p_arg;
    
    UART_HandleTypeDef *p_huart = sim7020_handle->p_uart_dev->p_huart; 
    
    uart_data_rx_int(p_huart, pData, size, Timeout);                  
}


//将1个字符转换为16进制数字
//chr:字符,0~9/A~F/a~F
//返回值:chr对应的16进制数值
u8 sim7020_chr2hex(u8 chr)
{
    if(chr>='0'&&chr<='9')return chr-'0';
    if(chr>='A'&&chr<='F')return (chr-'A'+10);
    if(chr>='a'&&chr<='f')return (chr-'a'+10); 
    return 0;
}

//将1个16进制数字转换为字符
//hex:16进制数字,0~15;
//返回值:字符
u8 sim7020_hex2chr(u8 hex)
{
    if(hex<=9)return hex+'0';
    if(hex>=10&&hex<=15)return (hex-10+'A'); 
    return '0';
}


//sim7020 at指令初始化
static void at_cmd_param_init(at_cmdhandle cmd_handle,
                              const char *at_cmd,
                              char* argument,
                              cmd_property_t property,
                              uint32_t at_cmd_time_out)
{
    if(cmd_handle == NULL)
    {
      return;
    }
    cmd_handle->cmd_try     = CMD_TRY_TIMES;
    cmd_handle->property    = property;
    cmd_handle->cmd_action  = ACTION_OK;
    cmd_handle->p_atcmd_arg = argument;
    cmd_handle->p_expectres = NULL;
    cmd_handle->have_tried  = 0;
    cmd_handle->max_timeout = at_cmd_time_out;
    cmd_handle->p_atcmd     = at_cmd;
}

//生成sim7020 at指令的字符串及其对应的长度
static int cmd_generate(at_cmdhandle cmd_handle)
{
    int cmdLen = 0;
    if(cmd_handle == NULL)
    {
       return cmdLen;
    }

    memset(sim7020_send_desc.buf,0,NB_UART_SEND_BUF_MAX_LEN);
    sim7020_send_desc.len = 0;

    if(cmd_handle->property == CMD_TEST)
    {
        cmdLen = snprintf(sim7020_send_desc.buf,NB_UART_SEND_BUF_MAX_LEN,
                         "%s=?\r\n",
                         cmd_handle->p_atcmd);
    }
    else if(cmd_handle->property == CMD_READ)
    {
        cmdLen = snprintf(sim7020_send_desc.buf,NB_UART_SEND_BUF_MAX_LEN,
                          "%s?\r\n",
                          cmd_handle->p_atcmd);
    }
    else if(cmd_handle->property == CMD_EXCUTE)
    {
        cmdLen = snprintf(sim7020_send_desc.buf,NB_UART_SEND_BUF_MAX_LEN,
                          "%s\r\n",
                          cmd_handle->p_atcmd);    
    }

    else if(cmd_handle->property == CMD_SET)
    {
        cmdLen = snprintf(sim7020_send_desc.buf,NB_UART_SEND_BUF_MAX_LEN,
                      "%s=%s\r\n",
                      cmd_handle->p_atcmd,cmd_handle->p_atcmd_arg);    
    }
    
    sim7020_send_desc.len = cmdLen;
    
    return cmdLen;
}


//判断at指令是否发送成功
static int8_t at_cmd_isPass(char* buf)
{
  int8_t result = -1;
     
  if(g_at_cmd.p_atcmd == NULL)
  {
    if (strstr(buf,"OK"))
    {
      result = TRUE; ;
    }
    else if (strstr(buf,"ERROR"))
    {
      result = FALSE;
    }
  }
  else
  {
    if(strstr(buf,"OK"))
    {
      if(strstr(buf,g_at_cmd.p_atcmd))
      {
        result = TRUE;
      }
      else
      {
        result = FALSE;
      }
        
    }
    else if(strstr(buf,"ERROR"))
    {
      //+CMEE ERROR
      result = FALSE;
    }
  }

  return result;
}


//通过注册的串口函数向外发送at指令
//sim7020_handle   sim7020_handle设备句柄
//cmd_handle       将要发送指令信息句柄
//note 调用该函数前先构造好命令的参数
static void sim7020_send_at_cmd(sim7020_handle_t sim7020_handle, at_cmdhandle cmd_handle)
{
    int strLen = 0;
        
    if (sim7020_handle == NULL || cmd_handle == NULL)
    {
       return;
    }
        
    strLen = cmd_generate(cmd_handle);

    sim7020_handle->p_drv_funcs->sim7020_send_data(sim7020_handle, 
                                                   (uint8_t*)sim7020_send_desc.buf, 
                                                   strLen, 
                                                   cmd_handle->max_timeout);
}

//sim7020初始化 
sim7020_handle_t sim7020_init(uart_handle_t lpuart_handle)
{
     //填充设备结构体
     g_sim7020_dev.p_uart_dev    = lpuart_handle;
     g_sim7020_dev.p_drv_funcs   = &drv_funcs; 
     g_sim7020_dev.firmware_info = &g_firmware_info;
     g_sim7020_dev.sim702_status = &g_sim702_status;    
    
     /* 注册sim7020串口收发事件回调函数 */
     lpuart_event_registercb(__uart_event_cb_handle, lpuart_handle);     
    
     return &g_sim7020_dev;    
}

//注册sim7020事件回调函数
void sim7020_event_registercb(sim7020_cb cb, void *p_arg)
{  
    if(cb != 0)
    {
        g_sim7020_dev.sim7020_cb  = cb;
        g_sim7020_dev.p_arg       = p_arg;
    }
}
   





