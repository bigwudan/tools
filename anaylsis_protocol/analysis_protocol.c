#include <string.h>
#include <stdlib.h>
#include "analysis_protocol.h"
#include "analysis_protocol_tools.h"



//初始化
/*
//从环形缓存中取出数据，判断是否得到一个完整帧，然后填充recv_frame. 完整的帧返回1 ， 未完整返回0
base_callback get_recv_frame_bc; 
//用户从recv_frame中，调用自己的逻辑分析。 1 下发命令 2 下发的命令，是需要得到回复
base_callback process_recv_frame_bc;
//收到的帧里面，是否是回复指令,是，就从重复列表中移除。
base_callback check_reply_send_frame_bc;
//发送列表中发送数据
base_callback run_send_frame_bc
*/
struct analysis_protocol_base_tag * 
analysis_protocol_init(                                                                                                                                               
					   void *arg,                                             
					   base_callback get_recv_frame_bc, 
					   base_callback process_recv_frame_bc,                   
					   base_callback run_send_frame_bc, 
					   base_callback check_reply_send_frame_bc )      
{
    struct analysis_protocol_base_tag *base = malloc(sizeof(struct analysis_protocol_base_tag));

    //初始化
    memset(base, 0, sizeof(struct analysis_protocol_base_tag));    
    //初始化樱雪
    if((uint8_t)arg == 0){
        base->recv_frame =  malloc( sizeof(struct yingxue_frame_tag));
        base->state = YINGXUE;

    }else{
        base->recv_frame = malloc(sizeof(struct wifi_frame_tag));
        base->state = WIFI;
    }
    
    base->chain_list= malloc(sizeof(struct chain_list_tag)); 


    //初始化链表
    create_chain_list(base->chain_list);
    //赋值给链表
    //初始化发送命令
    TAILQ_INIT(&base->send_frame_head);  
    TAILQ_INIT(&base->send_frame_dest_head);  
	base->get_recv_frame_bc = get_recv_frame_bc;
	base->process_recv_frame_bc = process_recv_frame_bc;                   
	base->run_send_frame_bc = run_send_frame_bc; 
	base->check_reply_send_frame_bc = check_reply_send_frame_bc;      

    return base;
}


//写入的数据，返回已经写入字节数
uint8_t
analysis_protocol_write_chain_list(struct chain_list_tag *chain_list, uint8_t *src, uint8_t len )
{
    uint8_t send_len = 0;
    uint8_t flag = 0;
    for(uint8_t i=0; i< len; i++){
        //判断是否满
        flag = is_full_chain_list(chain_list);
        if(flag != 0){
            in_chain_list(chain_list, src[i] ); 
            send_len++;
        //已经写满    
        }else{
            break;
        }
    }

    return send_len;

}

//读出的数据，返回已经写入字节数
uint8_t
analysis_protocol_read_chain_list(struct chain_list_tag *chain_list, uint8_t *src, uint8_t len )
{
    uint8_t send_len = 0;
    uint8_t flag = 0;

    for(int i=0; i< len; i++){
        //是否有数据
        flag = is_empty_chain_list(chain_list);
        if( flag !=0 ){
            out_chain_list(chain_list, (src[i]));
            send_len++;
        }else{
            break;
        }
    }
    return send_len;
}

//超时重发,对超过时间没有确认的数据，在一次重发
void
analysis_protocol_overtime_send(struct analysis_protocol_base_tag *base )
{
    //便利整个缓存命令链表
    struct analysis_protocol_send_frame_list_tag *send_frame;
    struct analysis_protocol_send_frame_list_tag *tmp_send_frame;
    struct analysis_protocol_send_frame_to_dest_tag *tmp_frame_dest;
    //获得一个元素
    send_frame = TAILQ_FIRST(&base->send_frame_head);
    while(send_frame){
        tmp_send_frame = send_frame;
        send_frame = TAILQ_NEXT(send_frame, next);
        //判断是否超过重发次
        if(tmp_send_frame->repeat_num > tmp_send_frame->repeat_max){
            TAILQ_REMOVE(&base->send_frame_head, tmp_send_frame, next);   
            SELF_FREE(tmp_send_frame);
        }else{
            //是否到达超时时间
            if(base->curr_cache_time.tv_sec >= (tmp_send_frame->last_send_time.tv_sec + tmp_send_frame->repeat_during ) ){
                tmp_send_frame->repeat_num++;
                //加入重发 
                analysis_protocol_insert_send_list(base, tmp_send_frame->state, tmp_send_frame->value, DATA, tmp_send_frame->data, tmp_send_frame->data_len, 1);

            }
        }
    }    
    return ;
}

//在收到的数据中，对比重复发送的队列，如果有重复就删除
//重复发送的头队列
//状态
//数据
//对比类型  是否需要对比数据
void
analysis_protocol_compare_recv_repeat(struct frame_send_head_tag *head, int state, int data,enum compare_state cmp_state )
{

    struct analysis_protocol_send_frame_list_tag *send_frame;
    struct analysis_protocol_send_frame_list_tag *tmp_send_frame;
    //获得一个元素
    send_frame = TAILQ_FIRST(head);
    while(send_frame){
        tmp_send_frame = send_frame;
        send_frame = TAILQ_NEXT(send_frame, next);
        //重复缓存中，命令状态是开启，去掉
        if(cmp_state == DATA){
            //需要同时对比数据和状态
            if(tmp_send_frame->state == state && tmp_send_frame->value == data){
                TAILQ_REMOVE(head, tmp_send_frame, next);   
                SELF_FREE(tmp_send_frame);
            }
        }else{
            //只对比状态
            if(tmp_send_frame->state == state ){
                TAILQ_REMOVE(head, tmp_send_frame, next);   
                SELF_FREE(tmp_send_frame);
            }
        }
    }
    return ;

}

//发送命令需要，在重复列表中,如果存在，且是DATA型需要更新数据，如果没有插入数据
void
analysis_protocol_recv_repeat_up(struct analysis_protocol_base_tag *base, int state, int data, enum compare_state cmp_state, uint8_t *src, uint8_t len )
{
    //需要插入的状态
    int flag = 0;
    struct analysis_protocol_send_frame_list_tag *send_frame;
    struct analysis_protocol_send_frame_list_tag *tmp_send_frame;

    //找寻是已经存在，更新数据
    TAILQ_FOREACH(tmp_send_frame, &base->send_frame_head, next) {
        if(tmp_send_frame->state == state){
            //如果是数据，需要对比数据
            if(cmp_state == DATA){
                //更新数据
                tmp_send_frame->value = data;
                tmp_send_frame->repeat_num = 0;
                tmp_send_frame->data_len = len;
                memmove(tmp_send_frame->data, src, len);
                flag = 1;
                break;
            
            }else{
                flag = 1;
                break;
            }
        }
    }
    //未找到需要查询数据
    if(flag == 0){
    
        struct analysis_protocol_send_frame_list_tag *send_dest = SELF_MALLOC(sizeof(struct analysis_protocol_send_frame_list_tag));
        if(send_dest){
        
            send_dest->repeat_max = 5;
            send_dest->repeat_num = 1;
            send_dest->value = data;
            send_dest->state = state;//关机
            memmove(send_dest->data, src, len );
            send_dest->data_len = len;
            gettimeofday(&send_dest->last_send_time,NULL);
            send_dest->repeat_during = 1;//2秒钟重发一次
            TAILQ_INSERT_TAIL(&base->send_frame_head, send_dest, next);
        }

    }

}

//插入发送队列，并且去重,或者更新
int
analysis_protocol_insert_send_list(struct analysis_protocol_base_tag *base, int state, int data, enum compare_state cmp_state, uint8_t *src, uint8_t len, int head_tail ){
    int flag = 0; //是否需要从新插入

    //首先查找是否有重复的数据，有的话更新，没有的话在插入

    struct analysis_protocol_send_frame_to_dest_tag *send_frame;
    struct analysis_protocol_send_frame_to_dest_tag *tmp_send_frame;

    TAILQ_FOREACH(tmp_send_frame, &base->send_frame_dest_head, next){
        if(tmp_send_frame->state == state){
            //如果是数据，需要对比数据和状态
            if(cmp_state == DATA){
                memmove(tmp_send_frame->data, src, len);
                tmp_send_frame->data_len = len;
                tmp_send_frame->value = data;
            }else{
                flag = 1;
                break;
            }
        }

    }
    if(flag == 0){
    
    
        struct analysis_protocol_send_frame_to_dest_tag *send_frame_dest = 
            SELF_MALLOC(sizeof(struct analysis_protocol_send_frame_to_dest_tag));

        if(send_frame_dest){
            memmove(send_frame_dest, src, len);
            send_frame_dest->data_len = len;
            send_frame_dest->state = state;
            send_frame_dest->value = data;
            if(head_tail == 0){
                TAILQ_INSERT_HEAD(&base->send_frame_dest_head, send_frame_dest, next );
            }else{
                TAILQ_INSERT_TAIL(&base->send_frame_dest_head, send_frame_dest, next );
            }

            return 0;
        }else{
            return -1;
        }
    
    }

    return 0;


}


