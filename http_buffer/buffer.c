#include <stdarg.h>  
#include <stddef.h>  
#include <setjmp.h>  
#include <google/cmockery.h>  


#include <stdio.h>
#include <stdlib.h>
#include <string.h>





#define ARRAY_IDX 2


extern char *strdup(const char *s);

typedef struct{
    char **data; //存数据的地方
    size_t *sort;//存数据顺序的数组
    size_t used; //已经使用的长度
    size_t size;//总长度
}buff_array;

//查找
char *
buff_find_key(const buff_array *p_buff, const char *key, int *idx)
{
    size_t ndata = 0;
    char *tmp = NULL;
    for(int i =0; i < p_buff->used; i++){
        ndata = p_buff->sort[i];
        tmp = p_buff->data[ndata];
        if(strcmp(tmp, key) == 0){
            *idx = i;
            return tmp;
        }
    }
    return NULL;
}


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
        p_arr->size = p_arr->size + ARRAY_IDX;    
    }

    int need_idx= 0;
    //查找小于他的数
    for(int i=0; i < p_arr->used; i++){
        idx =  p_arr->sort[i]; 
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
        p_arr->sort[p_arr->used] = p_arr->used;
        p_arr->data[p_arr->used] = strdup(key);
        p_arr->used++; 
        return 0;
    }
    //中间插入
    int remain_len = p_arr->used - need_idx;
    memmove(p_arr->sort+need_idx+1, p_arr->sort+need_idx, remain_len*sizeof(size_t));
    p_arr->data[p_arr->used] = strdup(key);
    p_arr->sort[need_idx] = p_arr->used;
    p_arr->used++;
    return 0;
}

//打印数据
void
buff_printf(buff_array *p_buf)
{
    for(int i=0; i<p_buf->used; i++){
        printf("i=%d, sort=%ld , str=%s\n", i, p_buf->sort[i], p_buf->data[i]);


    }



}

//删除key
int
buff_del_key(buff_array *p_buff, char *key)
{
    int idx = 0;
    int data_idx = 0;
    char *tmp_key = NULL;
    tmp_key = buff_find_key(p_buff, key, &idx);
    if(strcmp(tmp_key, key) == 0){
        data_idx = p_buff->sort[idx];
        //去掉data
        free(p_buff->data[data_idx]);
        memmove(p_buff->data+data_idx, p_buff->data+data_idx+1, (p_buff->used - (data_idx +1))*sizeof(char *)  );
        //去掉sort
        memmove(p_buff->sort+idx, p_buff->sort+idx+1, (p_buff->used - (idx+1))*sizeof(size_t) ) ;
        p_buff->used--;
        return 0;
    }else{
        return 1;
    }


}


void test_sub(void **state) {  
    int flag = 0;
    buff_array *p_buf = buff_init();
    assert_true(p_buf);            

    //第一个测试用例
    char *test1 = "12";
    buff_insert(p_buf, test1);
    assert_int_equal(p_buf->used, 1);
    assert_string_equal(p_buf->data[0], test1);
    assert_int_equal(p_buf->sort[0], 0);
    
    //第二个测试用例
    char *test2 = "34";
    buff_insert(p_buf, test2);
    assert_int_equal(p_buf->used, 2);
    assert_string_equal(p_buf->data[1], test2);
    assert_int_equal(p_buf->sort[1], 1);
    //第二个测试用例
    char *test3 = "56";
    buff_insert(p_buf, test3);
    assert_int_equal(p_buf->used, 3);
    assert_string_equal(p_buf->data[2], test3);
    assert_int_equal(p_buf->sort[2], 2);
    //第二个测试用例
    char *test4 = "78";
    buff_insert(p_buf, test4);
    assert_int_equal(p_buf->used, 4);
    assert_string_equal(p_buf->data[3], test4);
    assert_int_equal(p_buf->sort[3], 3);


    //第二个测试用例
    char *test5 = "35";
    buff_insert(p_buf, test5);
    
    //del
    flag = buff_del_key(p_buf, "34");
    printf("flag = %d\n", flag);
    return ;

    //查找
    char *tmp_key = NULL;
    int tmp_idx= 0;
    tmp_key = buff_find_key(p_buf, "34", &tmp_idx);

    assert_string_equal(tmp_key, "34");
    assert_int_equal(tmp_idx, 1);

    //打印数据
    buff_printf(p_buf);

    printf("test finish\n");
}  

int main()
{
    const UnitTest tests[] = {  
        unit_test(test_sub)  

    };  
    return run_tests(tests);  

}




