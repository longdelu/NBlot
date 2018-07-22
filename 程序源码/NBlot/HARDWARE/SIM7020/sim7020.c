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


#define SIM7020_DEBUG                   
#ifdef SIM7020_DEBUG
#define SIM7020_DEBUG_INFO(...)    (int)printf(__VA_ARGS__)
#else
#define SIM7020_DEBUG_INFO(...)
#endif


//简要逻辑
//对命令的响应后的解析会调用at_cmd_result_parse这个函数
//如果正确，会根据命令的属性来决定是否需要执行下一条命令
//发送消息时，调用sim7020_msg_send，子状态的命令此时并没有发生改变
//直到调用at_cmd_next构造好下一条指令的时候才会改变子状态
//当at_cmd_next返回FALSE时，表明所有的子状态都已经结束了，此时会复用状态机的逻辑

//定义sim7020数据收发描述结构体
static struct sim7020_recv  g_sim7020_recv_desc;
static struct sim7020_send  g_sim7020_send_desc;

//定义用于保存当前正在执行的AT指令的述结构体
static at_cmd_info_t g_at_cmd;                                              

//定义sim7020设备结构体
static struct sim7020_dev       g_sim7020_dev;

//定义sim7020状态信息结构体
static sim7020_status_nest_t    g_sim7020_status;

//定义sim7020固件信息结构体
static sim020_firmware_info_t   g_firmware_info; 

//定义socket信息结构体
static sim7020_socket_info_t    g_socket_info[5];

//驱动函数函数声明
static int  __sim7020_uart_data_tx (void *p_arg, uint8_t *pData, uint16_t size, uint32_t Timeout);
static int  __sim7020_uart_data_rx (void *p_arg, uint8_t *pData, uint16_t size, uint32_t Timeout);


//串口事件回调处理函数
static void __uart_event_cb_handle(void *p_arg);

//判读发送的AT指令执行结果
static int8_t at_cmd_result_parse(char* buf);

//指令响应结果处理
uint8_t sim7020_response_handle (sim7020_handle_t sim7020_handle, uint8_t cmd_response);

//产生下一条AT指令
static uint8_t at_cmd_next (void);
   
//sim7020事件通知
static uint8_t sim7020_event_notify (sim7020_handle_t sim7020_handle, char *buf);

//发送消息与应用层交互
static void sim7020_msg_send (sim7020_handle_t sim7020_handle, char**buf, int8_t is_ok);

//sim7020发送AT指令
//sim7020_handle   sim7020_handle设备句柄
//cmd_handle       将要发送指令信息句柄
//note 调用该函数前先构造好命令的参数
static int sim7020_at_cmd_send(sim7020_handle_t sim7020_handle, at_cmdhandle cmd_handle);

//sim7020接收数据
static int sim7020_data_recv(sim7020_handle_t sim7020_handle, uint8_t *pData, uint16_t size, uint32_t Timeout);

static struct sim7020_drv_funcs drv_funcs = {    
    __sim7020_uart_data_tx,
    __sim7020_uart_data_rx,        
};

//复位接收缓存
static void sim7020_recv_buf_reset(void)
{   
    memset(&g_sim7020_recv_desc, 0, sizeof(struct sim7020_recv));
}

//复位执行进程
static void sim7020_status_reset(void)
{
    g_sim7020_status.main_status  = SIM7020_NONE;
    g_sim7020_status.sub_status   = SIM7020_SUB_NONE;
}

//设置执行进程
static void sim7020_status_set (sim7020_main_status_t  main_status, int  sub_status)
{
    g_sim7020_status.main_status  = main_status;
    g_sim7020_status.sub_status   = sub_status;
}

//设置sim7020事件
void sim7020_event_set (sim7020_handle_t sim7020_handle, int sim7020_event)
{ 
    sim7020_handle->sim7020_event |= sim7020_event;   
}

//获取sim7020事件
int sim7020_event_get (sim7020_handle_t sim7020_handle,  int sim7020_event)
{ 
    return (sim7020_handle->sim7020_event & sim7020_event); 
}

//清除sim7020事件
void sim7020_event_clr (sim7020_handle_t sim7020_handle, int sim7020_event)
{ 
    sim7020_handle->sim7020_event ^= sim7020_event;
}

//串口事件回调处理函数
static void __uart_event_cb_handle (void *p_arg)
{    
    sim7020_handle_t  sim7020_handle = (sim7020_handle_t)p_arg; 
    
    uart_dev_t       *p_uart_dev     = sim7020_handle->p_uart_dev;
  
    int size = g_sim7020_recv_desc.len;
    
    if (p_uart_dev->uart_event & UART_TX_EVENT)
    {
               
        SIM7020_DEBUG_INFO("sim7020 uart tx ok %s", g_sim7020_send_desc.buf);  

        lpuart_event_clr(p_uart_dev, UART_TX_EVENT);                 
    }

    if (p_uart_dev->uart_event & UART_RX_EVENT)
    {               
        size = uart_ring_buf_avail_len(p_uart_dev);
        
        //从缓冲区当中读取数据
        if (size > 0)
        {                                  
            sim7020_data_recv(sim7020_handle, (uint8_t*)(&g_sim7020_recv_desc.buf[g_sim7020_recv_desc.len]), size, 0);
                     
            //产生异步事件等待处理
            sim7020_event_notify(sim7020_handle, g_sim7020_recv_desc.buf);
            
            SIM7020_DEBUG_INFO("sim7020 uart rx ok %s\r\n", &g_sim7020_recv_desc.buf[g_sim7020_recv_desc.len]);
          
            g_sim7020_recv_desc.len = g_sim7020_recv_desc.len + size;
            
        }
                                                                                 
        lpuart_event_clr(p_uart_dev, UART_RX_EVENT); 
    } 

    //发生发送超时事件，说明指令有可能没有发送出去，或者发送过程中出错
    //或者模块工作异常，没有回应命令的数据
    if (p_uart_dev->uart_event & UART_TX_TIMEOUT_EVENT) 
    {
        
        SIM7020_DEBUG_INFO("sim7020 uart tx timeout %s", g_sim7020_send_desc.buf);  

        sim7020_event_set(sim7020_handle, SIM7020_TIMEOUT_EVENT);
      
        lpuart_event_clr(p_uart_dev, UART_TX_TIMEOUT_EVENT); 
    } 

    //如果使用非超时成帧，此事件理论上不会发生
    if (p_uart_dev->uart_event & UART_RX_TIMEOUT_EVENT) 
    {       
        size = uart_ring_buf_avail_len(p_uart_dev);
        
        //超时成帧      
        if (g_sim7020_dev.frame_format == 1) 
        {
            if (size > 0)
            {               
                sim7020_data_recv(sim7020_handle, (uint8_t*)(&g_sim7020_recv_desc.buf[g_sim7020_recv_desc.len]), size, 0);
                                             
                //产生异步事件等待处理
                sim7020_event_notify(sim7020_handle, g_sim7020_recv_desc.buf);
              
                g_sim7020_recv_desc.len = g_sim7020_recv_desc.len + size;
            }
                                        
       //不在超时成帧的状态下，代表的确发生了超时事件      
       } else {
         
           sim7020_event_set(sim7020_handle, SIM7020_TIMEOUT_EVENT);
       }
       
       SIM7020_DEBUG_INFO("sim7020 uart rx timeout %s\r\n", &g_sim7020_recv_desc.buf[g_sim7020_recv_desc.len]);  
       
       lpuart_event_clr(p_uart_dev, UART_RX_TIMEOUT_EVENT);
    }            
}

//uint8_t tmp_buf[513];
//sim7020事件处理函数
int sim7020_event_poll(sim7020_handle_t sim7020_handle)
{
    char *at_response_par[AT_CMD_RESPONSE_PAR_NUM_MAX] = {0};
    
    char *p_revc_buf_tmp = g_sim7020_recv_desc.buf;
    
    uint8_t index = 0;
        
    int8_t next_cmd = 0;
    
    int8_t cmd_is_pass = 0;
    
    static int8_t recv_cnt = 0;
            
    if (sim7020_handle->sim7020_event & SIM7020_RECV_EVENT) 
    {   
        SIM7020_DEBUG_INFO("%s recv event\r\n", g_at_cmd.p_atcmd);
      
        cmd_is_pass = at_cmd_result_parse(g_sim7020_recv_desc.buf);
        
        //命令响应结果     
        if (cmd_is_pass == AT_CMD_RESULT_OK) 
        {

            //接收在超时时间内正常完成，停止接收超时  
            atk_soft_timer_stop(&sim7020_handle->p_uart_dev->uart_rx_timer); 
          
            recv_cnt=0; 
          
            //提取AT指令返回的参数,在使用strok期间，不允许改变缓冲区的内容，中间出现再多的\r\n，也只会当做一个来处理
            while((at_response_par[index] = strtok(p_revc_buf_tmp,"\r\n")) != NULL)
            {
                index++;
                p_revc_buf_tmp = NULL;
                
                if (index >= (AT_CMD_RESPONSE_PAR_NUM_MAX - 1))
                {
                    break;
                }  
            }

            if (index == 0)
            {
                //数据解析出错，重新发送该命令
                next_cmd = sim7020_response_handle(sim7020_handle, FALSE);
              
                //清缓存            
                sim7020_recv_buf_reset();                            
                                        
                sim7020_event_clr(sim7020_handle, SIM7020_RECV_EVENT); 
                            
                //未收到正确的数据帧
                return  SIM7020_ERROR;
            }            
                                                            
            SIM7020_DEBUG_INFO("%s cmd excute ok\r\n", g_at_cmd.p_atcmd);
          
            //代表命令发送成功了并得到正确的响应
            sim7020_msg_send(sim7020_handle, at_response_par, TRUE);
              
            next_cmd = sim7020_response_handle(sim7020_handle, TRUE);
            
            //清缓存            
            sim7020_recv_buf_reset();
       
        }       
        else if(cmd_is_pass == AT_CMD_RESULT_ERROR)
        {                            
            next_cmd = sim7020_response_handle(sim7020_handle, FALSE);     
        
            if (g_at_cmd.cmd_action & ACTION_ERROR_AND_TRY)
            {                                   
                SIM7020_DEBUG_INFO("%s cmd is failed and try\r\n", g_at_cmd.p_atcmd);
                
                at_response_par[AT_CMD_RESPONSE_PAR_NUM_MAX - 1] = (char*)g_at_cmd.p_atcmd;
                
                //通知上层应用，此动作执行失败,但会重试
                sim7020_msg_send(sim7020_handle, &at_response_par[AT_CMD_RESPONSE_PAR_NUM_MAX - 1], SIM7020_ERROR_RETRY);
                
            } 
            else if (g_at_cmd.cmd_action & ACTION_ERROR_BUT_NEXT)
            {               
                at_response_par[AT_CMD_RESPONSE_PAR_NUM_MAX - 1] = "error but next\r\n";
               
                SIM7020_DEBUG_INFO("%s cmd is failed and next\r\n", g_at_cmd.p_atcmd);
                
                //通知上层应用，此动作执行失败后跳过该命令执行
                sim7020_msg_send(sim7020_handle, &at_response_par[AT_CMD_RESPONSE_PAR_NUM_MAX - 1], SIM7020_ERROR_NEXT);
                
            }
            else 
            {                
                SIM7020_DEBUG_INFO("%s cmd is failed and exit\r\n", g_at_cmd.p_atcmd);        
                at_response_par[AT_CMD_RESPONSE_PAR_NUM_MAX - 1] = (char*)g_at_cmd.p_atcmd;
                
                //通知上层应用，此动作执行失败后跳过该命令执行
                sim7020_msg_send(sim7020_handle, &at_response_par[AT_CMD_RESPONSE_PAR_NUM_MAX - 1], FALSE);  

                //复位状态标志
                sim7020_status_reset();              

            } 
                    
            //清缓存            
            sim7020_recv_buf_reset();
            
            recv_cnt=0; 
            
            //接收在超时时间内正常完成，停止接收超时  
            atk_soft_timer_stop(&sim7020_handle->p_uart_dev->uart_rx_timer); 
        } 

        else if (cmd_is_pass == AT_CMD_RESULT_CONTINUE)
        {
          
            //命令未执行完成，正常情况下，收到的的是命令回显, 接下来的还是当前命令响应数据的接收, 重新启动接收超时               
            atk_soft_timer_timeout_change(&sim7020_handle->p_uart_dev->uart_rx_timer, 1000);
          
            //处于TCP/UDP创建状态时
            if (g_sim7020_status.main_status == SIM7020_TCPUDP_CR) {
                //通知上层应用，获取相关的信息
               sim7020_msg_send(sim7020_handle, at_response_par, SIM7020_ERROR_CONTINUE);
            }          
            recv_cnt=0; 

            //命令未完成            
        }       
        else 
        {
         
            recv_cnt++;          
          
            //理论上不会运行到这里, 运行到这里，有两种情况，
            //一种是表示传输出错，收到的数据都是乱码        
            //第二种是IDLE成帧判断不是那么准确，没有收完也以为一帧结束了
            if (recv_cnt > AT_CMD_RESPONSE_PAR_NUM_MAX)                         
            {              
               //收到的是乱码,强制接收结束
               next_cmd = sim7020_response_handle(sim7020_handle, FALSE);
               //清缓存            
               sim7020_recv_buf_reset();                
            }
            else 
            {
              
               //命令未完成,收到的的是命令回显当中其中的一部分                
//               atk_soft_timer_timeout_change(&sim7020_handle->p_uart_dev->uart_rx_timer, 1000);
             
            }              
                               
        }            
        
        sim7020_event_clr(sim7020_handle, SIM7020_RECV_EVENT); 
    }

    if (sim7020_handle->sim7020_event & SIM7020_TIMEOUT_EVENT) 
    {        
        //超时处理，尝试重发命令        
        next_cmd = sim7020_response_handle(sim7020_handle, FALSE);
      
        //通知上层应用，此动作执行超时
        if (g_at_cmd.cmd_action & ACTION_ERROR_AND_TRY) 
        {           
            at_response_par[AT_CMD_RESPONSE_PAR_NUM_MAX - 1] = (char *)g_at_cmd.p_atcmd;
           
            SIM7020_DEBUG_INFO("%s cmd not repsonse or send failed\r\n", g_at_cmd.p_atcmd);
                               
            //通知上层应用，此动作执行失败后跳过该命令执行
            sim7020_msg_send(sim7020_handle, &at_response_par[AT_CMD_RESPONSE_PAR_NUM_MAX - 1], SIM7020_ERROR_NEXT);            
        } 
        else 
        {            
            SIM7020_DEBUG_INFO("%s cmd is failed and exit\r\n", g_at_cmd.p_atcmd);        
            at_response_par[AT_CMD_RESPONSE_PAR_NUM_MAX - 1] = (char*)g_at_cmd.p_atcmd;
            
            //通知上层应用，此动作执行失败后跳过该命令执行
            sim7020_msg_send(sim7020_handle, &at_response_par[AT_CMD_RESPONSE_PAR_NUM_MAX - 1], FALSE);  

            //复位状态标志
            sim7020_status_reset();              
        } 
        
        //清除垃圾数据      
        sim7020_recv_buf_reset();      
              
        //清相关事件
        sim7020_event_clr(sim7020_handle, SIM7020_RECV_EVENT);                                         
        sim7020_event_clr(sim7020_handle, SIM7020_TIMEOUT_EVENT); 
    } 

    if (sim7020_handle->sim7020_event & SIM7020_REG_STA_EVENT) 
    {      
        SIM7020_DEBUG_INFO("creg event ok\r\n");
                             
        at_response_par[AT_CMD_RESPONSE_PAR_NUM_MAX - 1] = "creg ok";        
          
        //通知上层应用网络注册结果
        sim7020_msg_send(sim7020_handle, &at_response_par[AT_CMD_RESPONSE_PAR_NUM_MAX - 1], TRUE);        

        //处理决定是否执行下一条命令        
        next_cmd = sim7020_response_handle(sim7020_handle, TRUE);
        
        sim7020_event_clr(sim7020_handle,   SIM7020_REG_STA_EVENT); 
      
        //清除缓存数据    
        sim7020_recv_buf_reset();   
    }
    
    if (sim7020_handle->sim7020_event & SIM7020_TCP_RECV_EVENT) 
    {
        SIM7020_DEBUG_INFO("tcp recv event ok\r\n");
      
        //通知上层应用接收到TCP数据
        sim7020_msg_send(sim7020_handle, NULL, TRUE);     
        
        sim7020_event_clr(sim7020_handle,SIM7020_TCP_RECV_EVENT);
      
        //清除缓存数据    
        sim7020_recv_buf_reset();       
    }

    if (sim7020_handle->sim7020_event & SIM7020_UDP_RECV_EVENT) 
    {
        SIM7020_DEBUG_INFO("udp recv event ok\r\n");
      
        //通知上层应用网络注册结果
        sim7020_msg_send(sim7020_handle, NULL, TRUE);    
        
        sim7020_event_clr(sim7020_handle, SIM7020_UDP_RECV_EVENT); 
      
        //清除缓存数据    
        sim7020_recv_buf_reset();  
    }
        
    if (sim7020_handle->sim7020_event & SIM7020_COAP_RECV_EVENT) 
    {
        
        SIM7020_DEBUG_INFO("coap recv event ok\r\n");
             
        sim7020_event_clr(sim7020_handle, SIM7020_COAP_RECV_EVENT); 
    }
    
    if (sim7020_handle->sim7020_event & SIM7020_SOCKET_ERR_EVENT) 
    {       
        SIM7020_DEBUG_INFO("socket err event\r\n");
      
        //通知上层应用SOCKET失败了
        sim7020_msg_send(sim7020_handle, NULL, TRUE);        
              
        sim7020_event_clr(sim7020_handle, SIM7020_SOCKET_ERR_EVENT); 
    }    

    //根据事件及状态判断是否需要执行下一条命令
    if(next_cmd)
    {
        //执行下一条命令
        if (at_cmd_next())
        {
            sim7020_at_cmd_send(sim7020_handle, &g_at_cmd);
        }
        
        //返回FALSE表示子进程已经结束了
        else
        {
            //代表该主状态下所有的子状态命令已经完在了
            sim7020_msg_send(sim7020_handle, NULL,TRUE);

            //复位状态标志
            sim7020_status_reset();
        }     
    }
    
//    if ((!sim7020_handle->sim7020_event))
//    {
//        SIM7020_DEBUG_INFO("g_recv_buf %s \r\n", g_sim7020_recv_desc.buf);
//    }

    return SIM7020_OK;    
}

static int  __sim7020_uart_data_tx (void *p_arg, uint8_t *pData, uint16_t size, uint32_t Timeout)
{  
    int ret = 0;
    
    sim7020_handle_t  sim7020_handle = (sim7020_handle_t)p_arg;
    
    uart_handle_t uart_handle = sim7020_handle->p_uart_dev; 
    
    ret = uart_data_tx_poll(uart_handle, pData, size, Timeout); 

    return ret;    
}

static int  __sim7020_uart_data_rx (void *p_arg, uint8_t *pData, uint16_t size, uint32_t Timeout)
{  
    int ret = 0;
    
    //超时参数保留使用
    (void)Timeout;
    
    sim7020_handle_t  sim7020_handle = (sim7020_handle_t)p_arg;
    
    uart_handle_t uart_handle = sim7020_handle->p_uart_dev;

    uart_ring_buf_read(uart_handle, pData, size);    
    
    return ret;    
}

//将1个字符转换为10进制数字
//chr:字符,0~9/A~F/a~F
//返回值:chr对应的10进制数值
u8 sim7020_chr2hex(u8 chr)
{
    if(chr>='0'&&chr<='9')
    {
      return chr - '0';
    }
    else if(chr>='A'&&chr<='F')
    {
      return (chr-'A'+10);
    }
    else if(chr>='a'&&chr<='f')
    {
      return (chr-'a'+10); 
    }
    else
    {
      
      return 0;
    }
                
}



//将1个16进制数字转换为字符
//hex:16进制数字,0~15;
//返回值:字符
u8 sim7020_hex2chr(u8 hex)
{
    if(hex<=9)
    {
      return hex+'0';
    }
    else if(hex>=10&&hex<=15)
    {
      return (hex-10+'A'); 
    }
    else 
    {
        
    }      
    
    return '0';
}


////将十六进制字符串转成数字
//uint16_t sim7020_hexstr2num(char* str)
//{
//  uint16_t i = 0;
//  uint8_t tmp = 0;
//  uint8_t tmp1 = 0;
//  
//  uint16_t len = strlen(str);
//  
//  
//  
//  
//  return (i >> 1);
//}



//sim7020 at指令初始化
static void at_cmd_param_init(at_cmdhandle cmd_handle,
                              const char *at_cmd,
                              char *argument,
                              cmd_property_t property,
                              uint32_t at_cmd_time_out)
{
    if(cmd_handle == NULL)
    {
      return;
    }
    cmd_handle->cmd_try     = CMD_TRY_TIMES;
    cmd_handle->property    = property;
    cmd_handle->cmd_action  = ACTION_OK_AND_NEXT | ACTION_ERROR_AND_TRY;
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
    memset(g_sim7020_send_desc.buf,0,SIM7020_SEND_BUF_MAX_LEN);
    g_sim7020_send_desc.len = 0;

    if(cmd_handle->property == CMD_TEST)
    {
        cmdLen = snprintf(g_sim7020_send_desc.buf,SIM7020_SEND_BUF_MAX_LEN,
                          "%s=?\r\n",
                          cmd_handle->p_atcmd);
    }    
    else if(cmd_handle->property == CMD_READ)
    {
        cmdLen = snprintf(g_sim7020_send_desc.buf,SIM7020_SEND_BUF_MAX_LEN,
                          "%s?\r\n",
                          cmd_handle->p_atcmd);
    }
    else if(cmd_handle->property == CMD_EXCUTE)
    {
        cmdLen = snprintf(g_sim7020_send_desc.buf,SIM7020_SEND_BUF_MAX_LEN,
                          "%s\r\n",
                          cmd_handle->p_atcmd);    
    }

    else if(cmd_handle->property == CMD_SET)
    {
        cmdLen = snprintf(g_sim7020_send_desc.buf,SIM7020_SEND_BUF_MAX_LEN,
                          "%s=%s\r\n",
                          cmd_handle->p_atcmd,cmd_handle->p_atcmd_arg);    
    }
    
    //cmdlen是有效的数据长度，不包括字符串结束标记符
    g_sim7020_send_desc.len = cmdLen;
    
    return cmdLen;
}


//判断at指令是否发送成功
//返回值如果是-1，则接收到的数据可能为乱码
//返回值如果是0， 指令通过，如果是1，指令出错
static int8_t at_cmd_result_parse (char *buf)
{
    int8_t result = -1;
  
//    SIM7020_DEBUG_INFO("respones buf%s\r\n",buf); 
//  
//    SIM7020_DEBUG_INFO("respones buf len %d \r\n", strlen(buf)); 
  
    char *p_colon = strchr(g_at_cmd.p_atcmd,'+');
    char *p_colon_temp = strchr(buf,':');
       
    if(g_at_cmd.p_expectres == NULL)
    {
        if (strstr(buf,"\r\nOK\r\n"))
        {
            result = AT_CMD_RESULT_OK;
        }
        else if (strstr(buf,"\r\nERROR\r\n"))
        {
            result = AT_CMD_RESULT_ERROR;
          
        } else if ((strstr(buf,g_at_cmd.p_atcmd)) || ((p_colon && p_colon_temp))) {
          
           //是命令的回显,或者OK之前的命令参数
           result = AT_CMD_RESULT_CONTINUE;
          
        } else {
          
           //乱码
           result = AT_CMD_RESULT_RANDOM_CODE;
        }
    }
    else
    {
        if (strstr(buf,"\r\nOK\r\n"))
        {
            //与得到的期望值一致
            if(strstr(buf,g_at_cmd.p_expectres))
            {
                result = AT_CMD_RESULT_OK;
            }
            else
            {
                result = AT_CMD_RESULT_ERROR;
            }
            
        }
        else if(strstr(buf,"\r\nERROR\r\n"))
        {
            //ERROR
            result = AT_CMD_RESULT_ERROR;
        }
         
        else if (strstr(buf, g_at_cmd.p_atcmd)) {
          
             result = AT_CMD_RESULT_CONTINUE;
          
        } else {
          
             //乱码
             result = AT_CMD_RESULT_RANDOM_CODE;      
          
        }    
    }

    return result;
}


//处理sim7020事件通知
static uint8_t sim7020_event_notify (sim7020_handle_t sim7020_handle, char *buf)
{
   char *target_pos_start = NULL;
    
   if((target_pos_start = strstr(buf, "+CGREG:")) != NULL)
    {
        char *p_colon = strchr(target_pos_start,':');
      
        if (p_colon)
        {
            p_colon = p_colon + 4;
            
            //该命令后面直接跟的网络注册状态的信息，用字符来表示的，要转换成数字        
            g_sim7020_status.register_status = (*p_colon - '0');
        }
        
        //产生注册事件
        if (g_sim7020_status.register_status == 1) {

            sim7020_event_set(sim7020_handle, SIM7020_REG_STA_EVENT);
        }        
           
        //同时产生接收类事件
        sim7020_event_set(sim7020_handle, SIM7020_RECV_EVENT);  
        
    }    
    else if((target_pos_start = strstr(buf,"+CSONMI")) != NULL)
    {
        //收到服务器端发来TCP/UDP数据
        char *p_colon = strchr(target_pos_start, ':');
      
        int8_t socket_id = 0;
        
        //得到是哪个socket收到数据
        if (p_colon)
        {
            p_colon++;
            socket_id = strtoul(p_colon,0,10);
        } 
        
        //得到收到的数据长度
        char *pComma = strchr(p_colon,',');

        if (pComma)
        {
            pComma++;
            g_socket_info[0].data_len = strtoul(pComma,0,10);
        }     
       
        //得到有效数据的起始地址
        char *p_data_offest = strchr(pComma,',');

        if (p_data_offest)
        {
            p_data_offest++;
            g_socket_info[0].data_offest = p_data_offest;
        }               
        if (g_socket_info[0].socket_type == 2) 
        {
            sim7020_event_set(sim7020_handle, SIM7020_UDP_RECV_EVENT);             
        }         
        else if (g_socket_info[0].socket_type == SIM7020_TCP)
        {

            sim7020_event_set(sim7020_handle, SIM7020_TCP_RECV_EVENT); 
        } 
        else 
        {
            //其它，默认产生接收事件
            sim7020_event_set(sim7020_handle, SIM7020_RECV_EVENT);            
        }
        
        sim7020_status_set(SIM7020_TCPUDP_RECV, SIM7020_SUB_TCPUDP_RECV);        
    }  
    else if((target_pos_start = strstr(buf,"+CSOERR")) != NULL)
    {
        //收到服务器端发来TCP/UDP错误码
        char *p_colon = strchr(target_pos_start,':');
      
        int8_t socket_id = 0;
      
        int8_t socket_err = 0;
        
        //得到是哪个socket收到数据
        if (p_colon)
        {
            p_colon++;          
            socket_id = strtoul(p_colon,0,10);
        }
        
        //得到收到的socket错误码
        char *pComma = strchr(p_colon,',');
        
        if(pComma)
        {
          pComma++;
          socket_err = strtoul(pComma, 0, 10);          
          g_socket_info[0].socket_errcode = socket_err; 
        }
                
        sim7020_event_set(sim7020_handle, SIM7020_SOCKET_ERR_EVENT); 
        sim7020_status_set(SIM7020_SOCKET_ERR, SIM7020_SUB_SOCKET_ERR);        
    }
    
    //收到Coap数据包
    else if((target_pos_start = strstr(buf,"+CCOAPNMI")) != NULL)
    {
        sim7020_event_set(sim7020_handle, SIM7020_COAP_RECV_EVENT);  
    }          
    else if((target_pos_start = strstr(buf,"+CLMOBSERVE")) != NULL)
    {
        sim7020_event_set(sim7020_handle, SIM7020_LWM2M_RECV_EVENT);          
    } 

    //收到MQTT数据包    
    else if ((target_pos_start = strstr(buf,"+CMQPUB")) != NULL)
    {        
        sim7020_event_set(sim7020_handle, SIM7020_MQTT_RECV_EVENT);  
    }    
    else 
    {
        //如果收到回复，其它是命令响应的数据
        sim7020_event_set(sim7020_handle, SIM7020_RECV_EVENT);  
    }
    
    return 0;
}

//产生下一条AT指令
static uint8_t at_cmd_next (void)
{ 
    if (g_sim7020_status.main_status == SIM7020_NBLOT_INIT)
    {
        g_sim7020_status.sub_status++;
      
        if (g_sim7020_status.sub_status == SIM7020_SUB_END)
        {
            return FALSE;
        }

        switch(g_sim7020_status.sub_status)
        {
          
        case SIM7020_SUB_SYNC:
            
            break;
        
        case SIM7020_SUB_CMEE:
          
            at_cmd_param_init(&g_at_cmd, AT_CMEE, "1", CMD_SET, 3000);
          
            break;
        
        case SIM7020_SUB_ATI:
          
            at_cmd_param_init(&g_at_cmd, AT_ATI, NULL, CMD_EXCUTE, 3000);
            g_at_cmd.p_expectres = "SIM";         //设置期望回复消息，如果指令执行完成
                                                  //没有与期望的消息匹配，则认为出错
                                                  //并进行出错尝试           
            break;
                      
        //查询NB卡状态是否准备好    
        case SIM7020_SUB_CPIN:
          {
            at_cmd_param_init(&g_at_cmd, AT_CPIN, NULL, CMD_READ, 3000);
            g_at_cmd.p_expectres = "+CPIN: READY"; //设置期望回复消息，如果指令执行完成
                                                   //没有与期望的消息匹配，则认为出错
                                                   //并进行出错尝试              
          }
          break;
                    
          
        //查询射频模块信号质量   
        case SIM7020_SUB_CSQ:
          {
             at_cmd_param_init(&g_at_cmd, AT_CSQ, NULL, CMD_EXCUTE, 3000);
          }
          break;

        //使能模块射频信号,响应等待的最长时间为10S      
        case SIM7020_SUB_CFUN:                                   
          {
            at_cmd_param_init(&g_at_cmd, AT_CFUN,"1",CMD_SET, 11000);
          }
          
          break;
          
        // 使能NBlot网络注册   
        case SIM7020_SUB_CEREG:
          {
            at_cmd_param_init(&g_at_cmd, AT_CGREG, "1", CMD_SET, 3000);
          }
          break;  
          
//        //先禁能PDN， 命令在150S之内会有回应     
//        case SIM7020_SUB_CGACT_DISABLE:
//          {
//            at_cmd_param_init(&g_at_cmd, AT_CGACT,"0,1",CMD_SET, 151000);
//          }
//          break;          
//    
//          
        //再使能PDN， 命令在150S之内会有回应，不先禁能的话，再使能状态执行执命令会出错     
//        case SIM7020_SUB_CGACT:
//          {
//            at_cmd_param_init(&g_at_cmd, AT_CGACT,"1,1",CMD_SET, 151000);
//          }
//          break;
        
        //查询PDN激活信息          
        case SIM7020_SUB_CGACT_QUERY:
          {
            at_cmd_param_init(&g_at_cmd, AT_CGACT, NULL, CMD_READ, 151000);
            g_at_cmd.p_expectres = "+CGACT: 1,1"; //设置期望回复消息，如果指令执行完成
                                                  //没有与期望的消息匹配，则认为出错
                                                  //并进行出错尝试
          }
          break;          
               
          
        //使能网络附着,最大响应时间不详      
        case SIM7020_SUB_CGATT:
          {
            at_cmd_param_init(&g_at_cmd, AT_CGATT, "1", CMD_SET, 3000);
          }
          break;

        //查询网络附着信息,最大响应时间不详       
        case SIM7020_SUB_CGATT_QUERY:
          {
            at_cmd_param_init(&g_at_cmd, AT_CGATT, NULL, CMD_READ, 3000);
            
            //设置期望回复消息，如果指令执行完成
            //没有与期望的消息匹配，则认为出错                                             
            //并进行出错尝试               
            g_at_cmd.p_expectres = "CGATT: 1";     
          }
          break;
          
        //查询是否是使用NBlot网络,最大响应时间不详       
        case SIM7020_SUB_COPS_QUERY:
          {
            at_cmd_param_init(&g_at_cmd, AT_COPS, NULL, CMD_READ, 3000);
            
            //设置期望回复消息为9，代表是NBloT网络
            //如果指令执行完成,没有与期望的消息匹配                                            
            //则认为出错，并进行出错尝试               
            g_at_cmd.p_expectres = "9";     
          }
          break;
          
 
        //查询分配的APN信息及IP地址     
        case SIM7020_SUB_CGCONTRDP_QUERY:
          {
            at_cmd_param_init(&g_at_cmd, AT_CGCONTRDP, NULL, CMD_EXCUTE, 3000);
               
          }
          break;
                    
          
        //查询NBlot网络是否注册,最大响应时间不详       
        case SIM7020_SUB_CEREG_QUERY:
          {
            at_cmd_param_init(&g_at_cmd, AT_CGREG, NULL, CMD_READ, 3000);             

          }
          break;
                    
        default: 
          
          //强制表示子进程结束
          g_sim7020_status.sub_status = SIM7020_SUB_END;
        
          return FALSE;
                   
         }
    }
    
    else if (g_sim7020_status.main_status == SIM7020_NBLOT_INFO)
    {
        g_sim7020_status.sub_status++;
      
        if (g_sim7020_status.sub_status == SIM7020_SUB_END)
        {
            return FALSE;
        }
        
        switch(g_sim7020_status.sub_status)
        {
          
        case  SIM7020_SUB_CGMI:
          
          {
            at_cmd_param_init(&g_at_cmd,AT_CGMI,NULL,CMD_EXCUTE,3000);
          }
          break;

                   
        case SIM7020_SUB_CGMM:
          {
            at_cmd_param_init(&g_at_cmd,AT_CGMM,NULL,CMD_EXCUTE,3000);
          }
          break;
          
        case SIM7020_SUB_CGMR:
          {
            at_cmd_param_init(&g_at_cmd,AT_CGMR,NULL,CMD_EXCUTE,3000);
          }
          break;
          
        case SIM7020_SUB_CIMI:
          {
            at_cmd_param_init(&g_at_cmd,AT_CIMI,NULL,CMD_EXCUTE,3000);
          }
          break;

        case SIM7020_SUB_CGSN:
          {
            at_cmd_param_init(&g_at_cmd, AT_CGSN, NULL, CMD_EXCUTE, 3000);
            
            //设置期望回复消息为络
            //如果指令执行完成,没有与期望的消息匹配                                            
            //则认为出错，并进行出错尝试               
            g_at_cmd.p_expectres = "CGSN";
          }
          break;             
          
          
        case SIM7020_SUB_CBAND:
          {
            at_cmd_param_init(&g_at_cmd,AT_CBAND,NULL,CMD_READ,3000);
            //设置期望回复消息为络
            //如果指令执行完成,没有与期望的消息匹配                                            
            //则认为出错，并进行出错尝试               
            g_at_cmd.p_expectres = "CBAND";
          }
          break;
          
        default: 
          
          //强制表示子进程结束
          g_sim7020_status.sub_status = SIM7020_SUB_END;
        
          return FALSE;          
        
        }
    }
    else if (g_sim7020_status.main_status == SIM7020_SIGNAL)
    {
        
        g_sim7020_status.sub_status = SIM7020_SUB_END;
        return FALSE;
    }
    
    else if (g_sim7020_status.main_status == SIM7020_TCPUDP_CR)
    {
        
        g_sim7020_status.sub_status++;
      
      
        if (g_sim7020_status.sub_status == SIM7020_SUB_END)
        {
            return FALSE;
        }
        
        switch(g_sim7020_status.sub_status)
        {
          
        case SIM7020_SUB_TCPUDP_CONNECT:
          
          {
          
            char *p_remote_port = NULL;

            static char buf[64];
          
            memset(buf,0,sizeof(buf));
            
            if (g_socket_info[0].socket_type == SIM7020_TCP)
            { 
                p_remote_port = REMOTE_TCP_PORT;
            }
            else
            {
                p_remote_port = REMOTE_UDP_PORT;   
            }              

    
            //这个函数的返回值为想要格式化写入的长度，并会在字符串结束后面自动加入结束字符
            uint16_t tcpudp_cn_len = snprintf(buf,
                                        sizeof(buf) -1,"%d,%s,%s",                                          
                                        g_socket_info[0].socket_id,
                                        p_remote_port,                                         
                                        REMOTE_SERVER_IP);
                                        
//            SIM7020_DEBUG_INFO("tcpudp_cn_len = %d\r\n", tcpudp_cn_len);
//                                        
//            SIM7020_DEBUG_INFO("tcpudp_cn_buf = %s\r\n", buf);                            
            
            //最大响应时间不详                                        
            at_cmd_param_init(&g_at_cmd,AT_CSOCON, buf, CMD_SET, 3000);
                                        
          }
          break;

                                     
        default: 
          
          //强制表示子进程结束
          g_sim7020_status.sub_status = SIM7020_SUB_END;
        
          return FALSE;          
        
        }
    } 
    else if (g_sim7020_status.main_status == SIM7020_TCPUDP_CL) 
    {
      
        g_sim7020_status.sub_status = SIM7020_SUB_END;
      
        return FALSE;    
      
    }  
    else if (g_sim7020_status.main_status == SIM7020_TCPUDP_SEND) 
    {
      
        g_sim7020_status.sub_status = SIM7020_SUB_END;
      
        return FALSE;     
    }
        
    else if (g_sim7020_status.main_status == SIM7020_NONE)   
    {  //防止意外重发
       return FALSE; 
    }
    
    else 
    {
      
    }
    
    return TRUE;
}

//发送消息与应用层交互
static void sim7020_msg_send (sim7020_handle_t sim7020_handle, char**buf, int8_t is_ok)
{
  if (sim7020_handle == NULL)
  {
      return;
  }
  
  if ((is_ok == SIM7020_ERROR_RETRY) || 
      (is_ok == SIM7020_ERROR_NEXT)  ||
      (is_ok == SIM7020_ERROR_TIMEOUT)) {
        
      sim7020_handle->sim7020_cb(sim7020_handle->p_arg, (sim7020_msg_id_t)SIM7020_MSG_RETRY, strlen(buf[0]), buf[0]);  
        
      return;      
  }
      
  //出错，则上报此流程执行失败
  else if(is_ok == FALSE)
  {
    sim7020_handle->sim7020_cb(sim7020_handle->p_arg, (sim7020_msg_id_t)SIM7020_MSG_FAIL, strlen(buf[0]), buf[0]);
    return;
  }
  
  if (g_sim7020_status.main_status == SIM7020_NBLOT_INIT)
  {
    switch(g_sim7020_status.sub_status)
    {
        
    case SIM7020_SUB_SYNC:
        
      break;
    
    case SIM7020_SUB_CMEE:
      
      break;    
    
    case SIM7020_SUB_ATI:
      
      //得到模块的名字
      memcpy(g_firmware_info.name, buf[1], strlen(buf[1])); 
        
      break;    
    
    case SIM7020_SUB_CPIN:
        
      break;
      
    case SIM7020_SUB_CSQ:
    {
        char *p_colon = strchr(buf[1],':');
        p_colon++;
      
        //转换成10进制数字
        uint8_t lqi =strtoul(p_colon,0, 10);
      
        //运算取得每个数值对应的dbm范围
        int8_t rssi = -110 + (lqi << 1);
      
        g_sim7020_status.rssi = rssi; 
      
        break;
    }  
              
    case SIM7020_SUB_CFUN:
        break;
    

    case SIM7020_SUB_CEREG:
            
        break;
    
    
//    case SIM7020_SUB_CGACT_DISABLE:
//           
//    
//    case SIM7020_SUB_CGACT:
//        
//        break;
//    
//    
    case SIM7020_SUB_CGACT_QUERY:
        
        break;
    
        
    case SIM7020_SUB_CGATT:
        
        break;
    
    
    case SIM7020_SUB_CGATT_QUERY:
        
        break;
    
    case SIM7020_SUB_COPS_QUERY:
        
        break;  

    case SIM7020_SUB_CGCONTRDP_QUERY:
        
        break;       
       
    case SIM7020_SUB_END:
        
        sim7020_handle->sim7020_cb(sim7020_handle->p_arg, (sim7020_msg_id_t)SIM7020_NBLOT_INIT,1,"S");
    
        break;
    
    default:
      
        break;
    }
  }
  else if(g_sim7020_status.main_status == SIM7020_NBLOT_INFO)
  {
    switch(g_sim7020_status.sub_status)
    {
        
    //查询网络注册状态    
    case SIM7020_SUB_CEREG_QUERY:

      sim7020_handle->sim7020_cb(sim7020_handle->p_arg, (sim7020_msg_id_t)SIM7020_MSG_REG, strlen(buf[0]), buf[0]);
    
      break;        
        
                 
    case SIM7020_SUB_CGMI:
      {
        sim7020_handle->sim7020_cb(sim7020_handle->p_arg, (sim7020_msg_id_t)SIM7020_MSG_MID, strlen(buf[1]), buf[1]);
      }
      break;
      
    case SIM7020_SUB_CGMM:
      {
        sim7020_handle->sim7020_cb(sim7020_handle->p_arg, (sim7020_msg_id_t)SIM7020_MSG_MMODEL,strlen(buf[1]),buf[1]);
      }
      break;
      
    case SIM7020_SUB_CGMR:
      {

         sim7020_handle->sim7020_cb(sim7020_handle->p_arg, (sim7020_msg_id_t)SIM7020_MSG_MREV,strlen(buf[1]),buf[1]);
        
      }
      break;
      
      
    case SIM7020_SUB_CIMI:
      {
        memcpy(g_firmware_info.IMSI,buf[1],15);
        g_firmware_info.IMSI[15] = 0;
        sim7020_handle->sim7020_cb(sim7020_handle->p_arg, (sim7020_msg_id_t)SIM7020_MSG_IMSI,strlen(buf[1]),buf[1]);
      }
      break;
      
    
    case SIM7020_SUB_CGSN:
      {
        char *p_colon = strchr(buf[1],':');
        if(p_colon)
        {
          p_colon++;
          memcpy(g_firmware_info.IMEI ,p_colon,15);
          g_firmware_info.IMEI[15] = 0;
          sim7020_handle->sim7020_cb(sim7020_handle->p_arg, (sim7020_msg_id_t)SIM7020_MSG_IMEI,15,(char*)g_firmware_info.IMEI);
        }
      }
      break;            
      
    case SIM7020_SUB_CBAND:
      {
        char *p_colon = strchr(buf[1],':');
        char *pFreq = NULL;
        if(p_colon)
        {
          p_colon++;
          uint8_t hz_id = strtoul(p_colon,0,10);
          if(hz_id == BAND_850MHZ_ID)
          {
            //850MHZ
            pFreq = BAND_850MHZ_STR;
          }
          else if(hz_id == BAND_900MHZ_ID)
          {
            //900MHZ
            pFreq = BAND_900MHZ_STR;
          }
          else if(hz_id == BAND_800MHZ_ID)
          {
            //800MHZ 
            pFreq = BAND_800MHZ_STR;
          }
          else 
          {
            //700MHZ
            pFreq = BAND_700MHZ_STR;
          }
          sim7020_handle->sim7020_cb(sim7020_handle->p_arg, (sim7020_msg_id_t)SIM7020_MSG_BAND,strlen(pFreq),pFreq);
        }
      }
      break;

    case SIM7020_SUB_END:
      {
        sim7020_handle->sim7020_cb(sim7020_handle->p_arg, (sim7020_msg_id_t)SIM7020_MSG_NBLOT_INFO, 1, "S");
      }
      break;
    }
  }
  else if(g_sim7020_status.main_status == SIM7020_SIGNAL)
  {
    switch(g_sim7020_status.sub_status) 
    { 
    case SIM7020_SUB_CSQ:
    {
        char *p_colon = strchr(buf[1],':');
        if (p_colon != NULL) 
        {            
          p_colon++;
          //转换成10进制数字
          uint8_t lqi =strtoul(p_colon,0, 10);
          //运算取得每个数值对应的dbm范围
          int8_t rssi = -110 + (lqi << 1);
          uint8_t len = snprintf(buf[1],10,"%d",rssi);
          *(buf[1]+len) = 0;
          
          g_sim7020_status.rssi = rssi;
          
          sim7020_handle->sim7020_cb(sim7020_handle->p_arg,(sim7020_msg_id_t)SIM7020_MSG_CSQ,len,buf[1]);
          
        }
      
        break;
    } 
      
    case SIM7020_SUB_END:
    {
        sim7020_handle->sim7020_cb(sim7020_handle->p_arg,(sim7020_msg_id_t)SIM7020_MSG_SIGNAL,1,"S");
        break;
    }
    
    default:
      
        break;
    }
    
  }
  else if(g_sim7020_status.main_status == SIM7020_TCPUDP_CR)
  {
    switch(g_sim7020_status.sub_status)
    {
    case SIM7020_SUB_TCPUDP_CR:
      {

        char *p_colon = strchr(buf[1],':');
                    
        if (p_colon != NULL) 
        {                
            p_colon++;
            
            //转换成10进制数字,得到当前创建的socket id
            g_socket_info[0].socket_id =strtoul(p_colon,0, 10);
        }
                
      }
      break;
      
    case SIM7020_SUB_TCPUDP_CONNECT:
      {                                             
      }
      break;
      
    case SIM7020_SUB_END:
      {              
         char *p_buf_tmep = NULL;
              
         g_sim7020_status.connect_status = 1;  
               
         if (g_socket_info[0].socket_type == SIM7020_TCP)
         {
             p_buf_tmep = "tcp create ok";
         }
        
         else 
         {   
             p_buf_tmep = "udp create ok";
         } 
                                                       
         sim7020_handle->sim7020_cb(sim7020_handle->p_arg, (sim7020_msg_id_t)SIM7020_MSG_TCPUDP_CREATE, strlen(p_buf_tmep), p_buf_tmep);
                
      }
      break;
    }
  } 
  else if(g_sim7020_status.main_status == SIM7020_TCPUDP_CL)
  {
    if (g_sim7020_status.sub_status == SIM7020_SUB_END)
    {
         g_sim7020_status.connect_status = 0;  
      
         char *p_buf_tmep = NULL;
                                    
         if (g_socket_info[0].socket_type == SIM7020_TCP)
         {
             p_buf_tmep = "tcp close";
         }         
         else 
         {   
             p_buf_tmep = "udp close";
         }         
         sim7020_handle->sim7020_cb(sim7020_handle->p_arg,(sim7020_msg_id_t)SIM7020_MSG_TCPUDP_CLOSE, strlen(p_buf_tmep), p_buf_tmep);
     }
  }   
  else if(g_sim7020_status.main_status == SIM7020_TCPUDP_SEND)
  {
    switch(g_sim7020_status.sub_status)
    {
      
    case SIM7020_SUB_END:
      {
        char *p_buf_tmep = g_sim7020_send_desc.buf;
        sim7020_handle->sim7020_cb(sim7020_handle->p_arg, (sim7020_msg_id_t)SIM7020_MSG_TCPUDP_SEND,strlen(p_buf_tmep),p_buf_tmep);
      }
      break;
      
    default:
      
      break;
    }
  }
  else if(g_sim7020_status.main_status == SIM7020_TCPUDP_RECV)
  {
    if(g_sim7020_status.sub_status == SIM7020_SUB_TCPUDP_RECV)
    {
      char *data_buf = g_socket_info[0].data_offest; 

//      SIM7020_DEBUG_INFO("data_buf = %s", data_buf);      
           
      sim7020_handle->sim7020_cb(sim7020_handle->p_arg,(sim7020_msg_id_t)SIM7020_MSG_TCPUDP_RECV,strlen(data_buf),data_buf);
      
      //复位状态标志
      sim7020_status_reset();
    }
    
  }
  
  else if(g_sim7020_status.main_status == SIM7020_SOCKET_ERR)
  {
    if (g_sim7020_status.sub_status == SIM7020_SUB_SOCKET_ERR)
    {

        SIM7020_DEBUG_INFO("the socket err, the err code is %d\r\n", g_socket_info[0].socket_errcode); 
             
        sim7020_handle->sim7020_cb(sim7020_handle->p_arg,
                                   (sim7020_msg_id_t)SIM7020_MSG_SOCKET_ERROR, 
                                    1, 
                                    (char *)&g_socket_info[0].socket_errcode);

        //复位状态标志
        sim7020_status_reset();
    }
    
  }
  
  else if(g_sim7020_status.main_status == SIM7020_CoAP_SEVER)
  {
    if(g_sim7020_status.sub_status == 1)
    {
      char* tmp_buf = NULL;
      if(strstr(buf[0],"OK"))
      {
        tmp_buf = "S";
      }
      else
      {
        tmp_buf = strchr(buf[1],':');
        if(tmp_buf)
        {
          tmp_buf++;
        }
      }
      
      sim7020_handle->sim7020_cb(sim7020_handle->p_arg, (sim7020_msg_id_t)SIM7020_MSG_COAP,strlen(tmp_buf),tmp_buf);
    }
  }
  else if(g_sim7020_status.main_status == SIM7020_CoAP_SEND)
  {
    if(g_sim7020_status.sub_status == 1)
    {
      sim7020_handle->sim7020_cb(sim7020_handle->p_arg, (sim7020_msg_id_t)SIM7020_MSG_COAP_SEND,1,"S");
    }
  }
  else if(g_sim7020_status.main_status == SIM7020_CoAP_RECV)
  {
    if(g_sim7020_status.sub_status == 1)
    {
      uint16_t index = 0;
      char* tmp_buf = NULL;
     
      tmp_buf = strchr(buf[0],',');
      
      if(tmp_buf)
      {
        tmp_buf++;
        index = 0;
        sim7020_handle->sim7020_cb(sim7020_handle->p_arg, (sim7020_msg_id_t)SIM7020_MSG_COAP_RECV, index,tmp_buf); 
      }
   
    }
  }

  else
  {

  }    
}

//指令响应结果处理，这个函数利用命令的状态属性，来结束状态的执行。
uint8_t sim7020_response_handle (sim7020_handle_t sim7020_handle, uint8_t cmd_response)
{
    uint8_t next_cmd = 0;
      
    if (cmd_response)
    {
        if (g_at_cmd.cmd_action & ACTION_OK_AND_NEXT)
        {
            next_cmd = TRUE;
        }        
        else   
        {
            //代表命令执行成功后退出
             g_at_cmd.cmd_action = ACTION_OK_EXIT;             
        }
    }
    else
    {
        if(g_at_cmd.cmd_action & ACTION_ERROR_AND_TRY)
        {
            g_at_cmd.have_tried++;

            if (g_at_cmd.have_tried < g_at_cmd.cmd_try)
            {
              sim7020_at_cmd_send(sim7020_handle, &g_at_cmd);
            }
            else
            {
                  
               //代表命令执行错误后退出
                g_at_cmd.cmd_action = ACTION_ERROR_EXIT;
                
            }
        }
        else if (!(g_at_cmd.cmd_action & ACTION_ERROR_EXIT))  
        {
            //ACTION_ERROR_BUT_NEXT
            next_cmd = TRUE;
        }
        else 
        {
            //nerver reach here  
        }
    }
    
    return next_cmd;
}


//sim7020发送AT指令
//sim7020_handle   sim7020_handle设备句柄
//cmd_handle       将要发送指令信息句柄
//note 调用该函数前先构造好命令的参数
static int sim7020_at_cmd_send(sim7020_handle_t sim7020_handle, at_cmdhandle cmd_handle)
{
    int strLen = 0;
    
    int ret = 0;
        
    if (sim7020_handle == NULL || cmd_handle == NULL)
    {
       return SIM7020_ERROR;
    }
        
    strLen = cmd_generate(cmd_handle);

    ret = sim7020_handle->p_drv_funcs->sim7020_send_data(sim7020_handle, 
                                                         (uint8_t*)g_sim7020_send_desc.buf, 
                                                         strLen,                                                    
                                                         cmd_handle->max_timeout);   
    return ret;
}

//sim7020接收数据
//sim7020_handle 设备句柄
static int sim7020_data_recv(sim7020_handle_t sim7020_handle, uint8_t *pData, uint16_t size, uint32_t Timeout)
{   
    int ret = 0;
        
    if (sim7020_handle == NULL)
    {
       return SIM7020_ERROR;
    }
        
    ret = sim7020_handle->p_drv_funcs->sim7020_recv_data(sim7020_handle, pData, size, Timeout);      
    //在后面添加字符串结束符
//    pData[size]=0;
    
    return ret;
}


//sim7020初始化 
sim7020_handle_t sim7020_init(uart_handle_t lpuart_handle)
{
     //填充设备结构体
     g_sim7020_dev.p_uart_dev    = lpuart_handle;
     g_sim7020_dev.p_drv_funcs   = &drv_funcs; 

     g_sim7020_dev.p_sim702_cmd  = &g_at_cmd;    
     g_sim7020_dev.p_socket_info = g_socket_info;
     g_sim7020_dev.firmware_info = &g_firmware_info;
     g_sim7020_dev.sim702_status = &g_sim7020_status;

     g_sim7020_dev.frame_format  = 0;  
    
     /* 注册sim7020串口收发事件回调函数 */
     lpuart_event_registercb(lpuart_handle, __uart_event_cb_handle, &g_sim7020_dev);     
    
     return &g_sim7020_dev;    
}

//注册sim7020事件回调函数
void sim7020_event_registercb (sim7020_handle_t sim7020_handle, sim7020_cb cb, void *p_arg)
{  
    if(cb != 0)
    {
        sim7020_handle->sim7020_cb  = cb;
        sim7020_handle->p_arg       = p_arg;
    }
}

//sim7020 nblot初始化及完成网络注册
int sim7020_nblot_init (sim7020_handle_t sim7020_handle)
{
    
    if (g_sim7020_status.main_status != SIM7020_NONE)
    {
        return SIM7020_ERROR;
    }

    at_cmd_param_init(&g_at_cmd, AT_SYNC, NULL, CMD_EXCUTE, 3000);

    //进入SIM7020_NBLOT_INIT状态
    g_sim7020_status.main_status = SIM7020_NBLOT_INIT;
    g_sim7020_status.sub_status  = SIM7020_SUB_SYNC;

    sim7020_at_cmd_send(sim7020_handle, &g_at_cmd);

    return SIM7020_OK;
}



//获取NB模块的信息
int sim7020_nblot_info_get(sim7020_handle_t sim7020_handle)
{

    if (g_sim7020_status.main_status != SIM7020_NONE)
    {
        return SIM7020_ERROR;
    }
    
    
    at_cmd_param_init(&g_at_cmd, AT_CGREG, NULL, CMD_READ, 3000);

    //进入SIM7020_NBLOT_INFO状态
    g_sim7020_status.main_status = SIM7020_NBLOT_INFO;
    g_sim7020_status.sub_status  = SIM7020_SUB_CEREG_QUERY;

    sim7020_at_cmd_send(sim7020_handle, &g_at_cmd);
    
    return SIM7020_OK;
}


//获取NB模块的信号质量
int sim7020_nblot_signal_get(sim7020_handle_t sim7020_handle)
{

    if (g_sim7020_status.main_status != SIM7020_NONE)
    {
        return SIM7020_ERROR;
    }
        
    at_cmd_param_init(&g_at_cmd, AT_CSQ, NULL, CMD_EXCUTE, 3000);

    //进入SIM7020_SIGNAL状态
    g_sim7020_status.main_status = SIM7020_SIGNAL;
    g_sim7020_status.sub_status  = SIM7020_SUB_CSQ;

    sim7020_at_cmd_send(sim7020_handle, &g_at_cmd);
    
    return SIM7020_OK;
}

//创建tcpudp 
int sim7020_nblot_tcpudp_create(sim7020_handle_t sim7020_handle, sim7020_connect_type_t type)
{
    char *p_tcpudp = NULL;
  
    if (g_sim7020_status.main_status != SIM7020_NONE)
    {
        return SIM7020_ERROR;
    }
    
    if (type == SIM7020_TCP)
    {
        g_socket_info[0].socket_type = SIM7020_TCP;
        p_tcpudp = "1,1,1";
    }
    
    else if(type == SIM7020_UDP)
    {   
        g_socket_info[0].socket_type = SIM7020_UDP;
        p_tcpudp = "1,2,1";
    } 
    else 
    {
       return SIM7020_NOTSUPPORT;
      
    }
            
    at_cmd_param_init(&g_at_cmd, AT_CSOC, p_tcpudp, CMD_SET, 3000);
//    g_at_cmd.cmd_action  = ACTION_OK_AND_NEXT | ACTION_ERROR_EXIT;
    
    //进入SIM7020_SIGNAL状态
    g_sim7020_status.main_status = SIM7020_TCPUDP_CR;
    g_sim7020_status.sub_status  = SIM7020_SUB_TCPUDP_CR;

    sim7020_at_cmd_send(sim7020_handle, &g_at_cmd);
    
    return SIM7020_OK;
}


//关闭tcpudp
int sim7020_nblot_tcpudp_close(sim7020_handle_t sim7020_handle, sim7020_connect_type_t type)
{

    if (g_sim7020_status.main_status != SIM7020_NONE)
    {
        return SIM7020_ERROR;
    }
    
       
    (void)type;
    
    //最大数据长度为有效数据加上头部

    char  buf[36];
    
    memset(buf, 0, sizeof(buf));
    
    
    int16_t msg_len = snprintf(buf,
                                sizeof(buf),
                                "%d",
                                g_socket_info[0].socket_id);
                                
    if (msg_len < 0) {
      
        return SIM7020_ERROR;
    }
    
//    SIM7020_DEBUG_INFO("close len %d\r\n",msg_len); 
    
                                    
        
    at_cmd_param_init(&g_at_cmd, AT_CSOCL, buf, CMD_SET, 3000);

    //进入tcp/udp关闭状态
    g_sim7020_status.main_status = SIM7020_TCPUDP_CL;
    g_sim7020_status.sub_status  = SIM7020_SUB_TCPUDP_CL;

    sim7020_at_cmd_send(sim7020_handle, &g_at_cmd);
    
//    SIM7020_DEBUG_INFO("close buf %s", g_sim7020_send_desc.buf);
    
    return SIM7020_OK;
}


//以hex数据格式发送数据,必须是偶数个长度
int sim7020_nblot_tcpudp_send_hex(sim7020_handle_t sim7020_handle, int len, char *msg, sim7020_connect_type_t type)
{
  
    if (g_sim7020_status.main_status != SIM7020_NONE)
    {
        return SIM7020_ERROR;
    }
  
    //判断SOCKET ID是否正确
    if (g_socket_info[0].socket_id  < 0 || g_socket_info[0].socket_id > 5 )
    {
        return SIM7020_ERROR;
    }
  
    //最大数据长度为有效数据加上头部
    int16_t str_len = (SIM7020_SEND_BUF_MAX_LEN - 20) ;

    char  buf[(SIM7020_SEND_BUF_MAX_LEN - 20)];
    
    memset(buf, 0, str_len);


    uint16_t msg_len = snprintf(buf,
                                str_len,
                                "%d,%d,",
                                 g_socket_info[0].socket_id, 
                                len);
                                
    for(uint16_t i = 0 ; i < len ; i++)
    {
        sprintf(&buf[msg_len + (i << 1)],"%02X",(uint8_t)msg[i]);
    }                             
 
    //构建TCP/UDP数据发送命令，最大响应时间不详
    at_cmd_param_init(&g_at_cmd, AT_CSOSEND, buf, CMD_SET, 3000);
    
    //进入tcp/udp数据发送状态
    g_sim7020_status.main_status = SIM7020_TCPUDP_SEND;
    g_sim7020_status.sub_status  = SIM7020_SUB_TCPUDP_SEND;

    sim7020_at_cmd_send(sim7020_handle, &g_at_cmd);

    return SIM7020_OK;
}

//以字符串格式发送数据
int sim7020_nblot_tcpudp_send_str(sim7020_handle_t sim7020_handle, int len, char *msg, sim7020_connect_type_t type)
{
  
    if (g_sim7020_status.main_status != SIM7020_NONE)
    {
        return SIM7020_ERROR;
    }
  
    //判断SOCKET ID是否正确
    if (g_socket_info[0].socket_id  < 0 || g_socket_info[0].socket_id > 5 )
    {
        return SIM7020_ERROR;
    }
      
    //最大数据长度为有效数据加上头部
    int16_t str_len = (SIM7020_SEND_BUF_MAX_LEN - 20) ;


    char  buf[SIM7020_SEND_BUF_MAX_LEN - 20];
    
    memset(buf, 0, str_len);

    int16_t msg_len = snprintf(buf,
                               str_len,
                               "%d,%d,%s%s%s",
                                g_socket_info[0].socket_id,
                                0,
                                "\"",                                  
                                msg,
                                "\"");
    
    if (msg_len < 0) {
      
        return SIM7020_ERROR;
    }
    
//    SIM7020_DEBUG_INFO("send buf len %d\r\n",msg_len); 
                                                          
    //构建TCP/UDP数据发送命令，最大响应时间不详
    at_cmd_param_init(&g_at_cmd, AT_CSOSEND, buf, CMD_SET, 3000);
    
    //进入tcp/udp数据发送状态
    g_sim7020_status.main_status = SIM7020_TCPUDP_SEND;
    g_sim7020_status.sub_status  = SIM7020_SUB_TCPUDP_SEND;

    sim7020_at_cmd_send(sim7020_handle, &g_at_cmd);
    
    
//    SIM7020_DEBUG_INFO("send buf %s", g_sim7020_send_desc.buf);    

    return SIM7020_OK;
}   

//创建coap服务器 
int sim7020_nblot_coap_server_create(sim7020_handle_t sim7020_handle, sim7020_connect_type_t type)
{
  
    char buf[64] = {0};
  
    if (g_sim7020_status.main_status != SIM7020_NONE)
    {
        return SIM7020_ERROR;
    }
    
    if (type != SIM7020_COAP)
    {
        return SIM7020_NOTSUPPORT;
    }
    
    g_sim7020_status.connect_type = SIM7020_COAP;  
    g_sim7020_status.cid          = 1;
               
                       
    //这个函数的返回值为想要格式化写入的长度，并会在字符串结束后面自动加入结束字符
    uint16_t coap_cn_len = snprintf(buf,
                                    sizeof(buf) -1,"%s,%s,%d",                                                                                             
                                    REMOTE_SERVER_IP,
                                    REMOTE_COAP_PORT,
                                    g_sim7020_status.cid);
                                        
                     
            
    //最大响应时间不详                                          
    
    at_cmd_param_init(&g_at_cmd, AT_CCOAPSTA, buf, CMD_SET, 3000);
    
    //进入设置COAP服务器状态
    g_sim7020_status.main_status = SIM7020_CoAP_SEVER;
    g_sim7020_status.sub_status  = SIM7020_SUB_CoAP_SEVER;

    sim7020_at_cmd_send(sim7020_handle, &g_at_cmd);
    
    return SIM7020_OK;
}

//创建coap客户端
int sim7020_nblot_coap_client_create(sim7020_handle_t sim7020_handle, sim7020_connect_type_t type)
{
    char *p_tcpudp = NULL;
  
    if (g_sim7020_status.main_status != SIM7020_NONE)
    {
        return SIM7020_ERROR;
    }
    
    if (type == SIM7020_TCP)
    {
        g_socket_info[0].socket_type = SIM7020_TCP;
        p_tcpudp = "1,1,1";
    }
    
    else if(type == SIM7020_UDP)
    {   
        g_socket_info[0].socket_type = SIM7020_UDP;
        p_tcpudp = "1,2,1";
    } 
    else 
    {
        return SIM7020_NOTSUPPORT;
      
    }
            
    at_cmd_param_init(&g_at_cmd, AT_CSOC, p_tcpudp, CMD_SET, 3000);
    
    //进入SIM7020_SIGNAL状态
    g_sim7020_status.main_status = SIM7020_TCPUDP_CR;
    g_sim7020_status.sub_status  = SIM7020_SUB_TCPUDP_CR;

    sim7020_at_cmd_send(sim7020_handle, &g_at_cmd);
    
    return SIM7020_OK;
}


//关闭coap
int sim7020_nblot_coap_close(sim7020_handle_t sim7020_handle, sim7020_connect_type_t type)
{

    if (g_sim7020_status.main_status != SIM7020_NONE)
    {
        return SIM7020_ERROR;
    }
    
       
    (void)type;
    
    //最大数据长度为有效数据加上头部

    char  buf[36];
    
    memset(buf, 0, sizeof(buf));
    
    
    int16_t msg_len = snprintf(buf,
                                sizeof(buf),
                                "%d",
                                g_socket_info[0].socket_id);
                                
    if (msg_len < 0) {
      
        return SIM7020_ERROR;
    }
    
//    SIM7020_DEBUG_INFO("close len %d\r\n",msg_len); 
    
                                    
        
    at_cmd_param_init(&g_at_cmd, AT_CSOCL, buf, CMD_SET, 3000);

    //进入tcp/udp关闭状态
    g_sim7020_status.main_status = SIM7020_TCPUDP_CL;
    g_sim7020_status.sub_status  = SIM7020_SUB_TCPUDP_CL;

    sim7020_at_cmd_send(sim7020_handle, &g_at_cmd);
    
//    SIM7020_DEBUG_INFO("close buf %s", g_sim7020_send_desc.buf);
    
    return SIM7020_OK;
}


//以hex数据格式发送数据,必须是偶数个长度
int sim7020_nblot_coap_send_hex(sim7020_handle_t sim7020_handle, int len, char *msg, sim7020_connect_type_t type)
{
  
    if (g_sim7020_status.main_status != SIM7020_NONE)
    {
        return SIM7020_ERROR;
    }
  
    //判断SOCKET ID是否正确
    if (g_socket_info[0].socket_id  < 0 || g_socket_info[0].socket_id > 5 )
    {
        return SIM7020_ERROR;
    }
  
    //最大数据长度为有效数据加上头部
    int16_t str_len = (SIM7020_SEND_BUF_MAX_LEN - 20) ;

    char  buf[(SIM7020_SEND_BUF_MAX_LEN - 20)];
    
    memset(buf, 0, str_len);


    uint16_t msg_len = snprintf(buf,
                                str_len,
                                "%d,%d,",
                                 g_socket_info[0].socket_id, 
                                len);
                                
    for(uint16_t i = 0 ; i < len ; i++)
    {
        sprintf(&buf[msg_len + (i << 1)],"%02X",(uint8_t)msg[i]);
    }                             
 
    //构建TCP/UDP数据发送命令，最大响应时间不详
    at_cmd_param_init(&g_at_cmd, AT_CSOSEND, buf, CMD_SET, 3000);
    
    //进入tcp/udp数据发送状态
    g_sim7020_status.main_status = SIM7020_TCPUDP_SEND;
    g_sim7020_status.sub_status  = SIM7020_SUB_TCPUDP_SEND;

    sim7020_at_cmd_send(sim7020_handle, &g_at_cmd);

    return SIM7020_OK;
}

//以字符串格式发送数据coap协议
int sim7020_nblot_coap_send_str(sim7020_handle_t sim7020_handle, int len, char *msg, sim7020_connect_type_t type)
{ 
    if (g_sim7020_status.main_status != SIM7020_NONE)
    {
        return SIM7020_ERROR;
    }
  
    //判断SOCKET ID是否正确
    if (g_socket_info[0].socket_id  < 0 || g_socket_info[0].socket_id > 5 )
    {
        return SIM7020_ERROR;
    }
      
    //最大数据长度为有效数据加上头部
    int16_t str_len = (SIM7020_SEND_BUF_MAX_LEN - 20) ;


    char  buf[SIM7020_SEND_BUF_MAX_LEN - 20];
    
    memset(buf, 0, str_len);

    int16_t msg_len = snprintf(buf,
                               str_len,
                               "%d,%d,%s%s%s",
                                g_socket_info[0].socket_id,
                                0,
                                "\"",                                  
                                msg,
                                "\"");
    
    if (msg_len < 0) {
      
        return SIM7020_ERROR;
    }
    
//    SIM7020_DEBUG_INFO("send buf len %d\r\n",msg_len); 
                                                          
    //构建TCP/UDP数据发送命令，最大响应时间不详
    at_cmd_param_init(&g_at_cmd, AT_CSOSEND, buf, CMD_SET, 3000);
    
    //进入tcp/udp数据发送状态
    g_sim7020_status.main_status = SIM7020_TCPUDP_SEND;
    g_sim7020_status.sub_status  = SIM7020_SUB_TCPUDP_SEND;

    sim7020_at_cmd_send(sim7020_handle, &g_at_cmd);
    
    
//    SIM7020_DEBUG_INFO("send buf %s", g_sim7020_send_desc.buf);    

    return SIM7020_OK;
}   



