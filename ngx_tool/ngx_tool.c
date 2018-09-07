#include "ngx_tool.h"


ngx_pool_t *
create_ngx_pool(int size)
{
    char *p_pool = NULL;
    p_pool = calloc(size, 1);
    if(p_pool == NULL){
        return NULL; 
    }
    ngx_pool_t *p_pool_t = NULL;
    p_pool_t = (ngx_pool_t *)p_pool;
    p_pool_t->d.last = p_pool + sizeof(ngx_pool_t);
    p_pool_t->d.end = p_pool +  size;
    p_pool_t->max = size;
    p_pool_t->large = NULL;
    p_pool_t->current = p_pool_t;
    p_pool_t->cleanup = NULL;



}


