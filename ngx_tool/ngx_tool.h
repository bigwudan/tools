
typedef void(*ngx_pool_cleanup_pt)(void *data);
typedef struct  ngx_pool_cleanup_s ngx_pool_cleanup_t;

struct ngx_pool_cleanup_s{
	ngx_pool_cleanup_pt handler;
	void *data;
	ngx_pool_cleanup_t *next;

};

typedef struct ngx_pool_large_s ngx_pool_large_t;
struct ngx_pool_large_s{
	ngx_pool_large_t *next;
	void *alloc;

};

typedef struct ngx_pool_s ngx_pool_t;

typedef struct {
	char *last;
	char *end;
	ngx_pool_t *next;
	int failed;
}ngx_pool_data_t;
struct ngx_pool_s{
	ngx_pool_data_t d;
	int max;
	ngx_pool_t *current;
	ngx_pool_large_t *large;
	ngx_pool_cleanup_t *cleanup;


};




