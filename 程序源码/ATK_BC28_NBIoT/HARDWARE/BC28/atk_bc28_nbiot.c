
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

#define NBIOT_DEBUG                   
#ifdef NBIOT_DEBUG
#define NBIOT_DEBUG_INFO(...)    (int)printf(__VA_ARGS__)
#else
#define NBIOT_DEBUG_INFO(...)
#endif

static char cmd_buf_temp[NBIOT_RECV_BUF_MAX_LEN] = {0};

//NB模块初始化及完成网络注册
int nbiot_init (nbiot_handle_t nbiot_handle)
{    
    if (nbiot_handle->p_sm_status->main_status != NBIOT_NONE)
    {
        return NBIOT_ERROR;
    }

    nbiot_at_cmd_param_init(nbiot_handle->p_nbiot_cmd, AT_SYNC, NULL, CMD_EXCUTE, 3000);

    //进入NBIOT_INIT状态
    nbiot_handle->p_sm_status->main_status = NBIOT_INIT;
    nbiot_handle->p_sm_status->sub_status  = NBIOT_SUB_SYNC;

    nbiot_at_cmd_send(nbiot_handle, nbiot_handle->p_nbiot_cmd);
  
    return NBIOT_OK;
}


//重启nbiot模块
int nbiot_reboot(nbiot_handle_t nbiot_handle)
{

    if (nbiot_handle->p_sm_status->main_status != NBIOT_NONE)
    {
        return NBIOT_ERROR;
    }
    
    //命令最大超时时间为300ms，为留余量，这里超时设置为500ms    
    nbiot_at_cmd_param_init(nbiot_handle->p_nbiot_cmd, AT_NRB, NULL, CMD_EXCUTE, 500);

    //进入NBIOT_REBOOT状态
    nbiot_handle->p_sm_status->main_status = NBIOT_RESET;
    nbiot_handle->p_sm_status->sub_status  = NBIOT_SUB_RESET;

    nbiot_at_cmd_send(nbiot_handle, nbiot_handle->p_nbiot_cmd);
    
    return NBIOT_OK;
}

//使能/禁能自动入网
int nbiot_nconfig(nbiot_handle_t nbiot_handle, uint8_t auto_flag)
{
    char *p_auto_nconfig = NULL;
    
    if (nbiot_handle->p_sm_status->main_status != NBIOT_NONE)
    {
        return NBIOT_ERROR;
    }
        
    if (auto_flag == 0) 
    {
        
        p_auto_nconfig = "AUTOCONNECT,FALSE";
    }    
    else 
    {
        p_auto_nconfig = "AUTOCONNECT,TRUE"; 
    }
        
    //命令最大超时时间为300ms，为留余量，这里超时设置为500ms    
    nbiot_at_cmd_param_init(nbiot_handle->p_nbiot_cmd, AT_NCONFIG, p_auto_nconfig, CMD_SET, 500);

    //进入NBIOT_REBOOT状态
    nbiot_handle->p_sm_status->main_status = NBIOT_NCONFIG;
    nbiot_handle->p_sm_status->sub_status  = NBIOT_SUB_NCONFIG;

    nbiot_at_cmd_send(nbiot_handle, nbiot_handle->p_nbiot_cmd);
    
    return NBIOT_OK;
}



//获取NB模块的信息
int nbiot_info_get(nbiot_handle_t nbiot_handle)
{

    if (nbiot_handle->p_sm_status->main_status != NBIOT_NONE)
    {
        return NBIOT_ERROR;
    }
    
    nbiot_at_cmd_param_init(nbiot_handle->p_nbiot_cmd, AT_CEREG, NULL, CMD_READ, 3000);

    //进入NBIOT_INFO状态
    nbiot_handle->p_sm_status->main_status = NBIOT_INFO;
    nbiot_handle->p_sm_status->sub_status  = NBIOT_SUB_CEREG_QUERY;

    nbiot_at_cmd_send(nbiot_handle, nbiot_handle->p_nbiot_cmd);
    
    return NBIOT_OK;
}


//获取NB模块的信号质量
int nbiot_signal_get(nbiot_handle_t nbiot_handle)
{

    if (nbiot_handle->p_sm_status->main_status != NBIOT_NONE)
    {
        return NBIOT_ERROR;
    }
        
    nbiot_at_cmd_param_init(nbiot_handle->p_nbiot_cmd, AT_CSQ, NULL, CMD_EXCUTE, 3000);

    //进入NBIOT_SIGNAL状态
    nbiot_handle->p_sm_status->main_status = NBIOT_SIGNAL;
    nbiot_handle->p_sm_status->sub_status  = NBIOT_SUB_CSQ;

    nbiot_at_cmd_send(nbiot_handle, nbiot_handle->p_nbiot_cmd);
    
    return NBIOT_OK;
}

//创建tcpudp 
int nbiot_tcpudp_create(nbiot_handle_t nbiot_handle, nbiot_connect_type_t type)
{
    char *p_tcpudp = NULL;
  
    if (nbiot_handle->p_sm_status->main_status != NBIOT_NONE)
    {
        return NBIOT_ERROR;
    }
    
    if (type == NBIOT_TCP)
    {
        nbiot_handle->p_socket_info[0].socket_type = NBIOT_TCP;
        p_tcpudp = "1,1,1";
    }
    
    else if(type == NBIOT_UDP)
    {   
        nbiot_handle->p_socket_info[0].socket_type = NBIOT_UDP;
        p_tcpudp = "1,2,1";
    } 
    else 
    {
       return NBIOT_NOTSUPPORT;
      
    }
            
    nbiot_at_cmd_param_init(nbiot_handle->p_nbiot_cmd, AT_CSOC, p_tcpudp, CMD_SET, 3000);
    
    //进入创建TCP/UDP状态
    nbiot_handle->p_sm_status->main_status = NBIOT_TCPUDP_CR;
    nbiot_handle->p_sm_status->sub_status  = NBIOT_SUB_TCPUDP_CR;

    nbiot_at_cmd_send(nbiot_handle, nbiot_handle->p_nbiot_cmd);
    
    return NBIOT_OK;
}


//关闭tcpudp
int nbiot_tcpudp_close(nbiot_handle_t nbiot_handle, nbiot_connect_type_t type)
{

    if (nbiot_handle->p_sm_status->main_status != NBIOT_NONE)
    {
        return NBIOT_ERROR;
    }
    
       
    (void)type;
    
    //不能使用栈上的内存分配数据，要不然重发命令数据时会出错   
    memset(cmd_buf_temp, 0, sizeof(cmd_buf_temp));
       
    int16_t msg_len = snprintf(cmd_buf_temp,
                                sizeof(cmd_buf_temp),
                                "%d",
                                nbiot_handle->p_socket_info[0].socket_id);
                                
    if (msg_len < 0) {
      
        return NBIOT_ERROR;
    }
                                                   
    nbiot_at_cmd_param_init(nbiot_handle->p_nbiot_cmd, AT_CSOCL, cmd_buf_temp, CMD_SET, 3000);

    //进入tcp/udp关闭状态
    nbiot_handle->p_sm_status->main_status = NBIOT_TCPUDP_CL;
    nbiot_handle->p_sm_status->sub_status  = NBIOT_SUB_TCPUDP_CL;

    nbiot_at_cmd_send(nbiot_handle, nbiot_handle->p_nbiot_cmd);
        
    return NBIOT_OK;
}


//以hex数据格式发送数据,必须是偶数个长度
int nbiot_tcpudp_send_hex(nbiot_handle_t nbiot_handle, int len, char *msg, nbiot_connect_type_t type)
{
  
    if (nbiot_handle->p_sm_status->main_status != NBIOT_NONE)
    {
        return NBIOT_ERROR;
    }
  
    //判断SOCKET ID是否正确
    if (nbiot_handle->p_socket_info[0].socket_id  < 0 || nbiot_handle->p_socket_info[0].socket_id > 5 )
    {
        return NBIOT_ERROR;
    }
  
    //最大数据长度为有效数据加上头部
    int16_t str_len = (NBIOT_SEND_BUF_MAX_LEN - 20) ;

    //不能使用栈上的内存分配数据，要不然重发命令因栈上的内存数据释放掉时会出错   
    memset(cmd_buf_temp, 0, sizeof(cmd_buf_temp));


    uint16_t msg_len = snprintf(cmd_buf_temp,
                                str_len,
                                "%d,%d,",
                                 nbiot_handle->p_socket_info[0].socket_id, 
                                len);
                                
    for(uint16_t i = 0 ; i < len ; i++)
    {
        sprintf(&cmd_buf_temp[msg_len + (i << 1)],"%02X",(uint8_t)msg[i]);
    }                             
 
    //构建TCP/UDP数据发送命令，最大响应时间不详
    nbiot_at_cmd_param_init(nbiot_handle->p_nbiot_cmd, AT_CSOSEND, cmd_buf_temp, CMD_SET, 3000);
    
    //进入tcp/udp数据发送状态
    nbiot_handle->p_sm_status->main_status = NBIOT_TCPUDP_SEND;
    nbiot_handle->p_sm_status->sub_status  = NBIOT_SUB_TCPUDP_SEND;

    nbiot_at_cmd_send(nbiot_handle, nbiot_handle->p_nbiot_cmd);

    return NBIOT_OK;
}

//以字符串格式发送数据
int nbiot_tcpudp_send_str(nbiot_handle_t nbiot_handle, int len, char *msg, nbiot_connect_type_t type)
{
  
    if (nbiot_handle->p_sm_status->main_status != NBIOT_NONE)
    {
        return NBIOT_ERROR;
    }
  
    //判断SOCKET ID是否正确
    if (nbiot_handle->p_socket_info[0].socket_id  < 0 || nbiot_handle->p_socket_info[0].socket_id > 5 )
    {
        return NBIOT_ERROR;
    }
      

    //最大数据长度为有效数据加上头部
    int16_t str_len = (NBIOT_SEND_BUF_MAX_LEN - 20) ;

    //不能使用栈上的内存分配数据，要不然重发命令因栈上的内存数据释放掉时会出错   
    memset(cmd_buf_temp, 0, sizeof(cmd_buf_temp));
    

    int16_t msg_len = snprintf(cmd_buf_temp,
                               str_len,
                               "%d,%d,%s%s%s",
                                nbiot_handle->p_socket_info[0].socket_id,
                                0,
                                "\"",                                  
                                msg,
                                "\"");
    
    if (msg_len < 0) {
      
        return NBIOT_ERROR;
    }
    
                                                          
    //构建TCP/UDP数据发送命令，最大响应时间不详
    nbiot_at_cmd_param_init(nbiot_handle->p_nbiot_cmd, AT_CSOSEND, cmd_buf_temp, CMD_SET, 3000);
    
    //进入tcp/udp数据发送状态
    nbiot_handle->p_sm_status->main_status = NBIOT_TCPUDP_SEND;
    nbiot_handle->p_sm_status->sub_status  = NBIOT_SUB_TCPUDP_SEND;

    nbiot_at_cmd_send(nbiot_handle, nbiot_handle->p_nbiot_cmd);
    
    return NBIOT_OK;
}   

//创建coap服务器 
int nbiot_coap_server_create(nbiot_handle_t nbiot_handle, nbiot_connect_type_t type)
{  
    if (nbiot_handle->p_sm_status->main_status != NBIOT_NONE)
    {
        return NBIOT_ERROR;
    }
    
    if (type != NBIOT_COAP)
    {
        return NBIOT_NOTSUPPORT;
    }
    
    nbiot_handle->p_connect_status->connect_type = NBIOT_COAP;  
    nbiot_handle->p_connect_status->cid          = 1;
    nbiot_handle->p_connect_status->connect_id   = 1;
    
    
    //不能使用栈上的内存分配数据，要不然重发命令因栈上的内存数据释放掉时会出错   
    memset(cmd_buf_temp, 0, sizeof(cmd_buf_temp));    
               
                       
    //这个函数的返回值为想要   格式化写入的长度，并会在字符串结束后面自动加入结束字符
    uint16_t coap_cn_len = snprintf(cmd_buf_temp,
                                    sizeof(cmd_buf_temp) -1,"%s,%s,%d",                                                                                             
                                    REMOTE_SERVER_IP,
                                    REMOTE_COAP_PORT,
                                    nbiot_handle->p_connect_status->cid);
                                        
                     
            
    //最大响应时间不详                                              
    nbiot_at_cmd_param_init(nbiot_handle->p_nbiot_cmd, AT_CCOAPSTA, cmd_buf_temp, CMD_SET, 3000);
    nbiot_handle->p_nbiot_cmd->cmd_action = ACTION_OK_AND_NEXT | ACTION_ERROR_BUT_NEXT;                                
    
    //进入设置COAP服务器状态
    nbiot_handle->p_sm_status->main_status = NBIOT_CoAP_SEVER;
    nbiot_handle->p_sm_status->sub_status  = NBIOT_SUB_CoAP_SEVER;

    nbiot_at_cmd_send(nbiot_handle, nbiot_handle->p_nbiot_cmd);
    
    return NBIOT_OK;
}


//创建coap客户端
int nbiot_coap_client_create(nbiot_handle_t nbiot_handle, nbiot_connect_type_t type)
{
     
    if (nbiot_handle->p_sm_status->main_status != NBIOT_NONE)
    {
        return NBIOT_ERROR;
    }
    
    if (type != NBIOT_COAP)
    {
        return NBIOT_NOTSUPPORT;
    }
    
    nbiot_handle->p_connect_status->connect_type = NBIOT_COAP;
    nbiot_handle->p_connect_status->cid          = 1;  

    //不能使用栈上的内存分配数据，要不然重发命令因栈上的内存数据释放掉时会出错   
    memset(cmd_buf_temp, 0, sizeof(cmd_buf_temp));       
                
    //这个函数的返回值为想要格式化写入的长度，并会在字符串结束后面自动加入结束字符
    uint16_t coap_cn_len = snprintf(cmd_buf_temp,
                                    sizeof(cmd_buf_temp) -1,"%s,%s,%d",                                                                                             
                                    REMOTE_SERVER_IP,
                                    REMOTE_COAP_PORT,
                                    nbiot_handle->p_connect_status->cid);
                                                                       
    //最大响应时间不详                                              
    nbiot_at_cmd_param_init(nbiot_handle->p_nbiot_cmd, AT_CCOAPNEW, cmd_buf_temp, CMD_SET, 3000);
    nbiot_handle->p_nbiot_cmd->cmd_action = ACTION_OK_AND_NEXT | ACTION_ERROR_BUT_NEXT; 
                                       
    //进入创建客户端状态
    nbiot_handle->p_sm_status->main_status = NBIOT_CoAP_CLIENT;
    nbiot_handle->p_sm_status->sub_status  = NBIOT_SUB_CoAP_CLIENT;

    nbiot_at_cmd_send(nbiot_handle, nbiot_handle->p_nbiot_cmd);
    
    return NBIOT_OK;
}


//关闭coap
int nbiot_coap_close(nbiot_handle_t nbiot_handle, nbiot_connect_type_t type)
{
  
    if (nbiot_handle->p_sm_status->main_status != NBIOT_NONE)
    {
        return NBIOT_ERROR;
    }
           
    if (type != NBIOT_COAP)
    {
        return NBIOT_NOTSUPPORT;
    }
    
    nbiot_handle->p_connect_status->connect_type = NBIOT_COAP;  

    //通过客户端id来销毁COAP链接
    cmd_buf_temp[0] = nbiot_handle->p_connect_status->connect_id + '0'; 
    cmd_buf_temp[1] = 0;
    cmd_buf_temp[2] = 0;     
                                                                                   
    nbiot_at_cmd_param_init(nbiot_handle->p_nbiot_cmd, AT_CCOAPDEL, cmd_buf_temp, CMD_SET, 3000);

    //进入coap关闭状态,最大响应时间不详
    nbiot_handle->p_sm_status->main_status = NBIOT_CoAP_CL;
    nbiot_handle->p_sm_status->sub_status  = NBIOT_SUB_CoAP_CL;

    nbiot_at_cmd_send(nbiot_handle, nbiot_handle->p_nbiot_cmd);
    
    
    return NBIOT_OK;
}


//以hex数据格式发送coap协议数据,必须是偶数个长度
int nbiot_coap_send_hex(nbiot_handle_t nbiot_handle, int len, char *msg, nbiot_connect_type_t type)
{ 
    if (nbiot_handle->p_sm_status->main_status != NBIOT_NONE)
    {
        return NBIOT_ERROR;
    }
  
    //判断coap id是否正确
    if (nbiot_handle->p_connect_status->connect_id  < 0)
    {
        return NBIOT_ERROR;
    }
    
    if (type != NBIOT_COAP)
    {
        return NBIOT_NOTSUPPORT;
    }    
  
    //最大数据长度为有效数据加上头部
    int16_t str_len = (NBIOT_SEND_BUF_MAX_LEN - 24) ;

    //不能使用栈上的内存分配数据，要不然重发命令因栈上的内存数据释放掉时会出错   
    memset(cmd_buf_temp, 0, sizeof(cmd_buf_temp));    


    uint16_t msg_len = snprintf(cmd_buf_temp,
                                str_len,
                                "%d,%d,",
                                 nbiot_handle->p_connect_status->connect_id, 
                                len);
    
    cmd_buf_temp[msg_len] = '\"';
    
    
    
    for(uint16_t i = 0 ; i < len ; i++)
    {
        //表示一个字符用十六进制表示 
        sprintf(&cmd_buf_temp[msg_len + 1 + (i << 1)],"%02X",(uint8_t)msg[i]);
    }

    cmd_buf_temp[msg_len + 1 + (len << 1)] = '\"';    
 
    //构建coap数据发送命令，最大响应时间不详
    nbiot_at_cmd_param_init(nbiot_handle->p_nbiot_cmd, AT_CCOAPSEND, cmd_buf_temp, CMD_SET, 3000);
    
    //进入coap数据发送状态
    nbiot_handle->p_sm_status->main_status = NBIOT_CoAP_SEND;
    nbiot_handle->p_sm_status->sub_status  = NBIOT_SUB_CoAP_SEND;

    nbiot_at_cmd_send(nbiot_handle, nbiot_handle->p_nbiot_cmd);

    return NBIOT_OK;
}

//以字符串格式发送数据coap协议数据
int nbiot_coap_send_str(nbiot_handle_t nbiot_handle, int len, char *msg, nbiot_connect_type_t type)
{ 
    if (nbiot_handle->p_sm_status->main_status != NBIOT_NONE)
    {
        return NBIOT_ERROR;
    }
  
    //判断coap id是否正确
    if (nbiot_handle->p_connect_status->connect_id  < 0)
    {
        return NBIOT_ERROR;
    }
    
    if (type != NBIOT_COAP)
    {
        return NBIOT_NOTSUPPORT;
    } 
      
    //最大数据长度为有效数据加上头部
    int16_t str_len = (NBIOT_SEND_BUF_MAX_LEN - 24) ;


    //不能使用栈上的内存分配数据，要不然重发命令因栈上的内存数据释放掉时会出错   
    memset(cmd_buf_temp, 0, sizeof(cmd_buf_temp));   

    int16_t msg_len = snprintf(cmd_buf_temp,
                               str_len,
                               "%d,%d,%s%s%s",
                                nbiot_handle->p_connect_status->connect_id ,
                                len,
                                "\"",                                  
                                msg,
                                "\"");
    
    if (msg_len < 0) {
      
        return NBIOT_ERROR;
    }
    
                                                          
    //构建coap数据发送命令，最大响应时间不详
    nbiot_at_cmd_param_init(nbiot_handle->p_nbiot_cmd, AT_CCOAPSEND, cmd_buf_temp, CMD_SET, 3000);
    
    //进入coap数据发送状态
    nbiot_handle->p_sm_status->main_status = NBIOT_CoAP_SEND;
    nbiot_handle->p_sm_status->sub_status  = NBIOT_SUB_CoAP_SEND;

    nbiot_at_cmd_send(nbiot_handle, nbiot_handle->p_nbiot_cmd);
         
    return NBIOT_OK;
}

//更新cdp服务器
int nbiot_ncdp_update(nbiot_handle_t nbiot_handle, nbiot_connect_type_t type)
{     
    if (nbiot_handle->p_sm_status->main_status != NBIOT_NONE)
    {
        return NBIOT_ERROR;
    }
    
    if (type != NBIOT_NCDP)
    {
        return NBIOT_NOTSUPPORT;
    }
    
    nbiot_handle->p_connect_status->connect_type = NBIOT_NCDP;

    //不能使用栈上的内存分配数据，要不然重发命令因栈上的内存数据释放掉时会出错   
    memset(cmd_buf_temp, 0, sizeof(cmd_buf_temp));       
                
    //这个函数的返回值为想要格式化写入的长度，并会在字符串结束后面自动加入结束字符
    uint16_t ncdp_cn_len = snprintf(cmd_buf_temp,
                                    sizeof(cmd_buf_temp) -1,"%s,%s",                                                                                             
                                    REMOTE_SERVER_IP,
                                    REMOTE_COAP_PORT);
                                                                       
    //最大响应时间为300ms。留余量这里保持为500ms                                           
    nbiot_at_cmd_param_init(nbiot_handle->p_nbiot_cmd, AT_NCDP, cmd_buf_temp, CMD_SET, 500);
                                    
//    nbiot_handle->p_nbiot_cmd->cmd_action = ACTION_OK_AND_NEXT | ACTION_ERROR_BUT_NEXT; 
                                       
    //进入创建客户端状态
    nbiot_handle->p_sm_status->main_status = NBIOT_NCDP_SERVER;
    nbiot_handle->p_sm_status->sub_status  = NBIOT_SUB_NCDP_SERVER;

    nbiot_at_cmd_send(nbiot_handle, nbiot_handle->p_nbiot_cmd);
    
    return NBIOT_OK;
}


//关闭ncdp
int nbiot_ncdp_close(nbiot_handle_t nbiot_handle, nbiot_connect_type_t type)
{
  
    if (nbiot_handle->p_sm_status->main_status != NBIOT_NONE)
    {
        return NBIOT_ERROR;
    }
           
    if (type != NBIOT_NCDP)
    {
        return NBIOT_NOTSUPPORT;
    }   
                                                                                   
    nbiot_at_cmd_param_init(nbiot_handle->p_nbiot_cmd, AT_QLWSREGIND, "1", CMD_SET, 500);  

    //进入ncdp关闭状态,最大响应时间不详
    nbiot_handle->p_sm_status->main_status = NBIOT_NCDP_CL;
    nbiot_handle->p_sm_status->sub_status  = NBIOT_SUB_NCDP_CL;

    nbiot_at_cmd_send(nbiot_handle, nbiot_handle->p_nbiot_cmd);
        
    return NBIOT_OK;
}

//以hex数据格式发送ncdp协议数据,必须是偶数个长度
int nbiot_ncdp_send_hex(nbiot_handle_t nbiot_handle, int len, char *msg, nbiot_connect_type_t type, char *mode)
{ 
    
    int16_t msg_len = 0;
    
    char *p_at_cmd = NULL;
    
    if (nbiot_handle->p_sm_status->main_status != NBIOT_NONE)
    {
        return NBIOT_ERROR;
    }
  
    //当类型不为NCDP时，或都发送的数据个数不为偶数个时
    if ((type != NBIOT_NCDP) || ((strlen(msg) % 2) != 0))
    {
        return NBIOT_NOTSUPPORT;
    } 
      
    //最大数据长度为有效数据加上头部
    int16_t str_len = (NBIOT_SEND_BUF_MAX_LEN - 24) ;

    //不能使用栈上的内存分配数据，要不然重发命令因栈上的内存数据释放掉时会出错   
    memset(cmd_buf_temp, 0, sizeof(cmd_buf_temp));
    
    
    if (mode == NULL) 
    {

        msg_len = snprintf(cmd_buf_temp,
                               str_len,
                               "%d,%s",
                               (strlen(msg) / 2),                                                  
                                msg);
        
        p_at_cmd = AT_QLWULDATA;
    }
    
    else 
    {
        msg_len = snprintf(cmd_buf_temp,
                               str_len,
                               "%d,%s,%s",
                               (strlen(msg) / 2),                                                  
                                msg,
                                mode); 

        p_at_cmd = AT_QLWULDATAEX;        
    }
    
    if (msg_len < 0) 
    {
      
        return NBIOT_ERROR;
    }
    
                                                          
    //构建ncdp数据发送命令，最大响应时间不详
    nbiot_at_cmd_param_init(nbiot_handle->p_nbiot_cmd, p_at_cmd, cmd_buf_temp, CMD_SET, 3000);
    
    //进入ncdp数据发送状态
    nbiot_handle->p_sm_status->main_status = NBIOT_NCDP_SEND;
    nbiot_handle->p_sm_status->sub_status  = NBIOT_SUB_NCDP_SEND;

    nbiot_at_cmd_send(nbiot_handle, nbiot_handle->p_nbiot_cmd);
         
    return NBIOT_OK;
}


