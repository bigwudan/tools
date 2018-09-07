#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <stdint.h>

#define NGX_MAX_ALLOC_FROM_POOL 4095
#define NGX_ALIGNMENT   sizeof(unsigned long)    
#define ngx_align_ptr(p, a) \
(u_char *) (((uintptr_t) (p) + ((uintptr_t) a - 1)) & ~((uintptr_t) a - 1))

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
extern ngx_pool_t *ngx_create_pool(int);

extern void *ngx_palloc(ngx_pool_t *,int);


