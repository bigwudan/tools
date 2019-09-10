#include "array.h"

//初始化
array *
array_init(void) {
    array *a;
    a = calloc(1, sizeof(*a));
    return a;
}

//插入数据
void 
array_insert_unique(array *a, data_unset *entry) {
    data_unset **old;
    return ;
}

//查找数据
static 
data_unset **array_find_or_insert(array *a, data_unset *entry) 
{

    return NULL;
}

