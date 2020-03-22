
/*
1 调用analysis_protocol_write_chain_list 把串口来的数据写入环形缓存

2 自定义接受数据写入 缓存帧 get_recv_frame_bc ,从环形缓存中，
  取出数据，判断是否得到一个完整帧，并存入缓存recv_frame. 完整的帧 返回1 
  
  
3 process_recv_frame_bc 分析一个完整的帧， 1是否加入返回命令 2是否加入需要重复发送

5  check_reply_send_frame_bc  判断是否有重发的数据在链表里面

5 是否需要超时发送

4 run_send_frame_bc  发送数据 


*/


#ifndef _ANALYSIS_PROTOCOL_H_
#define _ANALYSIS_PROTOCOL_H_

#include <stdint.h>
#include <sys/time.h>
#include "analysis_protocol_tools.h"
#include "queue.h"
#include "heap.h"

#define FRAME_RECV_CACHE_MAX 50
#define FRAME_SEND_DATA 50

#define SELF_MALLOC(num) pvPortMalloc(num) 
#define SELF_FREE(p) vPortFree(p)

//前置声明
struct analysis_protocol_base_tag;

//对比类型 在取重中 state状态型 只比较状态
//DATA 数据性 ，状态和数据需要同时对比
enum compare_state{STATE, DATA  }; 

//发送的命令缓存
struct analysis_protocol_send_frame_list_tag
{
    uint8_t repeat_num; //重发次数
    uint8_t repeat_max; //最大重发次数
    uint8_t state; //自己定义状态
    uint8_t data[FRAME_SEND_DATA]; //保存的命令数据
    uint8_t data_len; //数据长度
    struct timeval last_send_time;//最后发送的时间
    uint8_t repeat_during ; //重发时间间隔
    uint8_t value;//值，就是需要改变的值
    TAILQ_ENTRY(analysis_protocol_send_frame_list_tag)  next; //下一个节点  

};

//发送的命令的缓缓列表
struct analysis_protocol_send_frame_to_dest_tag
{
    uint8_t data[FRAME_SEND_DATA]; //保存的命令
    uint8_t data_len; //命令长度
    uint8_t state; //类型
    uint8_t value; //值
    TAILQ_ENTRY(analysis_protocol_send_frame_to_dest_tag)  next; //下一个节点  
};




//普通返回函数
typedef uint8_t (*base_callback)(struct analysis_protocol_base_tag *, void *);




struct yingxue_frame_tag{
    uint8_t data[17];
    uint8_t len;

};

struct wifi_frame_tag{
    uint8_t wifi_1;
    uint8_t wifi_2;


};


enum analysis_protocol_state_tag{ YINGXUE, WIFI  };


//解析数据基础结构
struct analysis_protocol_base_tag
{
    //缓存当前时间
    struct timeval curr_cache_time;
    
    //环形链表
    struct chain_list_tag *chain_list;
    

	//从环形缓存中取出数据，判断是否得到一个完整帧，然后填充recv_frame. 完整的帧返回1 ， 未完整返回0
	base_callback get_recv_frame_bc; 
	//用户从recv_frame中，调用自己的逻辑分析。 1 下发命令 2 下发的命令，是需要得到回复
	base_callback process_recv_frame_bc;
	//收到的帧里面，是否是回复指令,是，就从重复列表中移除。
	base_callback check_reply_send_frame_bc;
	//发送列表中发送数据
	base_callback run_send_frame_bc;
    

    //发送命令缓存头
    TAILQ_HEAD(frame_send_head_tag, analysis_protocol_send_frame_list_tag)   send_frame_head;
    
    //发送命令到中断缓存
    TAILQ_HEAD(frame_send_dest_head_tag, analysis_protocol_send_frame_to_dest_tag)   send_frame_dest_head;
    
    //收到帧指针
    void *recv_frame;
    //帧类型
    enum analysis_protocol_state_tag state; 
    

}; 


//写入的数据，返回已经写入字节数
uint8_t analysis_protocol_write_chain_list(struct chain_list_tag *chain_list, uint8_t *src, uint8_t len );


//读出的数据，返回已经写入字节数
uint8_t analysis_protocol_read_chain_list(struct chain_list_tag *chain_list, uint8_t *src, uint8_t len );


//初始化
struct analysis_protocol_base_tag * 
analysis_protocol_init( 
						void *arg, 
						base_callback get_recv_frame_bc, 
						base_callback process_recv_frame_bc, 
						base_callback run_send_frame_bc, 
						base_callback check_reply_send_frame_bc );




//超时重发,对超过时间没有确认的数据，在一次重发
void analysis_protocol_overtime_send(struct analysis_protocol_base_tag *base );

//在收到的数据中，对比重复发送的队列，如果有重复就删除
//重复发送的头队列
//状态
//数据
//对比类型  是否需要对比数据
void
analysis_protocol_compare_recv_repeat(struct frame_send_head_tag *head, int state, int data,enum compare_state cmp_state );

//插入发送队列，并且去重,或者更新
int
analysis_protocol_insert_send_list(struct analysis_protocol_base_tag *base, int state, int data, enum compare_state cmp_state, uint8_t *src, uint8_t len, int head_tail );
//发送命令需要，在重复列表中,如果存在，且是DATA型需要更新数据，如果没有插入数据
void
analysis_protocol_recv_repeat_up(struct analysis_protocol_base_tag *base, int state, int data, enum compare_state cmp_state, uint8_t *src, uint8_t len );
#endif
