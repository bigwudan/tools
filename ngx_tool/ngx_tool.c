#include "ngx_tool.h"


static void *
ngx_palloc_block(ngx_pool_t *pool,int size)
{
	unsigned char *m;
	int psize;
	ngx_pool_t *new, *current, *p;
	psize = (int)(pool->d.end - pool);
	posix_memalign((void *)&new, 16, psize);
	m = (void *)new;
	new->d.end = m + psize;
	new->d.next = NULL;
	new->d.failed = 0;
	
	m += sizeof(ngx_pool_data_t);
	m = ngx_align_ptr(m, NGX_ALIGNMENT);//对m指针按4字节对齐处理

	new->d.last = m + size;//设置新内存块的last，即申请使用size大小的内存

	current = pool->current;
	for(p = current; p->d.next; p=p->d.next ){
		if (p->d.failed++ > 4) {
			current = p->d.next;
		}
	}	
	p->d.next = new;
	pool->d.current = current ? current : new;
	return m;
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










