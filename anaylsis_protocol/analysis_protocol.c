#include <string.h>
#include <stdlib.h>
#include "analysis_protocol.h"
#include "analysis_protocol_tools.h"



//初始化
struct analysis_protocol_base_tag * 
analysis_protocol_init( void *arg, frame_recv_fun_tag frame_recv_bc,
                        self_process_frame_tag self_process_bc,
                        send_func_tag send_func_bc
                        )
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
    base->frame_recv_fun = frame_recv_bc;
    base->self_process_frame = self_process_bc;
    base->send_func = send_func_bc;

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
        is_full_chain_list(chain_list , flag );
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
        is_empty_chain_list(chain_list, flag);
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
        if(tmp_send_frame->repeat_num++ >= tmp_send_frame->repeat_max){
            TAILQ_REMOVE(&base->send_frame_head, tmp_send_frame, next);   
            SELF_FREE(tmp_send_frame);
        }else{
            //是否到达超时时间
            if(base->curr_cache_time.tv_sec >= (tmp_send_frame->last_send_time.tv_sec + tmp_send_frame->repeat_during ) ){
                //加入重发 
                tmp_frame_dest = (struct analysis_protocol_send_frame_to_dest_tag *)SELF_MALLOC(sizeof(struct analysis_protocol_send_frame_to_dest_tag));
                memmove(tmp_frame_dest->data, tmp_send_frame->data, tmp_send_frame->data_len);
                //插入数据
                TAILQ_INSERT_TAIL(&base->send_frame_dest_head, tmp_frame_dest, next);
                //更改时间
                memmove(&tmp_send_frame->last_send_time, &base->curr_cache_time, sizeof(struct timeval));
            } 
        }
    }    
    return ;
}



