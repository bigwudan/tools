#include "ngx_tool.h"


static void *
ngx_palloc_block(ngx_pool_t *pool,int size)
{



}



ngx_pool_t *
ngx_create_pool(int size)
{
    char *p_pool = NULL;
    int err = 0;
    err = posix_memalign((void *)&p_pool, 16, size);
    if(p_pool == NULL){
        return NULL; 
    }
    ngx_pool_t *p_pool_t = NULL;
    p_pool_t = (ngx_pool_t *)p_pool;
    p_pool_t->d.last = p_pool + sizeof(ngx_pool_t);
    p_pool_t->d.end = p_pool +  size;
    size = size - sizeof(ngx_pool_t);
    p_pool_t->max = size > NGX_MAX_ALLOC_FROM_POOL ? NGX_MAX_ALLOC_FROM_POOL : size  ;
    p_pool_t->large = NULL;
    p_pool_t->current = p_pool_t;
    p_pool_t->cleanup = NULL;
    
    return p_pool_t;


}

void *
ngx_palloc(ngx_pool_t *pool , int size){
    unsigned char *m = NULL;
    ngx_pool_t *p = NULL;
    if(size <= pool->max  ){

        p = pool->current;

        do{
            m = ngx_align_ptr(p->d.last, NGX_ALIGNMENT); 
            if( (p->d.end - m) > size  ){
                p->d.last =  m + size;
                return m;     
            }
            p = p->d.next;


        }while(p);
        
        return ngx_palloc_block(pool, size);


    }
}










