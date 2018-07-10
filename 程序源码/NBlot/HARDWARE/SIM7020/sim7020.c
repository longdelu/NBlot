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

//用于保存当前正在执行的AT指令
static at_cmd_info_t g_at_cmd;

static struct sim7020_recv  sim7020_recv_desc;
static struct sim7020_send  sim7020_send_desc;

static struct sim7020_dev   g_sim7020_dev;
static struct sim020_status g_sim020_status;  


static struct sim7020_drv_funcs drv_funcs = {
    
    uart_data_tx_poll,
    uart_data_rx_int        
};

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


//******************************************************************************
// fn : cmd_isPass
//
// brief : 判断SIM7020执行的AT指令是否执行成功
//
// param : none
//
// return : none
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


//******************************************************************************
// fn : NB_SendCmd
//
// brief : 通过注册的串口函数向外发送at指令
//
// param : hw_handle ->  硬件操作函数指针
//         cmdHandle -> 将要发送指令信息
//
// return : none
static void sim7020_send_at_cmd(sim7020_handle_t sim7020_handle,at_cmdhandle cmd_handle)
{
    int strLen = 0;
    
    UART_HandleTypeDef *p_huart = sim7020_handle->p_uart_dev->p_huart; 
    
    if (sim7020_handle == NULL || cmd_handle == NULL)
    {
       return;
    }
        
    strLen = cmd_generate(cmd_handle);

    sim7020_handle->p_funcs->sim7020_send_data(p_huart, 
                                               (uint8_t*)sim7020_send_desc.buf, 
                                                strLen, 
                                                cmd_handle->max_timeout);
}

//sim7020初始化 
sim7020_handle_t sim7020_init(uart_handle_t lpuart_handle)
{

     g_sim7020_dev.p_uart_dev = lpuart_handle;
     g_sim7020_dev.p_funcs    = &drv_funcs; 
     
     return &g_sim7020_dev;    
}

//注册sim7020事件回调函数
void lpuart_event_registercb(sim7020_cb cb, void *p_arg)
{  
    if(cb != 0)
    {
        g_sim7020_dev.sim7020_cb  = cb;
        g_sim7020_dev.p_arg       = p_arg;
    }
}


void sim7020_sm_event (int sim7020_event)
{   
    switch(sim7020_event) {
        
    case 0: 

        break;


    case 1:

        break; 
    
   default:
        
        break;
   }    
    
}




