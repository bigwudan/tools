#include <stdio.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <limits.h>


#include "evbuffer.h"

//生成一个新得buf
struct evbuffer *
evbuffer_new(void)
{
    struct evbuffer *buf = calloc(1, sizeof(struct evbuffer));
    if(!buf){
        printf("create buf error\n");
        return NULL;
    }
    return buf;
}


//生成一个新得buffer_chain
static struct evbuffer_chain *
evbuffer_chain_new(size_t size)
{
    if(size > EV_UINT64_MAX){
        return NULL;
    }
    
    struct evbuffer_chain *chain = NULL;
    size_t tot_len = MIN_BUFFER_SIZE + sizeof(struct evbuffer_chain);
    size_t ndata = 0;
    //小于最小值,直接用最小值
    if(tot_len >= size ){
        ndata = tot_len;
    }else{
        while(tot_len < size)
            tot_len << 1;
        ndata = tot_len;
    }
    chain = calloc(ndata, 1); 
    chain->buffer_len = ndata - sizeof(struct evbuffer_chain);
    if(chain == NULL)
        return NULL;
    chain->buffer =(unsigned char *)((struct evbuffer_chain *)chain + 1);
    return chain;
}


//释放在他后面的所有数据
static void
evbuffer_chain_free_before(struct evbuffer_chain *buf)
{
    struct evbuffer_chain *next = buf; 
    struct evbuffer_chain *tmp = NULL;
    while( next  ){
        tmp = next;
        next = next->next;
        free(tmp);
    }
}


//插入一个chain
static void
evbuffer_chain_insert(struct evbuffer *buf,
            struct evbuffer_chain *chain)
{
    if(buf == NULL) return ;
    //如果是一个新的链表，还没有插入连接,直接插入
    if(buf->last == NULL){
        buf->first = buf->last = chain;
        buf->last_with_datap = &buf->first;
        buf->total_len = chain->buffer_len;
    }else{
        struct evbuffer_chain *old = NULL;
        struct evbuffer_chain *next = NULL;
        struct evbuffer_chain *tmp = NULL;
        struct evbuffer_chain *head = buf->first;
        //找到第一个数据未空
        while(next->off !=0 && next){
            old = next;
            next = next->next;
        }
        //释放所有未空的数据
        if(next){
            evbuffer_chain_free_before(next);
            buf->last_with_datap = &old;
            old->next = chain;
            buf->last = chain;
        }else{
            buf->last->next = chain;
            buf->last = chain;
        } 
    }

}




int main()
{

    return 1;


}



