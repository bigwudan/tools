#ifndef _ARRAY_H_
#define _ARRAY_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define DATA_UNSET \
	char *key; \
	char *value; 

typedef struct data_unset {
        DATA_UNSET;
} data_unset;


typedef struct {
    data_unset  **data;
    size_t *sorted;
    size_t used; /* <= SSIZE_MAX */
    size_t size;

} array;

//初始化
array *array_init(void);

//插入数据
void array_insert_unique(array *a, data_unset *entry);


//查找数据,并且插入数据
data_unset **array_find_or_insert(array *a, data_unset *entry) ;


//查找数据
size_t array_get_index(const array *a, const char *key, size_t keylen, size_t *rndx);


//生成data_unset
data_unset * create_data_unset(char *key, char *value); 

//释放data_unset
void free_data_unset(data_unset *p);

//打印数据
void
test_printf(array *p);

//通过key找到data_unset
data_unset * get_data_unset_by_key(array *p, char *key );

//释放array
void free_array(array *p);
#endif
