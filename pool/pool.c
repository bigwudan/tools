#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "pool.h"


int init_pool(pool *p_pool)
{
	int i = 0;
	int j = 0;
	int block_len = sizeof(block);
	char *p_star = NULL;
	block *p_old_block = NULL;
	block_len += p_pool->inodenum*4;
	for(i = 0; i < p_pool->iblocknum; i++){
		block *p_block = (block *)malloc(block_len);
		p_block->totnum = p_pool->inodenum;
		p_block->freenum = p_pool->inodenum;
		p_block->free_count = 0;
		p_block->p_next_block = NULL;
		p_star = (char *)p_block + sizeof(block);
		p_block->p_adata = p_star;
		for(j = 0; j < p_pool->inodenum-1; j++){
			*((short *)p_star) = j+1;
			p_star = p_star + 4;
		}
		*((short *)p_star) = -1;
		if(i == 0){
			p_old_block = p_block;
			p_pool->p_next = p_block;
		}else{
			p_old_block->p_next_block = p_block;
			p_old_block = p_block;
		}
	}
	return 1;
}


char *my_malloc(pool *p_pool)
{
	int free_count = 0;
	char *p_free = NULL;
	block *p_block = p_pool->p_next;
	while( p_block  )
	{
		if(p_block->freenum > 0 ){
			free_count = p_block->free_count;
			p_free =  p_block->p_adata + free_count*4;
			p_block->freenum--;
			p_block->free_count = *((short *)p_free);
			return p_free;
		}
		p_block = p_block->p_next_block;
	}
	return NULL;
}

int main()
{
	char *p_free = NULL;
	pool my_pool;
	my_pool.iblocknum = 2;
	my_pool.inodenum = 4;
	my_pool.p_next = NULL;
	init_pool(&my_pool);
	p_free = my_malloc(&my_pool);
		
	printf("p_free=%p\n", p_free);




}

