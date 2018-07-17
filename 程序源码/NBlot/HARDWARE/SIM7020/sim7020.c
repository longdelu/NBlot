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
   
//处理sim7020事件通知
static uint8_t sim7020_notify (sim7020_handle_t sim7020_handle, char *buf);

//发送消息与应用层交互
static void sim7020_msg_send (sim7020_handle_t sim7020_handle, char**buf, int8_t is_ok);

//sim7020发送AT指令
//sim7020_handle   sim7020_handle设备句柄
//cmd_handle       将要发送指令信息句柄
//note 调用该函数前先构造好命令的参数
static int sim7020_at_cmd_send(sim7020_handle_t sim7020_handle, at_cmdhandle cmd_handle);

//sim7020接收数据
static int sim7020_data_recv(sim7020_handle_t sim7020_handle, uint32_t timeout);

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
    
    if (p_uart_dev->uart_event & UART_TX_EVENT) {
               
        printf("sim7020 tx ok %s", g_sim7020_send_desc.buf);  

        lpuart_event_clr(p_uart_dev, UART_TX_EVENT);                 
    }

    if (p_uart_dev->uart_event & UART_RX_EVENT) {
                
        g_sim7020_recv_desc.len = uart_ring_buf_avail_len(p_uart_dev);
        
        //从缓冲区当中读取数据
        if (g_sim7020_recv_desc.len > 0)
        {
                        
            sim7020_data_recv(sim7020_handle, 0);
                       
            //产生异步事件等待处理
            sim7020_notify(sim7020_handle, g_sim7020_recv_desc.buf);
            
            printf("sim7020 rx ok %s\r\n", g_sim7020_recv_desc.buf);
            
        }
                                                                                 
        lpuart_event_clr(p_uart_dev, UART_RX_EVENT); 
    } 

    //发生发送超时事件，说明指令有可能没有发送出去，或者发送过程中出错
    //或者模块工作异常，没有回应命令的数据
    if (p_uart_dev->uart_event & UART_TX_TIMEOUT_EVENT) {
        
        printf("sim7020 tx timeout %s", g_sim7020_send_desc.buf);  

        sim7020_event_set(sim7020_handle, SIM7020_TIMEOUT_EVENT);
      
        lpuart_event_clr(p_uart_dev, UART_TX_TIMEOUT_EVENT); 
    } 

    //如果使用非超时成帧，此事件理论上不会发生
    if (p_uart_dev->uart_event & UART_RX_TIMEOUT_EVENT) {
        
        g_sim7020_recv_desc.len = uart_ring_buf_avail_len(p_uart_dev);
              

        if (g_sim7020_dev.frame_format == 1) 
        {
            if (g_sim7020_recv_desc.len > 0)
            {
                
                sim7020_data_recv(sim7020_handle, 0);
                
                //产生异步事件等待处理
                sim7020_notify(sim7020_handle, g_sim7020_recv_desc.buf);
            }
                                        
       //不在超时成帧的状态下，代表的确发生了超时事件      
       } else {
         
           sim7020_event_set(sim7020_handle, SIM7020_TIMEOUT_EVENT);
       }
       
       printf("sim7020 rx timeout %s\r\n", g_sim7020_recv_desc.buf);   
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
    
//    memcpy(tmp_buf,g_sim7020_recv_desc.buf, 512); 
        
    if (sim7020_handle->sim7020_event & SIM7020_RECV_EVENT) {
           
        cmd_is_pass = at_cmd_result_parse(g_sim7020_recv_desc.buf);
        
        //如果命令响应结果正确      
        if (cmd_is_pass == AT_CMD_RESULT_OK) 
        {            
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

            if(index == 0)
            {
                //数据解析出错，重新发送该命令
                next_cmd = sim7020_response_handle(sim7020_handle, FALSE);
              
                //清缓存            
                sim7020_recv_buf_reset();                            
                                        
                sim7020_event_clr(sim7020_handle, SIM7020_RECV_EVENT); 
                                       
                //未收到正确的数据帧
                return  SIM7020_ERROR;
            }            
                                   
        }
        
        if(cmd_is_pass == AT_CMD_RESULT_OK)
        {
            
              printf("%s cmd excute ok \r\n\r\n", g_at_cmd.p_atcmd);
            
              //代表命令发送成功了并得到正确的响应
              sim7020_msg_send(sim7020_handle, at_response_par, TRUE);
                
              next_cmd = sim7020_response_handle(sim7020_handle, TRUE);
              
              //清缓存            
              sim7020_recv_buf_reset();
        }
        
        else if(cmd_is_pass == AT_CMD_RESULT_ERROR)
        {              
              
              next_cmd = sim7020_response_handle(sim7020_handle, FALSE);     
          
              if (g_at_cmd.cmd_action & ACTION_ERROR_AND_TRY) { 
                                    
                  printf("%s cmd is failed and try\r\n", g_at_cmd.p_atcmd);
                  
                  at_response_par[AT_CMD_RESPONSE_PAR_NUM_MAX - 1] = "error and try\r\n";
                  
                  //通知上层应用，此动作执行失败,但会重试
                  sim7020_msg_send(sim7020_handle, &at_response_par[AT_CMD_RESPONSE_PAR_NUM_MAX - 1], SIM7020_ERROR_RETRY);
                  
              } else if (g_at_cmd.cmd_action & ACTION_ERROR_BUT_NEXT) {
                  
                  at_response_par[AT_CMD_RESPONSE_PAR_NUM_MAX - 1] = "error but next\r\n";
                 
                  printf("%s cmd is failed and exit\r\n", g_at_cmd.p_atcmd);
                  
                  //通知上层应用，此动作执行失败后跳过该命令执行
                  sim7020_msg_send(sim7020_handle, &at_response_par[AT_CMD_RESPONSE_PAR_NUM_MAX - 1], SIM7020_ERROR_NEXT);
                  
              } else {
                  
                  at_response_par[AT_CMD_RESPONSE_PAR_NUM_MAX - 1] = "over max try, but failed\r\n";
                  
                  //通知上层应用，此动作执行失败后跳过该命令执行
                  sim7020_msg_send(sim7020_handle, &at_response_par[AT_CMD_RESPONSE_PAR_NUM_MAX - 1], FALSE);                  

              } 
                      
              //清缓存            
              sim7020_recv_buf_reset();
        } 

        else if (cmd_is_pass == AT_CMD_RESULT_CONTINUE)
        {
            //命令未执行完成，正常情况下，收到的的是命令回显, 接下来的还是当前命令响应数据的接收, 重新启动接收超时               
            atk_soft_timer_timeout_change(&sim7020_handle->p_uart_dev->uart_rx_timer, 500);
          
        }       
        else 
        {
            //理论上不会运行到这里,         
            //传输出错，延时一段时间，防止还有数据过来，这些数据都是垃圾数据

            if (g_sim7020_recv_desc.len > 0) {          
                                    
                next_cmd = sim7020_response_handle(sim7020_handle, FALSE); 
            }              
          
            printf("the err buf %s\r\n",g_sim7020_recv_desc.buf);

            printf("the err buf len %d\r\n",g_sim7020_recv_desc.len);             
        
            //清缓存            
            sim7020_recv_buf_reset();          

        }            
        
        sim7020_event_clr(sim7020_handle, SIM7020_RECV_EVENT); 
    }

    if (sim7020_handle->sim7020_event & SIM7020_TIMEOUT_EVENT) {
                          
        at_response_par[AT_CMD_RESPONSE_PAR_NUM_MAX - 1] = "cmd not repsonse or send failed\r\n";
      
        printf("%s cmd not repsonse or send failed\r\n", g_at_cmd.p_atcmd);
        
        //通知上层应用，此动作执行超时
        sim7020_msg_send(sim7020_handle, &at_response_par[AT_CMD_RESPONSE_PAR_NUM_MAX - 1], SIM7020_ERROR_TIMEOUT);   
          
        sim7020_recv_buf_reset();      
        
        //超时处理        
        next_cmd = sim7020_response_handle(sim7020_handle, FALSE);
      
        sim7020_event_clr(sim7020_handle, SIM7020_RECV_EVENT); 
                                          
        sim7020_event_clr(sim7020_handle, SIM7020_TIMEOUT_EVENT); 
    } 

    if (sim7020_handle->sim7020_event & SIM7020_REG_STA_EVENT) {
      
        printf("reg ok\r\n");
        
        at_response_par[AT_CMD_RESPONSE_PAR_NUM_MAX - 1] = (char *)&g_sim7020_status.register_status;        
          
        //通知上层应用网络注册结果
        sim7020_msg_send(sim7020_handle, &at_response_par[AT_CMD_RESPONSE_PAR_NUM_MAX - 1], TRUE);        

        //处理决定是否执行下一条命令        
        next_cmd = sim7020_response_handle(sim7020_handle, TRUE);
        
        sim7020_event_clr(sim7020_handle,   SIM7020_REG_STA_EVENT); 
    }
    
    if (sim7020_handle->sim7020_event & SIM7020_TCP_RECV_EVENT) {
        printf("tcp recv ok\r\n");
        
        sim7020_event_clr(sim7020_handle,SIM7020_TCP_RECV_EVENT); 
    }

    if (sim7020_handle->sim7020_event & SIM7020_UDP_RECV_EVENT) {
        printf("udp recv ok\r\n");
        
        sim7020_event_clr(sim7020_handle, SIM7020_UDP_RECV_EVENT); 
    }
    
    if (sim7020_handle->sim7020_event & SIM7020_COAP_RECV_EVENT) {
        
        printf("coap recv ok\r\n");
        
        sim7020_event_clr(sim7020_handle, SIM7020_COAP_RECV_EVENT); 
    }

    //命令还是next
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

    return SIM7020_OK;    
}

//sim7020 状态处理函数
//sim7020_main_status；sim7020所处的主状态阶段
void sim7020_app_status_poll(sim7020_handle_t sim7020_handle, int *sim7020_main_status)
{    
    switch(*sim7020_main_status)
    {
    case SIM7020_NONE:
      {
        
      }
      break;
           
    case SIM7020_NBLOT_INIT:
      {
        printf("sim7020 init start\r\n");
                
        sim7020_nblot_init(sim7020_handle);        

        *sim7020_main_status = SIM7020_END;
      }
      break;
      
    case SIM7020_NBLOT_INFO:
      {
         printf("sim7020 get signal start\r\n");
        
        
         sim7020_nblot_info_get(sim7020_handle);

         *sim7020_main_status = SIM7020_END;
      }
            
      break;
      
    case SIM7020_SIGNAL:
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
    
//    ret = uart_data_rx_int(uart_handle, pData, size, Timeout); 

    return ret;    
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
    memset(g_sim7020_send_desc.buf,0,NB_UART_SEND_BUF_MAX_LEN);
    g_sim7020_send_desc.len = 0;

    if(cmd_handle->property == CMD_TEST)
    {
        cmdLen = snprintf(g_sim7020_send_desc.buf,NB_UART_SEND_BUF_MAX_LEN,
                          "%s=?\r\n",
                          cmd_handle->p_atcmd);
    }    
    else if(cmd_handle->property == CMD_READ)
    {
        cmdLen = snprintf(g_sim7020_send_desc.buf,NB_UART_SEND_BUF_MAX_LEN,
                          "%s?\r\n",
                          cmd_handle->p_atcmd);
    }
    else if(cmd_handle->property == CMD_EXCUTE)
    {
        cmdLen = snprintf(g_sim7020_send_desc.buf,NB_UART_SEND_BUF_MAX_LEN,
                          "%s\r\n",
                          cmd_handle->p_atcmd);    
    }

    else if(cmd_handle->property == CMD_SET)
    {
        cmdLen = snprintf(g_sim7020_send_desc.buf,NB_UART_SEND_BUF_MAX_LEN,
                          "%s=%s\r\n",
                          cmd_handle->p_atcmd,cmd_handle->p_atcmd_arg);    
    }
    
    g_sim7020_send_desc.len = cmdLen;
    
    return cmdLen;
}


//判断at指令是否发送成功
//返回值如果是-1，则接收到的数据可能为乱码
//返回值如果是0， 指令通过，如果是1，指令出错
static int8_t at_cmd_result_parse(char* buf)
{
    int8_t result = -1;
       
    if(g_at_cmd.p_expectres == NULL)
    {
        if (strstr(buf,"\r\nOK\r\n"))
        {
            result = AT_CMD_RESULT_OK;
        }
        else if (strstr(buf,"\r\nERROR\r\n"))
        {
            result = AT_CMD_RESULT_ERROR;
          
        } else if (strstr(buf,g_at_cmd.p_atcmd)) {
          
           //是命令的回显
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
static uint8_t sim7020_notify (sim7020_handle_t sim7020_handle, char *buf)
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
        else 
        {
            
            //如果收到回复，其它是命令响应的数据
            sim7020_event_set(sim7020_handle, SIM7020_RECV_EVENT);  
        }
    }
    
    else if((target_pos_start = strstr(buf,"+CSONMI")) != NULL)
    {
        //收到服务器端发来的SOCKET包数据
        char* p_colon = strchr(target_pos_start,':');
        
        if(p_colon)
        {
          p_colon++;
          g_socket_info[0].socket_id = strtoul(p_colon,0,10);
        }
        
        char* pComma = strchr(p_colon,',');

        if(pComma)
        {
          pComma++;
          g_socket_info[0].data_len = strtoul(pComma,0,10);
        }
        
        if (g_socket_info[0].socket_type == 2) {

           sim7020_event_set(sim7020_handle, SIM7020_UDP_RECV_EVENT); 
            
        } else if (g_socket_info[0].socket_type == 1) {

           sim7020_event_set(sim7020_handle, SIM7020_TCP_RECV_EVENT); 
        }            
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
            //设置期望回复消息为络
            //如果指令执行完成,没有与期望的消息匹配                                            
            //则认为出错，并进行出错尝试               
            g_at_cmd.p_expectres = "CGREG: 1,1";              

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
        
      sim7020_handle->sim7020_cb(sim7020_handle->p_arg, (sim7020_msg_id_t)g_sim7020_status.main_status, strlen(buf[0]), buf[0]);  
        
      return;      
  }
      
  //出错，则上报此流程执行失败
  else if(is_ok == FALSE)
  {
    sim7020_handle->sim7020_cb(sim7020_handle->p_arg, (sim7020_msg_id_t)g_sim7020_status.main_status,1,"F");
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
        char *pColon = strchr(buf[0],':');
        pColon++;
      
        //转换成10进制数字
        uint8_t lqi =strtoul(pColon,0, 10);
      
        printf("rssi = %d\r\n",lqi); 
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

      sim7020_handle->sim7020_cb(sim7020_handle->p_arg, (sim7020_msg_id_t)SIM7020_MSG_REG, 1, buf[0]);
    
      break;        
        
                 
    case SIM7020_SUB_CGMI:
      {
        sim7020_handle->sim7020_cb(sim7020_handle->p_arg, (sim7020_msg_id_t)SIM7020_MSG_MID, strlen(buf[0]), buf[0]);
      }
      break;
      
    case SIM7020_SUB_CGMM:
      {
        sim7020_handle->sim7020_cb(sim7020_handle->p_arg, (sim7020_msg_id_t)SIM7020_MSG_MMODEL,strlen(buf[0]),buf[0]);
      }
      break;
      
    case SIM7020_SUB_CGMR:
      {

         sim7020_handle->sim7020_cb(sim7020_handle->p_arg, (sim7020_msg_id_t)SIM7020_MSG_MREV,strlen(buf[0]),buf[0]);
        
      }
      break;
      
      
    case SIM7020_SUB_CIMI:
      {
        memcpy(g_firmware_info.IMSI,buf[0],15);
        g_firmware_info.IMSI[15] = 0;
        sim7020_handle->sim7020_cb(sim7020_handle->p_arg, (sim7020_msg_id_t)SIM7020_MSG_IMSI,strlen(buf[0]),buf[0]);
      }
      break;
      
    
    case SIM7020_SUB_CGSN:
      {
        char* pColon = strchr(buf[0],':');
        if(pColon)
        {
          pColon++;
          memcpy(g_firmware_info.IMEI ,pColon,15);
          g_firmware_info.IMEI[15] = 0;
          sim7020_handle->sim7020_cb(sim7020_handle->p_arg, (sim7020_msg_id_t)SIM7020_MSG_IMEI,15,(char*)g_firmware_info.IMEI);
        }
      }
      break;            
      
    case SIM7020_SUB_CBAND:
      {
        char *pColon = strchr(buf[0],':');
        char *pFreq = NULL;
        if(pColon)
        {
          pColon++;
          uint8_t hz_id = strtoul(pColon,0,10);
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
    if(g_sim7020_status.sub_status == 1)
    {
      char* pColon = strchr(buf[0],':');
      pColon++;
      //转换成10进制数字
      uint8_t lqi =strtoul(pColon,0, 10);
      //运算取得每个数值对应的dbm范围
      int8_t rssi = -110 + (lqi << 1);
      uint8_t len = snprintf(buf[0],10,"%d",rssi);
      *(buf[0]+len) = 0;
      
      g_sim7020_status.rssi = rssi;
      sim7020_handle->sim7020_cb(sim7020_handle->p_arg,(sim7020_msg_id_t)SIM7020_MSG_SIGN,len,buf[0]);
    }

  }
  else if(g_sim7020_status.main_status == SIM7020_UDP_CR)
  {
    switch(g_sim7020_status.sub_status)
    {
    case SIM7020_SUB_UDP_CR:
      {
//        memcpy(g_firmware_info.nb95_udp_id,buf[0],2);
        
        if(g_sim7020_status.sub_status == 1)
        {
          sim7020_handle->sim7020_cb(sim7020_handle->p_arg,(sim7020_msg_id_t)SIM7020_UDP_CR,1,"S");
        }
      }
      break;
      
    case SIM7020_SUB_END:
      {
        sim7020_handle->sim7020_cb(sim7020_handle->p_arg, (sim7020_msg_id_t)SIM7020_MSG_UDP_CREATE,1,"S");
      }
      break;
    }
  }
  else if(g_sim7020_status.main_status == SIM7020_UDP_CL)
  {
    switch(g_sim7020_status.sub_status)
    {
    case SIM7020_SUB_UDP_CL:
      {
//        g_firmware_info.nb95_udp_id[0] = 0;
//        g_firmware_info.nb95_udp_id[1] = 0;
      }
      break;
    case SIM7020_SUB_END:
      {
        sim7020_handle->sim7020_cb(sim7020_handle->p_arg, (sim7020_msg_id_t)SIM7020_MSG_UDP_CLOSE,1,"S");
      }
      break;
    } 

  }
  else if(g_sim7020_status.main_status == SIM7020_UDP_SEND)
  {
    switch(g_sim7020_status.sub_status)
    {
    case SIM7020_SUB_UDP_ST:
      {
        sim7020_handle->sim7020_cb(sim7020_handle->p_arg, (sim7020_msg_id_t)SIM7020_MSG_UDP_SEND,1,"S");
      }
      break;
    }
  }
  else if(g_sim7020_status.main_status == SIM7020_UDP_RECV)
  {
    if(g_sim7020_status.sub_status == 1)
    {
      //
      char* param[6];
      uint16_t index = 0;
      char* tmp_buf = buf[0];
      while(( param[index] = strtok(tmp_buf,",")) != NULL)
      {
        index++;
        tmp_buf = NULL;
        if(index >= 6)
        {
          break;
        }
      }
      if(index != 6)
      {
        sim7020_handle->sim7020_cb(sim7020_handle->p_arg, (sim7020_msg_id_t)SIM7020_MSG_UDP_RECV,1,"F");
        return;
      }
      
      tmp_buf = param[4];
      index =  0; //NB_HexStrToNum(tmp_buf);
      
      sim7020_handle->sim7020_cb(sim7020_handle->p_arg,(sim7020_msg_id_t)SIM7020_MSG_UDP_RECV,index,tmp_buf);
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
        index = 0;//NB_HexStrToNum(tmp_buf);
        sim7020_handle->sim7020_cb(sim7020_handle->p_arg, (sim7020_msg_id_t)SIM7020_MSG_COAP_RECV, index,tmp_buf); 
      }
   
    }
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
//sim7020_handle   sim7020_handle设备句柄
static int sim7020_data_recv(sim7020_handle_t sim7020_handle, uint32_t timeout)
{   
    int ret = 0;
        
    if (sim7020_handle == NULL)
    {
       return SIM7020_ERROR;
    }
        

    ret = sim7020_handle->p_drv_funcs->sim7020_recv_data(sim7020_handle, 
                                                         (uint8_t*)g_sim7020_recv_desc.buf, 
                                                         g_sim7020_recv_desc.len,                                                                                                     
                                                         timeout);
    
    //在后面添加字符串结束符
    g_sim7020_recv_desc.buf[g_sim7020_recv_desc.len]=0;
    
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
    
    return SUCCESS;
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
    
    return SUCCESS;
}

   





