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


//查找数据
static data_unset **array_find_or_insert(array *a, data_unset *entry) ;

#endif
