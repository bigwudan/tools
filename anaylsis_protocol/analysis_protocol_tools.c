#include <stdio.h>
#include "analysis_protocol.h"
#include "analysis_protocol_tools.h"



/*
 *插入队列
 *
 * */
uint8_t
analysis_protocol_tools_insert_cmd_frame( analysis_protocol_cmd_frame_head *head, analysis_protocol_cmd_frame *cmd_frame)
{
    if(head == NULL || cmd_frame == NULL) return 1;
    //插入最后一个
    //1第一个
    if(head->frame_first == NULL){
        head->frame_first = cmd_frame;
        head->frame_last = cmd_frame;
    }else {
        cmd_frame->pre_frame = head->frame_last;
        head->frame_last->next_frame = cmd_frame;
        head->frame_last = cmd_frame;
    }
    head->frame_tot++;

    return 0;

}

//删除队列
uint8_t
analysis_protocol_tools_remove(analysis_protocol_cmd_frame_head *head, analysis_protocol_cmd_frame *cmd_frame)
{
    if(head == NULL || cmd_frame == NULL) return 1;
    analysis_protocol_cmd_frame *frame = head->frame_first;
    
    while( frame  ){
        if(frame == cmd_frame) break;
        frame = frame->next_frame;
    }
    
    //是第一个
    if(frame->pre_frame == NULL && frame->next_frame == NULL ){
        head->frame_first = NULL;
        head->frame_last = NULL;
    //最后一个
    }else if( frame->next_frame == NULL  ){
        head->frame_last = frame->pre_frame;
        frame->pre_frame->next = NULL;
    //中间
    }else{
        frame->pre_frame->next = frame->next;
        frame->next->pre_frame = frame->pre_frame;
    }

    return 0;

}





