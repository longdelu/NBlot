
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

/**
  * @brief  设置nbiot模块初始化网络注册
  * @param  nbiot_handle   : 指向nbiot设备句柄的指针.
  * @retval NBIOT_OK 设置初始化成功  NBIOT_ERROR 设置初始化失败
  */
int nbiot_reg_init (nbiot_handle_t nbiot_handle)
{    
    if (nbiot_handle->p_sm_status->main_status != NBIOT_NONE)
    {
        return NBIOT_ERROR;
    }

    //填充指令的描述结构体
    nbiot_at_cmd_param_init(nbiot_handle->p_nbiot_cmd, AT_SYNC, NULL, CMD_EXCUTE, 3000);

    //进入NBIOT_INIT状态
    nbiot_handle->p_sm_status->main_status = NBIOT_INIT;
    nbiot_handle->p_sm_status->sub_status  = NBIOT_SUB_SYNC;

    //发送指令
    nbiot_at_cmd_send(nbiot_handle, nbiot_handle->p_nbiot_cmd);
  
    return NBIOT_OK;
}


/**
  * @brief 设置重启nbiot模块
  * @param  nbiot_handle   : 指向nbiot设备句柄的指针.
  * @retval NBIOT_OK 设置重启成功  NBIOT_ERROR 设置重启失败
  */
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

/**
  * @brief 使能/禁能nbiot模块自动入网
  * @param  nbiot_handle  : 指向nbiot设备句柄的指针.
  * @param  auto_flag     : 0 禁能自动入网 1 使能自动入网  
  * @retval NBIOT_OK 设置成功  NBIOT_ERROR 设置失败
  */
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
         
    //填充指令的描述结构体 
    //命令最大超时时间为300ms，为留余量，这里超时设置为500ms      
    nbiot_at_cmd_param_init(nbiot_handle->p_nbiot_cmd, AT_NCONFIG, p_auto_nconfig, CMD_SET, 500);

    //进入NBIOT_REBOOT状态
    nbiot_handle->p_sm_status->main_status = NBIOT_NCONFIG;
    nbiot_handle->p_sm_status->sub_status  = NBIOT_SUB_NCONFIG;

    nbiot_at_cmd_send(nbiot_handle, nbiot_handle->p_nbiot_cmd);
    
    return NBIOT_OK;
}



/**
  * @brief 获取nbiot模块的信息
  * @param  nbiot_handle  : 指向nbiot设备句柄的指针.
  * @retval NBIOT_OK 设置成功  NBIOT_ERROR 设置失败
  */
int nbiot_info_get(nbiot_handle_t nbiot_handle)
{

    if (nbiot_handle->p_sm_status->main_status != NBIOT_NONE)
    {
        return NBIOT_ERROR;
    }
    
    //填充指令的描述结构体
    nbiot_at_cmd_param_init(nbiot_handle->p_nbiot_cmd, AT_CEREG, NULL, CMD_READ, 3000);

    //进入NBIOT_INFO状态
    nbiot_handle->p_sm_status->main_status = NBIOT_INFO;
    nbiot_handle->p_sm_status->sub_status  = NBIOT_SUB_CEREG_QUERY;

    //发送指令
    nbiot_at_cmd_send(nbiot_handle, nbiot_handle->p_nbiot_cmd);
    
    return NBIOT_OK;
}


/**
  * @brief 获取nbiot模块的信号质量
  * @param  nbiot_handle  : 指向nbiot设备句柄的指针.
  * @retval NBIOT_OK 设置成功  NBIOT_ERROR 设置失败
  */
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

/**
  * @brief 更新ncdp服务器
  * @param  nbiot_handle  : 指向nbiot设备句柄的指针.
  * @param  type          : 连接类型，值仅为枚举当中的值
  * @retval NBIOT_OK 设置成功  NBIOT_ERROR 设置失败  
  */
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
                                       
    //进入更新CDP服务器
    nbiot_handle->p_sm_status->main_status = NBIOT_NCDP_SERVER;
    nbiot_handle->p_sm_status->sub_status  = NBIOT_SUB_NCDP_SERVER;

    //发送指令
    nbiot_at_cmd_send(nbiot_handle, nbiot_handle->p_nbiot_cmd);
    
    return NBIOT_OK;
}


/**
  * @brief 关闭ncdp
  * @param  nbiot_handle  : 指向nbiot设备句柄的指针.
  * @param  type          : 连接类型，值仅为枚举当中的值
  * @retval NBIOT_OK 设置成功  NBIOT_ERROR 设置失败  
  */
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

/**
  * @brief 查询活动状态
  * @param  nbiot_handle  : 指向nbiot设备句柄的指针.
  * @retval NBIOT_OK 设置成功  NBIOT_ERROR 设置失败  
  */
int nbiot_cscon_query (nbiot_handle_t nbiot_handle)
{
  
    if (nbiot_handle->p_sm_status->main_status != NBIOT_NONE)
    {
        return NBIOT_ERROR;
    }
                                                                                              
    nbiot_at_cmd_param_init(nbiot_handle->p_nbiot_cmd, AT_CSCON, NULL, CMD_READ, 500);  

    nbiot_handle->p_sm_status->main_status = NBIOT_CSCON;
    nbiot_handle->p_sm_status->sub_status  = NBIOT_SUB_CSCON_QUERY;

    nbiot_at_cmd_send(nbiot_handle, nbiot_handle->p_nbiot_cmd);
        
    return NBIOT_OK;
}


/**
  * @brief 以hex格式字符串发送业务逻辑数据,数据有效长度必须是偶数个大小且不能包含特殊字符
  * @param  nbiot_handle  : 指向nbiot设备句柄的指针.
  * @param  type          : 连接类型，值仅为枚举当中的值
  * @param  mode          : 一般设为为NULL即可
  * @retval NBIOT_OK 设置成功  NBIOT_ERROR 设置失败  NBIOT_NOTSUPPORT 数据有效长度不为偶数个长度 
  */
int nbiot_ncdp_send_hexstr (nbiot_handle_t nbiot_handle, 
                            int len, 
                            char *msg, 
                            nbiot_connect_type_t type, 
                            char *mode)
{ 
    
    int16_t msg_len = 0;
    char buf_head[3] = {0};     
    char buf_head_tmp[5] = {0};    
    
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
    
    buf_head[1] =  (strlen(msg) / 2) % 256;
    buf_head[0] =  (strlen(msg) / 2) / 256;
    
    //转换数据格式                     
    nbiot_srcbuf2hexchrbuf(buf_head, buf_head_tmp, 2); 
    
           
    if (mode == NULL) 
    {

        msg_len = snprintf(cmd_buf_temp,
                               str_len,
                               "%d,%s%s",
                               (strlen(msg) / 2 + 2),
                                buf_head_tmp,        
                                msg);
        
        p_at_cmd = AT_QLWULDATA;
    }
    
    else 
    {
        msg_len = snprintf(cmd_buf_temp,
                               str_len,
                               "%d,%s%s,%s",
                               (strlen(msg) / 2 + 2), 
                                buf_head_tmp,
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

    //发送指令
    nbiot_at_cmd_send(nbiot_handle, nbiot_handle->p_nbiot_cmd);
         
    return NBIOT_OK;
}

/**
  * @brief 将1个字符转换为10进制数字
  * @param  nbiot_handle  : 指向nbiot设备句柄的指针.
  * @param  chr           : 字符,0~9/A~F/a~F
  * @retval chr对应的10进制数值  
  */
u8 nbiot_chr2hex (u8 chr)
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

/**
  * @brief 将1个16进制数字转换为字符
  * @param  nbiot_handle  : 指向nbiot设备句柄的指针.
  * @param  hex           : 十六进制数
  * @retval 返回十六进制数对应的字符  
  */
u8 nbiot_hex2chr (u8 hex)
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
      return '0';
    }              
}



/**
  * @brief  缓冲区当中每两个字节重新组成一个一字节十六进制数(字符).
  * @param  p_buf     : 源数据缓冲区首地址.
  * @param  len       : 源缓冲区长度.
  * @note  调用该接口会改变源缓冲区的长度
  * @retval None
  */
void nbiot_buf2hex (char *p_buf , int len)
{
    int i = 0; 
   
    char tmp  = 0;
    char tmp1 = 0;
     
    for (i = 0; i < len; i=i+2)
    {
        tmp = nbiot_chr2hex(p_buf[i]);       
        tmp1 = nbiot_chr2hex(p_buf[i + 1]);      
        p_buf[i / 2] = (tmp << 4)  |  tmp1;                    
    } 
    
    p_buf[i / 2] = 0;      
}


/**
  * @brief  缓冲区每1个字符重新组成为两个字符来显示
  * @eg     例如字符'1'为十六进制0x31,则变成两个字符"31"来表示字符'1'
  * @param  src_buf   : 源数据缓冲区首地址.
  * @param  dest_buf  : 目标数据缓冲区首地址.
  * @param  len       : 源缓冲区长度.
  * @note   确保目的缓冲区长度足够容纳源缓冲区的数据,其长度至少为源缓冲区有效数据长度的2倍
  * @retval None
  */
void nbiot_srcbuf2hexchrbuf (char *src_buf ,char *dest_buf, int len)
{
    int i = 0; 
   
    char tmp  = 0;
    char tmp1 = 0;
     
    for (i = 0; i < len; i = i+1)
    {
        tmp = ((src_buf[i] >> 4) & 0x0f);      
        tmp1 = ((src_buf[i]) & 0x0f);         
        dest_buf[2 * i] = nbiot_hex2chr(tmp); 
        dest_buf[2 * i + 1] = nbiot_hex2chr(tmp1);        
    }
    
    dest_buf[2 * i + 2] = 0;      
}


/**
  * @brief  缓冲区当中每两个字节重新组成一个一字节十六进制数(字符).
  * @param  src_buf   : 源数据缓冲区首地址.
  * @param  dest_buf  : 目标数据缓冲区首地址.
  * @param  len       : 源缓冲区长度.
  * @note   确保目的缓冲区长度足够容纳源缓冲区的数据,其长度至少为源缓冲区有效数据长度的一半
  * @retval None
  */
void nbiot_srcbuf2hexbuf (char *src_buf ,char *dest_buf, int len)
{
    int i = 0; 
   
    char tmp  = 0;
    char tmp1 = 0;
    
    for (i = 0; i < len; i=i+2)
    {
        tmp = nbiot_chr2hex(src_buf[i]);       
        tmp1 = nbiot_chr2hex(src_buf[i + 1]);  
       
        dest_buf[i / 2] = (tmp << 4)  |  tmp1;                    
    }     
        
    dest_buf[i / 2] = 0;      
}


