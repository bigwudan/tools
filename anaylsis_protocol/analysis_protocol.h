#ifndef _ANALYSIS_PROTOCOL_H_
#define _ANALYSIS_PROTOCOL_H_

#include <stdint.h>
#include <sys/time.h>

#define FRAME_DATA_NUM 100
#define MAX_RECV_NUM 20

//前置声明
struct analysis_protocol_cmd_frame_tag;
//前置声明
struct analysis_protocol_base_tag; 

//回调函数
typedef void (*analysis_protocol_bc)(struct analysis_protocol_base_tag * , void *);

//接受到的数据缓存
typedef struct recv_cache_data_tag{
    uint8_t data[MAX_RECV_NUM];//缓存数据
    uint8_t use_len; //已经使用的长度
    uint8_t data_len; //数据长度
    uint8_t tot_len; //整个长度


}recv_cache_data;

//帧缓存
typedef struct frame_cache_tag{
    uint8_t data[MAX_RECV_NUM];//缓存数据
    uint8_t use_len;//已经使用的数据
    uint8_t tot_len;//整个长度


}frame_cache;


//解析数据基础结构
typedef struct analysis_protocol_base_tag
{
    //从外设接受的数据加入缓存
    uint8_t (*join_cache_data_func)(struct analysis_protocol_base_tag *base, void *data, uint8_t len);
    //分析协议数据,是否读到一个完整的帧 0 正常 1 一个完整的
    uint8_t (*analy_frame_func)(struct analysis_protocol_base_tag *base);
    //发送命令,这里是写入队列
    void (*join_cmd_frame_list)(struct analysis_protocol_base_tag *base , struct analysis_protocol_cmd_frame_tag *data  );
    //发送命令,从链表中发送
    uint8_t (*send_cmd_frame)(struct analysis_protocol_base_tag *base);

    //发送命令链表头
    struct analysis_protocol_cmd_frame_head_tag *cmd_frame_head;
    
    //接受缓存结构
    recv_cache_data *recv_cache_data_p;

    //帧缓存
    frame_cache *frame_cache_p;

    
    

} analysis_protocol_base;


//帧的数据
typedef struct analysis_protocol_cmd_frame_tag
{
   //发送的数据 
   uint8_t frame_data[FRAME_DATA_NUM];
   //命令
   uint8_t cmd;
   //最后一次发送的时间戳
   struct timeval last_time;
   //延迟多少S重发
   uint8_t repeat_s;
   //重试次数
   uint8_t repeat_num;
    
   //链表前一个
   struct analysis_protocol_frame *pre_frame;
   
   //链表后一个
   struct analysis_protocol_frame *next_fame;



} analysis_protocol_cmd_frame;


//帧的头部
typedef struct analysis_protocol_cmd_frame_head_tag
{
    //多少帧
    uint8_t frame_tot;
    //第一个元素的地址
    analysis_protocol_cmd_frame *frame_first;



} analysis_protocol_cmd_frame_head;





#endif
