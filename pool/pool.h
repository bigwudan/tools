

typedef struct _block
{
	int totnum;
	int freenum;
	int free_count;
	struct _block * p_next_block;
	char *p_adata;
}block;


typedef struct _pool
{
	int iblocknum;
	int inodenum;
	block *p_next;
}pool;


int init_pool(pool *);

char *my_malloc(pool *);




