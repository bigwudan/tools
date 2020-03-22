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
                tmp_frame_dest = (struct analysis_protocol_send_frame_to_dest_tag *)SELF_MALLOC(sizeof(struct analysis_protocol_send_frame_to_dest_tag));
                if(tmp_frame_dest){
                    tmp_frame_dest->data_len = tmp_send_frame->data_len;
                    memmove(tmp_frame_dest->data, tmp_send_frame->data, tmp_send_frame->data_len);
                    //插入数据
                    TAILQ_INSERT_TAIL(&base->send_frame_dest_head, tmp_frame_dest, next);
                    //更改时间
                    memmove(&tmp_send_frame->last_send_time, &base->curr_cache_time, sizeof(struct timeval));
                }
            }
        }
    }    
    return ;
}



