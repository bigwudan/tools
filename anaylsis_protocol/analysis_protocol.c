#include <string.h>
#include "analysis_protocol.h"

//基本解析数据结构
static analysis_protocol_base protocol_base_g;

//头节点
static analysis_protocol_cmd_frame_head cmd_frame_head_g;

//缓存
recv_cache_data recv_cache_data_g;

//帧缓存
frame_cache frame_cache_g;

//从外设加入缓存 0 正常 1 满
uint8_t 
join_cache_data(struct analysis_protocol_base_tag *base, void *data, uint8_t len)
{
    //加入缓存
    recv_cache_data *p_cache = base->recv_cache_data_p;
    if(p_cache->tot_len < p_cache->use_len + len ){
        return 1;
    }
    memmove(p_cache->data + p_cache->use_len, data , len);
    return 0;
}

//分析协议数据,是否读到一个完整的帧 0未读到 1已经读到
uint8_t 
analy_frame_func(struct analysis_protocol_base_tag *base)
{
    return 0;
}

//发送命令然后写入命令队列
uint8_t
analysis_protocol_send_cmd( analysis_protocol_base *base, analysis_protocol_cmd_frame *cmd_frame   )
{



}


//轮训发送cmd 缓存 看是否需要从新发送
void
analysis_protocol_polling_cmd_frame(analysis_protocol_base *base)
{



}





//初始化
uint8_t
analysis_protocol_init()
{
    //全部初始化为0
    memset(&protocol_base_g, 0, sizeof(analysis_protocol_base));
    //默认头节点
    memset(&cmd_frame_head_g, 0, sizeof(analysis_protocol_cmd_frame_head));
    //初始化缓存
    memset(&recv_cache_data_g, 0, sizeof(recv_cache_data));
    recv_cache_data_g.tot_len = MAX_RECV_NUM;
    //帧缓存
    memset(&frame_cache_g, 0, sizeof(frame_cache));
    //最大缓存
    frame_cache_g.tot_len = MAX_RECV_NUM;
    protocol_base_g.frame_cache_p = &frame_cache_g;
    //加入头节点
    protocol_base_g.cmd_frame_head= &cmd_frame_head_g;
    //加入缓存
    protocol_base_g.recv_cache_data_p = &recv_cache_data_g;

    protocol_base_g.join_cache_data_func = join_cache_data;

    //是否读到一个网络帧
    protocol_base_g.analy_frame_func = analy_frame_func;

}






