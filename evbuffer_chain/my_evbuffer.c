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

//扩大内存
struct evbuffer_chain *evbuffer_expand(struct evbuffer *buf,
            size_t datlen)
{
    //查看最大缓存限制
    if(buf->total_len + datlen >= EV_UINT64_MAX){
        return NULL;
    }
    
    //未初始化
    if(buf->last == buf->first){
        struct evbuffer_chain *chain= evbuffer_chain_new(datlen);
        evbuffer_chain_insert(buf, chain);
        return chain;
    }
   
    //最后一个链表长度，剩余的可用量
    size_t remain = buf->last->buffer_len - buf->last->misalign - buf->last->off;
    //不需要移动，就满足长度
    if(remain >= datlen){
        return buf->last;
    }
    
    //看能不能通过mis移动满足条件
    remain = remain+buf->last->misalign;
    if(remain >= datlen){
        //移动数据
        memmove(buf->last->buffer, buf->last->buffer + buf->last->misalign, buf->last->off);
        buf->last->misalign = 0; 
        return buf->last;
    }

    //检查最后一个模块的数据值不值换到新的chain上面
    if(buf->last->off < 2014 * 5 ){
        //找到last 上一个
        struct evbuffer_chain *next = buf->first;
        while(next && next->next != buf->last){
            next = next->next; 
        }

        //移动原来的数据
        struct evbuffer_chain *res_chain = evbuffer_chain_new(buf->last->off+datlen);
        memmove(res_chain->buffer+res_chain->misalign, buf->last->buffer+buf->last->misalign, buf->last->off);

        res_chain->off = buf->last->off;

        //删除原来的连接
        free(buf->last);
        next->next = res_chain;
        *buf->last_with_datap = res_chain;
        //移动原来的数据到新的chain
        return res_chain;
    }else{
        struct evbuffer_chain *res_chain = evbuffer_chain_new(buf->last->off+datlen);
        return res_chain; 
    }
}




int main()
{

    return 1;


}



