
////////////////////////////////////////////////////////////////////////////////// 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F429开发板
//串口1初始化           
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2018/7/11
//版本：V1.5
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//********************************************************************************
#include "atk_bc28_nbiot.h"
#include "atk_delay.h"

#define SIM7020_NBLOT_DEBUG                   
#ifdef SIM7020_NBLOT_DEBUG
#define SIM7020_NBLOT_DEBUG_INFO(...)    (int)printf(__VA_ARGS__)
#else
#define SIM7020_NBLOT_DEBUG_INFO(...)
#endif

static char cmd_buf_temp[SIM7020_RECV_BUF_MAX_LEN] = {0};

//sim7020 nblot初始化及完成网络注册
int sim7020_nblot_init (sim7020_handle_t sim7020_handle)
{
    
    if (sim7020_handle->sim7020_sm_status->main_status != SIM7020_NONE)
    {
        return SIM7020_ERROR;
    }

    at_cmd_param_init(sim7020_handle->p_sim7020_cmd, AT_SYNC, NULL, CMD_EXCUTE, 3000);

    //进入SIM7020_NBLOT_INIT状态
    sim7020_handle->sim7020_sm_status->main_status = SIM7020_NBLOT_INIT;
    sim7020_handle->sim7020_sm_status->sub_status  = SIM7020_SUB_SYNC;

    sim7020_at_cmd_send(sim7020_handle, sim7020_handle->p_sim7020_cmd);

    return SIM7020_OK;
}

//获取NB模块的信息
int sim7020_nblot_info_get(sim7020_handle_t sim7020_handle)
{

    if (sim7020_handle->sim7020_sm_status->main_status != SIM7020_NONE)
    {
        return SIM7020_ERROR;
    }
    
    
    at_cmd_param_init(sim7020_handle->p_sim7020_cmd, AT_CGREG, NULL, CMD_READ, 3000);

    //进入SIM7020_NBLOT_INFO状态
    sim7020_handle->sim7020_sm_status->main_status = SIM7020_NBLOT_INFO;
    sim7020_handle->sim7020_sm_status->sub_status  = SIM7020_SUB_CEREG_QUERY;

    sim7020_at_cmd_send(sim7020_handle, sim7020_handle->p_sim7020_cmd);
    
    return SIM7020_OK;
}


//获取NB模块的信号质量
int sim7020_nblot_signal_get(sim7020_handle_t sim7020_handle)
{

    if (sim7020_handle->sim7020_sm_status->main_status != SIM7020_NONE)
    {
        return SIM7020_ERROR;
    }
        
    at_cmd_param_init(sim7020_handle->p_sim7020_cmd, AT_CSQ, NULL, CMD_EXCUTE, 3000);

    //进入SIM7020_SIGNAL状态
    sim7020_handle->sim7020_sm_status->main_status = SIM7020_SIGNAL;
    sim7020_handle->sim7020_sm_status->sub_status  = SIM7020_SUB_CSQ;

    sim7020_at_cmd_send(sim7020_handle, sim7020_handle->p_sim7020_cmd);
    
    return SIM7020_OK;
}

//创建tcpudp 
int sim7020_nblot_tcpudp_create(sim7020_handle_t sim7020_handle, sim7020_connect_type_t type)
{
    char *p_tcpudp = NULL;
  
    if (sim7020_handle->sim7020_sm_status->main_status != SIM7020_NONE)
    {
        return SIM7020_ERROR;
    }
    
    if (type == SIM7020_TCP)
    {
        sim7020_handle->p_socket_info[0].socket_type = SIM7020_TCP;
        p_tcpudp = "1,1,1";
    }
    
    else if(type == SIM7020_UDP)
    {   
        sim7020_handle->p_socket_info[0].socket_type = SIM7020_UDP;
        p_tcpudp = "1,2,1";
    } 
    else 
    {
       return SIM7020_NOTSUPPORT;
      
    }
            
    at_cmd_param_init(sim7020_handle->p_sim7020_cmd, AT_CSOC, p_tcpudp, CMD_SET, 3000);
    
    //进入创建TCP/UDP状态
    sim7020_handle->sim7020_sm_status->main_status = SIM7020_TCPUDP_CR;
    sim7020_handle->sim7020_sm_status->sub_status  = SIM7020_SUB_TCPUDP_CR;

    sim7020_at_cmd_send(sim7020_handle, sim7020_handle->p_sim7020_cmd);
    
    return SIM7020_OK;
}


//关闭tcpudp
int sim7020_nblot_tcpudp_close(sim7020_handle_t sim7020_handle, sim7020_connect_type_t type)
{

    if (sim7020_handle->sim7020_sm_status->main_status != SIM7020_NONE)
    {
        return SIM7020_ERROR;
    }
    
       
    (void)type;
    
    //不能使用栈上的内存分配数据，要不然重发命令数据时会出错   
    memset(cmd_buf_temp, 0, sizeof(cmd_buf_temp));
       
    int16_t msg_len = snprintf(cmd_buf_temp,
                                sizeof(cmd_buf_temp),
                                "%d",
                                sim7020_handle->p_socket_info[0].socket_id);
                                
    if (msg_len < 0) {
      
        return SIM7020_ERROR;
    }
                                                   
    at_cmd_param_init(sim7020_handle->p_sim7020_cmd, AT_CSOCL, cmd_buf_temp, CMD_SET, 3000);

    //进入tcp/udp关闭状态
    sim7020_handle->sim7020_sm_status->main_status = SIM7020_TCPUDP_CL;
    sim7020_handle->sim7020_sm_status->sub_status  = SIM7020_SUB_TCPUDP_CL;

    sim7020_at_cmd_send(sim7020_handle, sim7020_handle->p_sim7020_cmd);
        
    return SIM7020_OK;
}


//以hex数据格式发送数据,必须是偶数个长度
int sim7020_nblot_tcpudp_send_hex(sim7020_handle_t sim7020_handle, int len, char *msg, sim7020_connect_type_t type)
{
  
    if (sim7020_handle->sim7020_sm_status->main_status != SIM7020_NONE)
    {
        return SIM7020_ERROR;
    }
  
    //判断SOCKET ID是否正确
    if (sim7020_handle->p_socket_info[0].socket_id  < 0 || sim7020_handle->p_socket_info[0].socket_id > 5 )
    {
        return SIM7020_ERROR;
    }
  
    //最大数据长度为有效数据加上头部
    int16_t str_len = (SIM7020_SEND_BUF_MAX_LEN - 20) ;

    //不能使用栈上的内存分配数据，要不然重发命令因栈上的内存数据释放掉时会出错   
    memset(cmd_buf_temp, 0, sizeof(cmd_buf_temp));


    uint16_t msg_len = snprintf(cmd_buf_temp,
                                str_len,
                                "%d,%d,",
                                 sim7020_handle->p_socket_info[0].socket_id, 
                                len);
                                
    for(uint16_t i = 0 ; i < len ; i++)
    {
        sprintf(&cmd_buf_temp[msg_len + (i << 1)],"%02X",(uint8_t)msg[i]);
    }                             
 
    //构建TCP/UDP数据发送命令，最大响应时间不详
    at_cmd_param_init(sim7020_handle->p_sim7020_cmd, AT_CSOSEND, cmd_buf_temp, CMD_SET, 3000);
    
    //进入tcp/udp数据发送状态
    sim7020_handle->sim7020_sm_status->main_status = SIM7020_TCPUDP_SEND;
    sim7020_handle->sim7020_sm_status->sub_status  = SIM7020_SUB_TCPUDP_SEND;

    sim7020_at_cmd_send(sim7020_handle, sim7020_handle->p_sim7020_cmd);

    return SIM7020_OK;
}

//以字符串格式发送数据
int sim7020_nblot_tcpudp_send_str(sim7020_handle_t sim7020_handle, int len, char *msg, sim7020_connect_type_t type)
{
  
    if (sim7020_handle->sim7020_sm_status->main_status != SIM7020_NONE)
    {
        return SIM7020_ERROR;
    }
  
    //判断SOCKET ID是否正确
    if (sim7020_handle->p_socket_info[0].socket_id  < 0 || sim7020_handle->p_socket_info[0].socket_id > 5 )
    {
        return SIM7020_ERROR;
    }
      

    //最大数据长度为有效数据加上头部
    int16_t str_len = (SIM7020_SEND_BUF_MAX_LEN - 20) ;

    //不能使用栈上的内存分配数据，要不然重发命令因栈上的内存数据释放掉时会出错   
    memset(cmd_buf_temp, 0, sizeof(cmd_buf_temp));
    

    int16_t msg_len = snprintf(cmd_buf_temp,
                               str_len,
                               "%d,%d,%s%s%s",
                                sim7020_handle->p_socket_info[0].socket_id,
                                0,
                                "\"",                                  
                                msg,
                                "\"");
    
    if (msg_len < 0) {
      
        return SIM7020_ERROR;
    }
    
                                                          
    //构建TCP/UDP数据发送命令，最大响应时间不详
    at_cmd_param_init(sim7020_handle->p_sim7020_cmd, AT_CSOSEND, cmd_buf_temp, CMD_SET, 3000);
    
    //进入tcp/udp数据发送状态
    sim7020_handle->sim7020_sm_status->main_status = SIM7020_TCPUDP_SEND;
    sim7020_handle->sim7020_sm_status->sub_status  = SIM7020_SUB_TCPUDP_SEND;

    sim7020_at_cmd_send(sim7020_handle, sim7020_handle->p_sim7020_cmd);
    
    return SIM7020_OK;
}   

//创建coap服务器 
int sim7020_nblot_coap_server_create(sim7020_handle_t sim7020_handle, sim7020_connect_type_t type)
{
   
    if (sim7020_handle->sim7020_sm_status->main_status != SIM7020_NONE)
    {
        return SIM7020_ERROR;
    }
    
    if (type != SIM7020_COAP)
    {
        return SIM7020_NOTSUPPORT;
    }
    
    sim7020_handle->sim7020_connect_status->connect_type = SIM7020_COAP;  
    sim7020_handle->sim7020_connect_status->cid          = 1;
    sim7020_handle->sim7020_connect_status->connect_id   = 1;
    
    
    //不能使用栈上的内存分配数据，要不然重发命令因栈上的内存数据释放掉时会出错   
    memset(cmd_buf_temp, 0, sizeof(cmd_buf_temp));    
               
                       
    //这个函数的返回值为想要   格式化写入的长度，并会在字符串结束后面自动加入结束字符
    uint16_t coap_cn_len = snprintf(cmd_buf_temp,
                                    sizeof(cmd_buf_temp) -1,"%s,%s,%d",                                                                                             
                                    REMOTE_SERVER_IP,
                                    REMOTE_COAP_PORT,
                                    sim7020_handle->sim7020_connect_status->cid);
                                        
                     
            
    //最大响应时间不详                                              
    at_cmd_param_init(sim7020_handle->p_sim7020_cmd, AT_CCOAPSTA, cmd_buf_temp, CMD_SET, 3000);
    sim7020_handle->p_sim7020_cmd->cmd_action = ACTION_OK_AND_NEXT | ACTION_ERROR_BUT_NEXT;                                
    
    //进入设置COAP服务器状态
    sim7020_handle->sim7020_sm_status->main_status = SIM7020_CoAP_SEVER;
    sim7020_handle->sim7020_sm_status->sub_status  = SIM7020_SUB_CoAP_SEVER;

    sim7020_at_cmd_send(sim7020_handle, sim7020_handle->p_sim7020_cmd);
    
    return SIM7020_OK;
}

//创建coap客户端
int sim7020_nblot_coap_client_create(sim7020_handle_t sim7020_handle, sim7020_connect_type_t type)
{
     
    if (sim7020_handle->sim7020_sm_status->main_status != SIM7020_NONE)
    {
        return SIM7020_ERROR;
    }
    
    if (type != SIM7020_COAP)
    {
        return SIM7020_NOTSUPPORT;
    }
    
    sim7020_handle->sim7020_connect_status->connect_type = SIM7020_COAP;
    sim7020_handle->sim7020_connect_status->cid          = 1;  

    //不能使用栈上的内存分配数据，要不然重发命令因栈上的内存数据释放掉时会出错   
    memset(cmd_buf_temp, 0, sizeof(cmd_buf_temp));       
                
    //这个函数的返回值为想要格式化写入的长度，并会在字符串结束后面自动加入结束字符
    uint16_t coap_cn_len = snprintf(cmd_buf_temp,
                                    sizeof(cmd_buf_temp) -1,"%s,%s,%d",                                                                                             
                                    REMOTE_SERVER_IP,
                                    REMOTE_COAP_PORT,
                                    sim7020_handle->sim7020_connect_status->cid);
                                                                       
    //最大响应时间不详                                              
    at_cmd_param_init(sim7020_handle->p_sim7020_cmd, AT_CCOAPNEW, cmd_buf_temp, CMD_SET, 3000);
    sim7020_handle->p_sim7020_cmd->cmd_action = ACTION_OK_AND_NEXT | ACTION_ERROR_BUT_NEXT; 
                                       
    //进入创建客户端状态
    sim7020_handle->sim7020_sm_status->main_status = SIM7020_CoAP_CLIENT;
    sim7020_handle->sim7020_sm_status->sub_status  = SIM7020_SUB_CoAP_CLIENT;

    sim7020_at_cmd_send(sim7020_handle, sim7020_handle->p_sim7020_cmd);
    
    return SIM7020_OK;
}


//关闭coap
int sim7020_nblot_coap_close(sim7020_handle_t sim7020_handle, sim7020_connect_type_t type)
{
  
    if (sim7020_handle->sim7020_sm_status->main_status != SIM7020_NONE)
    {
        return SIM7020_ERROR;
    }
           
    if (type != SIM7020_COAP)
    {
        return SIM7020_NOTSUPPORT;
    }
    
    sim7020_handle->sim7020_connect_status->connect_type = SIM7020_COAP;  

    //通过客户端id来销毁COAP链接
    cmd_buf_temp[0] = sim7020_handle->sim7020_connect_status->connect_id + '0'; 
    cmd_buf_temp[1] = 0;
    cmd_buf_temp[2] = 0;     
                                                                                   
    at_cmd_param_init(sim7020_handle->p_sim7020_cmd, AT_CCOAPDEL, cmd_buf_temp, CMD_SET, 3000);

    //进入coap关闭状态,最大响应时间不详
    sim7020_handle->sim7020_sm_status->main_status = SIM7020_CoAP_CL;
    sim7020_handle->sim7020_sm_status->sub_status  = SIM7020_SUB_CoAP_CL;

    sim7020_at_cmd_send(sim7020_handle, sim7020_handle->p_sim7020_cmd);
    
    
    return SIM7020_OK;
}


//以hex数据格式发送coap协议数据,必须是偶数个长度
int sim7020_nblot_coap_send_hex(sim7020_handle_t sim7020_handle, int len, char *msg, sim7020_connect_type_t type)
{ 
    if (sim7020_handle->sim7020_sm_status->main_status != SIM7020_NONE)
    {
        return SIM7020_ERROR;
    }
  
    //判断coap id是否正确
    if (sim7020_handle->sim7020_connect_status->connect_id  < 0)
    {
        return SIM7020_ERROR;
    }
    
    if (type != SIM7020_COAP)
    {
        return SIM7020_NOTSUPPORT;
    }    
  
    //最大数据长度为有效数据加上头部
    int16_t str_len = (SIM7020_SEND_BUF_MAX_LEN - 24) ;

    //不能使用栈上的内存分配数据，要不然重发命令因栈上的内存数据释放掉时会出错   
    memset(cmd_buf_temp, 0, sizeof(cmd_buf_temp));    


    uint16_t msg_len = snprintf(cmd_buf_temp,
                                str_len,
                                "%d,%d,",
                                 sim7020_handle->sim7020_connect_status->connect_id, 
                                len);
    
    cmd_buf_temp[msg_len] = '\"';
    
    
    
    for(uint16_t i = 0 ; i < len ; i++)
    {
        //表示一个字符用十六进制表示 
        sprintf(&cmd_buf_temp[msg_len + 1 + (i << 1)],"%02X",(uint8_t)msg[i]);
    }

    cmd_buf_temp[msg_len + 1 + (len << 1)] = '\"';    
 
    //构建coap数据发送命令，最大响应时间不详
    at_cmd_param_init(sim7020_handle->p_sim7020_cmd, AT_CCOAPSEND, cmd_buf_temp, CMD_SET, 3000);
    
    //进入coap数据发送状态
    sim7020_handle->sim7020_sm_status->main_status = SIM7020_CoAP_SEND;
    sim7020_handle->sim7020_sm_status->sub_status  = SIM7020_SUB_CoAP_SEND;

    sim7020_at_cmd_send(sim7020_handle, sim7020_handle->p_sim7020_cmd);

    return SIM7020_OK;
}

//以字符串格式发送数据coap协议数据
int sim7020_nblot_coap_send_str(sim7020_handle_t sim7020_handle, int len, char *msg, sim7020_connect_type_t type)
{ 
    if (sim7020_handle->sim7020_sm_status->main_status != SIM7020_NONE)
    {
        return SIM7020_ERROR;
    }
  
    //判断coap id是否正确
    if (sim7020_handle->sim7020_connect_status->connect_id  < 0)
    {
        return SIM7020_ERROR;
    }
    
    if (type != SIM7020_COAP)
    {
        return SIM7020_NOTSUPPORT;
    } 
      
    //最大数据长度为有效数据加上头部
    int16_t str_len = (SIM7020_SEND_BUF_MAX_LEN - 24) ;


    //不能使用栈上的内存分配数据，要不然重发命令因栈上的内存数据释放掉时会出错   
    memset(cmd_buf_temp, 0, sizeof(cmd_buf_temp));   

    int16_t msg_len = snprintf(cmd_buf_temp,
                               str_len,
                               "%d,%d,%s%s%s",
                                sim7020_handle->sim7020_connect_status->connect_id ,
                                len,
                                "\"",                                  
                                msg,
                                "\"");
    
    if (msg_len < 0) {
      
        return SIM7020_ERROR;
    }
    
                                                          
    //构建coap数据发送命令，最大响应时间不详
    at_cmd_param_init(sim7020_handle->p_sim7020_cmd, AT_CCOAPSEND, cmd_buf_temp, CMD_SET, 3000);
    
    //进入coap数据发送状态
    sim7020_handle->sim7020_sm_status->main_status = SIM7020_CoAP_SEND;
    sim7020_handle->sim7020_sm_status->sub_status  = SIM7020_SUB_CoAP_SEND;

    sim7020_at_cmd_send(sim7020_handle, sim7020_handle->p_sim7020_cmd);
         
    return SIM7020_OK;
}

//创建cm2m客户端
int sim7020_nblot_cm2m_client_create(sim7020_handle_t sim7020_handle, sim7020_connect_type_t type)
{     
    if (sim7020_handle->sim7020_sm_status->main_status != SIM7020_NONE)
    {
        return SIM7020_ERROR;
    }
    
    if (type != SIM7020_CM2M)
    {
        return SIM7020_NOTSUPPORT;
    }
    
    sim7020_handle->sim7020_connect_status->connect_type = SIM7020_CM2M;

    //不能使用栈上的内存分配数据，要不然重发命令因栈上的内存数据释放掉时会出错   
    memset(cmd_buf_temp, 0, sizeof(cmd_buf_temp));       
                
    //这个函数的返回值为想要格式化写入的长度，并会在字符串结束后面自动加入结束字符
    uint16_t cm2m_cn_len = snprintf(cmd_buf_temp,
                                    sizeof(cmd_buf_temp) -1,"%s,%s%s%s,%s%s%s,%d",                                                                                             
                                    REMOTE_SERVER_IP,
                                    "\"",
                                    REMOTE_COAP_PORT,
                                    "\"",
                                    "\"",
                                    sim7020_handle->firmware_info->IMEI,
                                    "\"",
                                    100);
                                                                       
    //最大响应时间不详                                              
    at_cmd_param_init(sim7020_handle->p_sim7020_cmd, AT_CM2MCLINEW, cmd_buf_temp, CMD_SET, 15000);
                                    
    sim7020_handle->p_sim7020_cmd->cmd_action = ACTION_OK_AND_NEXT | ACTION_ERROR_BUT_NEXT; 
                                       
    //进入创建客户端状态
    sim7020_handle->sim7020_sm_status->main_status = SIM7020_CM2M_CLIENT;
    sim7020_handle->sim7020_sm_status->sub_status  = SIM7020_SUB_CM2M_CLIENT;

    sim7020_at_cmd_send(sim7020_handle, sim7020_handle->p_sim7020_cmd);
    
    return SIM7020_OK;
}


//关闭cm2m
int sim7020_nblot_cm2m_close(sim7020_handle_t sim7020_handle, sim7020_connect_type_t type)
{
  
    if (sim7020_handle->sim7020_sm_status->main_status != SIM7020_NONE)
    {
        return SIM7020_ERROR;
    }
           
    if (type != SIM7020_CM2M)
    {
        return SIM7020_NOTSUPPORT;
    }   
                                                                                   
    at_cmd_param_init(sim7020_handle->p_sim7020_cmd, AT_CM2MCLIDEL, NULL, CMD_EXCUTE, 15000);
    sim7020_handle->p_sim7020_cmd->cmd_action = ACTION_OK_AND_NEXT | ACTION_ERROR_BUT_NEXT;     

    //进入cm2m关闭状态,最大响应时间不详
    sim7020_handle->sim7020_sm_status->main_status = SIM7020_CM2M_CL;
    sim7020_handle->sim7020_sm_status->sub_status  = SIM7020_SUB_CM2M_CL;

    sim7020_at_cmd_send(sim7020_handle, sim7020_handle->p_sim7020_cmd);
        
    return SIM7020_OK;
}

//以hex数据格式发送cm2m协议数据,必须是偶数个长度
int sim7020_nblot_cm2m_send_hex(sim7020_handle_t sim7020_handle, int len, char *msg, sim7020_connect_type_t type)
{ 
    if (sim7020_handle->sim7020_sm_status->main_status != SIM7020_NONE)
    {
        return SIM7020_ERROR;
    }
  
    //当类型不为CM2M时，或都发送的数据个数不为偶数个时
    if ((type != SIM7020_CM2M) || ((strlen(msg) % 2) != 0))
    {
        return SIM7020_NOTSUPPORT;
    } 
      
    //最大数据长度为有效数据加上头部
    int16_t str_len = (SIM7020_SEND_BUF_MAX_LEN - 24) ;


    //不能使用栈上的内存分配数据，要不然重发命令因栈上的内存数据释放掉时会出错   
    memset(cmd_buf_temp, 0, sizeof(cmd_buf_temp));   

    int16_t msg_len = snprintf(cmd_buf_temp,
                               str_len,
                               "%s%s%s",                                
                                "\"",                                  
                                msg,
                                "\"");
    
    if (msg_len < 0) {
      
        return SIM7020_ERROR;
    }
    
                                                          
    //构建cm2m数据发送命令，最大响应时间不详
    at_cmd_param_init(sim7020_handle->p_sim7020_cmd, AT_CM2MCLISEND, cmd_buf_temp, CMD_SET, 3000);
    
    //进入cm2m数据发送状态
    sim7020_handle->sim7020_sm_status->main_status = SIM7020_CM2M_SEND;
    sim7020_handle->sim7020_sm_status->sub_status  = SIM7020_SUB_CM2M_SEND;

    sim7020_at_cmd_send(sim7020_handle, sim7020_handle->p_sim7020_cmd);
         
    return SIM7020_OK;
}


