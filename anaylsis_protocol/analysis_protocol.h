#ifndef _ANALYSIS_PROTOCOL_H_
#define _ANALYSIS_PROTOCOL_H_

#include <stdint.h>
#include <sys/time.h>
#include "analysis_protocol_tools.h"
#include "queue.h"

#define FRAME_RECV_CACHE_MAX 50
#define FRAME_SEND_DATA 50

//前置声明
struct analysis_protocol_base_tag;

//接受到数据的缓存
struct analysis_protocol_frame_recv_cache
{
    uint8_t data[FRAME_RECV_CACHE_MAX]; //缓存数据
    uint8_t tot_len; //总数据的长度
    uint8_t use_len; //已经使用的长度

};


//发送的命令缓存
struct analysis_protocol_send_frame_list_tag
{
    uint8_t repeat_num; //重发次数
    uint8_t repeat_max; //最大重发次数
    uint8_t state; //自己定义状态
    uint8_t data[FRAME_SEND_DATA]; //保存的命令数据
    struct timeval last_send_time;//最后发送的时间
    uint8_t repeat_during ; //重发时间间隔
    TAILQ_ENTRY(analysis_protocol_send_frame_list_tag)  next; //下一个节点  

};


//分析数据回调函数
typedef uint8_t (*frame_recv_fun_tag)(struct analysis_protocol_base_tag *); 

//用户自己解析，逻辑运算然后写入等待列表
typedef uint8_t (*self_process_frame_tag)(struct analysis_protocol_base_tag *, void *);




//解析数据基础结构
struct analysis_protocol_base_tag
{
    //缓存当前时间
    struct timeval curr_cache_time;
    
    //环形链表
    struct chain_list_tag *chain_list;
    
    //帧的数据缓存
    struct analysis_protocol_frame_recv_cache frame_recv_cache;
    
    //回调函数，解析接受到数据，并且存入帧，返回0正在等待，返回1得到一个完整帧
    frame_recv_fun_tag frame_recv_fun;
    
    //用户自己解析，并且判断是否需要写入等待回复，写入缓存列表
    self_process_frame_tag self_process_frame; 

    //发送命令缓存头
    TAILQ_HEAD(frame_send_head_tag, analysis_protocol_send_frame_list_tag)   head;

    

}; 


//写入的数据，返回已经写入字节数
uint8_t analysis_protocol_write_chain_list(struct chain_list_tag *chain_list, uint8_t *src, uint8_t len );


//读出的数据，返回已经写入字节数
uint8_t analysis_protocol_read_chain_list(struct chain_list_tag *chain_list, uint8_t *src, uint8_t len );






#endif
