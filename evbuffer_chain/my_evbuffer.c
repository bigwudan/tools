#include <stdio.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <limits.h>

#include <stdarg.h>  
#include <stddef.h>  
#include <setjmp.h>  


#include <google/cmockery.h>



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
            tot_len <<= 1;
        ndata = tot_len;
    }
    chain = calloc(ndata, 1); 
    chain->buffer_len = ndata - sizeof(struct evbuffer_chain);
    if(chain == NULL)
        return NULL;
    chain->buffer =(unsigned char *)((struct evbuffer_chain *)chain + 1);
    return chain;
}


//释放在他和他后面的所有数据
static void
evbuffer_chain_free_before(struct evbuffer_chain *buf,
        struct evbuffer *evbuf)
{
    struct evbuffer_chain *next = buf; 
    struct evbuffer_chain *tmp = NULL;
    while( next  ){
        tmp = next;
        next = next->next;
        evbuf->total_len -= tmp->buffer_len;
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
        struct evbuffer_chain *tmp = NULL;
        struct evbuffer_chain *next = buf->first;
        //找到第一个数据=空
        while( next &&  next->off !=0 ){
            old = next;
            next = next->next;
        }
        //释放所有未空的数据
        if(next){
            evbuffer_chain_free_before(next, buf);
            //头节点
            if(old == NULL){
                buf->first = chain;
                buf->last = chain;
            }else{
                old->next = chain;
                buf->last = chain;
            }
            buf->total_len += chain->buffer_len;
            //需要 从新计算total_len
        }else{
            buf->last->next = chain;
            buf->last = chain;
            buf->total_len += chain->buffer_len;
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
    if(buf->first == NULL){
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
    if (buf->last->off < 100 ){
        //找到last 上一个
        struct evbuffer_chain *next = buf->first;
        while(next && next->next != buf->last){
            next = next->next; 
        }
        
        //头节点
        if(next == NULL){
            //移动原来的数据
            struct evbuffer_chain *res_chain = evbuffer_chain_new(buf->last->off+datlen);
            if(buf->last->off != 0){
                memmove(res_chain->buffer+res_chain->misalign, buf->last->buffer+buf->last->misalign, buf->last->off);
            }
            res_chain->off = buf->last->off;
            //删除原来的连接
            free(buf->last);
            buf->first = buf->last = res_chain;
            //移动原来的数据到新的chain
            return res_chain;
        
        }else{
            //移动原来的数据
            struct evbuffer_chain *res_chain = evbuffer_chain_new(buf->last->off+datlen);
            if(buf->last->off != 0)
                memmove(res_chain->buffer+res_chain->misalign, buf->last->buffer+buf->last->misalign, buf->last->off);
            res_chain->off = buf->last->off;
            //删除原来的连接
            free(buf->last);
            next->next = res_chain;
            //移动原来的数据到新的chain
            return res_chain;
        }

    }else{
        struct evbuffer_chain *res_chain = evbuffer_chain_new(buf->last->off+datlen);
        buf->last = res_chain;
       (*(buf->last_with_datap))->next = res_chain;
        return res_chain; 
    }
}



int
evbuffer_drain(struct evbuffer *buf, size_t len)
{
    struct evbuffer_chain *next = buf->first;
    size_t remain = len;
    size_t ntmp = 0;
    //向前剔除
    while(next && remain != 0){
        if(remain >= next->off){
            next->misalign = next->off;
            next->off = 0;
            remain = remain - next->off;
        }else{
            next->misalign = remain;
            next->off = next->off - remain;
            remain = 0;
        } 
        next = next->next;
    }    
    return remain;
}

//拷贝数据 不删除
size_t
evbuffer_copyout(struct evbuffer *buf, void *data_out, size_t datlen)
{
    struct evbuffer_chain *next = buf->first;
    size_t remain = datlen;
    void *p = data_out;
    //向前剔除
    while(next && remain != 0){
        if(remain >= next->off){
            memmove(p, next->buffer+next->misalign, next->off);
            p = p + next->off;
            remain = remain - next->off;
        }else{
            memmove(p, next->buffer+next->misalign, next->off);
            remain = 0;
        } 
        next = next->next;
    }    
    return remain;
}


//插入数据
int
evbuffer_add(struct evbuffer *buf, const void *data_in, size_t datlen)
{
    if(buf == NULL) return -1;
    //首先判断是否有需要扩空间
    struct evbuffer_chain *chain = evbuffer_expand(buf, datlen);  
    if(chain == NULL) return -1;
    memmove(chain->buffer+chain->misalign,  data_in, datlen );
    chain->off += datlen;
    buf->last_with_datap = &(buf->last);

    return datlen;
}


//测试创建一个evbuffer,并且添加进去
void test_insert(void **state) {  
    //生成一个evbuffer 
    struct evbuffer *buf = evbuffer_new();
    assert_true(buf);
    //生成一个evbuffer_chain
    struct evbuffer_chain *buf_chain_1 = evbuffer_chain_new(1);
    assert_true(buf_chain_1);

    struct evbuffer_chain *buf_chain_2 = evbuffer_chain_new(2);
    assert_true(buf_chain_2);

    struct evbuffer_chain *buf_chain_3 = evbuffer_chain_new(3);
    assert_true(buf_chain_3);

    //插入一个空表
    evbuffer_chain_insert(buf, buf_chain_1);
    assert_int_equal(buf->first, buf_chain_1); 
    assert_int_equal(buf->last, buf_chain_1); 
    assert_int_equal(buf->last_with_datap, &buf->first); 
    assert_int_equal(buf->total_len, buf_chain_1->buffer_len);
    
    //继续插入第二章表

    evbuffer_chain_insert(buf, buf_chain_2);

    assert_int_equal(buf->first, buf_chain_2); 
    assert_int_equal(buf->last, buf_chain_2); 
    assert_int_equal(buf->last_with_datap, &buf->first); 
    assert_int_equal(buf->total_len,  buf_chain_2->buffer_len);

    //继续插入第3章表

    evbuffer_chain_insert(buf, buf_chain_3);
    assert_int_equal(buf->first, buf_chain_3); 
    assert_int_equal(buf->last, buf_chain_3); 
    assert_int_equal(buf->last_with_datap, &buf->first); 
    assert_int_equal(buf->total_len, buf_chain_3->buffer_len);

    //扩展空间 空间数量满足
    struct evbuffer_chain *tmp_1 =  evbuffer_expand(buf, 230);

    assert_int_equal(tmp_1, buf_chain_3); 

    //扩展空间，  空间数量不足
    struct evbuffer_chain *tmp_2 =  evbuffer_expand(buf, 1000);
    assert_true(tmp_2);
    char test[300] = {1};

    int a = evbuffer_add(buf, test, 300);
    printf("a=%d\n", a);
    
    char test1[2000] = {2};

    a = evbuffer_add(buf, test1, sizeof(test1));
    printf("a=%d\n", a);

}  




int main()
{
    const UnitTest tests[] = {  
        unit_test(test_insert),  

    };  
    return run_tests(tests);  





}



