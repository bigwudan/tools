#include <stdio.h>
#include <stdlib.h>
#include <string.h>





#define ARRAY_IDX 16


extern char *strdup(const char *s);

typedef struct{
    char **data; //存数据的地方
    size_t *sort;//存数据顺序的数组
    size_t used; //已经使用的长度
    size_t size;//总长度
}buff_array;

//初始化
buff_array *
buff_init()
{
    buff_array *p_buf = calloc(1, sizeof(buff_array));
    if(p_buf == NULL) return NULL;
    //指针指针地址
    p_buf->data = calloc(ARRAY_IDX, sizeof(char *));
    if(p_buf->data == NULL) return NULL;
    //顺序数组
    p_buf->sort = calloc(ARRAY_IDX, sizeof(char));
    //已经使用的长度
    p_buf->used = 0;
    //总长度
    p_buf->size = ARRAY_IDX;
    return p_buf;
}


//插入key
int
buff_insert(buff_array *p_arr, char *key )
{
    char *tmp = NULL;
    int idx = 0;
    int flag = 0;
    //如果是初始值 直接插入
    if(p_arr->used == 0){
       p_arr->used = 1;
       p_arr->sort[0] = 0;
       p_arr->data[0] =  strdup(key);
       return 0;
    }   
        
    //如果相等就需要扩容
    if(p_arr->used == p_arr->size){
        p_arr->data = realloc(p_arr->data, (p_arr->size + ARRAY_IDX)*(sizeof(char *)));;
        if(p_arr->data == NULL){
            return -1;
        } 
        p_arr->sort = realloc(p_arr->sort, (p_arr->size + ARRAY_IDX)*(sizeof(size_t)));
    
    }

    int need_idx= 0;
    //查找小于他的数
    for(int i=0; i < p_arr->used; i++){
        idx =  p_arr->sort[i]; 
        p_arr->data[idx];
        flag = strcmp(p_arr->data[idx], key);
        if(flag == 0){
            return 0;
        }
        //需要插入得位置
        if(flag >0){
            need_idx = i;
            break;
        }
    }
    
    //如果是末尾直接插入
    if(need_idx== 0){
        p_arr->sort[p_arr->used - 1] = p_arr->used - 1;
        p_arr->data[p_arr->used - 1] = strdup(key);
        p_arr->used++; 
        return 0;
    }

    //中间插入
    int remain_len = p_arr->used - need_idx;
    memmove(p_arr->sort+need_idx+1, p_arr->sort+need_idx, remain_len);
    p_arr->data[p_arr->used] = strdup(key);
    p_arr->sort[need_idx] = p_arr->used;
    p_arr->used++;
    return 0;
}


int main()
{
    buff_array *p_buf = buff_init();

    printf("wudan\n");

    return 0;
}




