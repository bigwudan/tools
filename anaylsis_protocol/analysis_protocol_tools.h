#ifndef _ANALYSIS_PROTOCOL_TOOLS_H
#define _ANALYSIS_PROTOCOL_TOOLS_H
#include <stdint.h>
#include <string.h>

#define MAX_CHAIN_NUM 60


//环形队列
struct chain_list_tag{
	uint8_t rear; //尾结点
	uint8_t front; //头结点
	uint8_t count; //当前数量
	uint8_t buf[MAX_CHAIN_NUM];
};

//初始化一个环形队列
#define create_chain_list(p_chain_list) do{ p_chain_list->rear= 0; p_chain_list->front = 0; p_chain_list->count = 0;memset(p_chain_list->buf, 0, sizeof(p_chain_list->buf));  }while(0)

//是否为空 flag 0空 1未空
#define is_empty_chain_list(p_chain_list, flag) do{\
    if(p_chain_list->rear == p_chain_list->front){flag =0;}else{flag = 1;}\
}while(0)

//是否已经满 flag 0满 1 为满
#define is_full_chain_list(p_chain_list, flag) do{\
    if(  ((p_chain_list->rear + 1) % MAX_CHAIN_NUM) == p_chain_list->front   ){flag = 0;}else{flag = 1;}\
}while(0)

//插入数据，但是插入前先判度是否满
#define in_chain_list(p_chain_list, src) do{ *(p_chain_list->buf + p_chain_list->rear) = src;  p_chain_list->rear = (p_chain_list->rear + 1) % MAX_CHAIN_NUM; }while(0)

//取出数据，但是出去前先要判断是否空
#define out_chain_list(p_chain_list, src) do{src = *(p_chain_list->buf + p_chain_list->front);p_chain_list->front = (p_chain_list->front + 1) % MAX_CHAIN_NUM;}while(0)


#endif
