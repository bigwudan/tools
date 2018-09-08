#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ngx_tool.h"


int create_pool(ngx_pool_t *p_ngx_pool_t, int size)
{
	char *p_data = NULL;
	posix_memalign(&p_data, 16, size);
	p_ngx_pool_t = calloc(sizeof(ngx_pool_t), 1);

	p_ngx_pool_t->max = size;
	p_ngx_pool_t->large = NULL;
	p_ngx_pool_t->cleanup = NULL;
	p_ngx_pool_t->current = p_ngx_pool_t;
	p_ngx_pool_t->d->last = p_data;
	p_ngx_pool_t->d->end = p_data+size;
	p_ngx_pool_t->d->next = 0;
	p_ngx_pool_t->d->failed = 0;




	
}




int main()
{
	printf("test\n");

}

