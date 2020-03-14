#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "analysis_protocol.h"
#include "analysis_protocol_tools.h"



uint8_t 
yingxue_frame_recv_fun(struct analysis_protocol_base_tag *base)
{
    uint8_t flag = 0; //是否有数据
    uint8_t data = 0x00;//保存的数据
    uint8_t len =0;//长度
    struct chain_list_tag *chain = base->chain_list;
    struct yingxue_frame_tag *yingxue = (struct yingxue_frame_tag *)base->recv_frame;
    do{
        is_empty_chain_list(chain, flag);
        if(flag == 0) break;
        out_chain_list(chain, data);
        printf("rec=0x%02X\n ", data);
        yingxue->data[len++] = data;
    }while(flag);
    return 1;
}

//得到一个完整的数据，然后分析，最后判断是否写入缓存
uint8_t
yingxue_process_frame(struct analysis_protocol_base_tag *base, void *arg)
{
   //写入写入缓存
    printf("wudan\n");
    //是否加入检查重试的队列
    struct analysis_protocol_send_frame_list_tag *send_dest = SELF_MALLOC(sizeof(struct analysis_protocol_send_frame_list_tag));
    send_dest->repeat_max = 2;
    send_dest->repeat_num = 1;
    send_dest->data[0]  = 0x1;
    send_dest->data[1] = 0x2;
    send_dest->data_len = 2;
    gettimeofday(&send_dest->last_send_time,NULL);
    send_dest->repeat_during = 2;
    TAILQ_INSERT_TAIL(&base->send_frame_head, send_dest, next);
    //加入发送列表

    struct analysis_protocol_send_frame_to_dest_tag *send_frame_dest = 
        SELF_MALLOC(sizeof(struct analysis_protocol_send_frame_to_dest_tag));

    memmove(send_frame_dest, send_dest->data, send_dest->data_len);
    send_frame_dest->data_len = send_dest->data_len;
    TAILQ_INSERT_TAIL(&base->send_frame_dest_head, send_frame_dest, next );
    base->send_func(base);
        

}


//底层发送数据
uint8_t
send_func_bc(struct analysis_protocol_base_tag *base)
{
    struct analysis_protocol_send_frame_to_dest_tag *send;
    send = TAILQ_FIRST(&base->send_frame_dest_head);

    TAILQ_REMOVE(&base->send_frame_dest_head, send, next); 

    printf("send uart\n");

    return 0;
}



//测试
void test_fun( struct analysis_protocol_base_tag *base )
{
    //读的数据写入数据库
    uint8_t buffer[] = {0x11, 0x22, 0x33};
    uint8_t len = 0;
    //写入缓存
    len = analysis_protocol_write_chain_list(base->chain_list, buffer, sizeof(buffer));
    printf("len=0x%02X\n", len);
    
    //运行分析数据看是否得到一个完整的数据
    len = base->frame_recv_fun(base);
    if(len == 1){
        base->self_process_frame(base, NULL);

    }




}




int main()
{
    struct analysis_protocol_base_tag *base_yingxue = analysis_protocol_init((void *)0, yingxue_frame_recv_fun, yingxue_process_frame, send_func_bc ); 
    test_fun(base_yingxue);




    printf("wudan\n");


}
