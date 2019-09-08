#include <stdarg.h>  
#include <stddef.h>  
#include <setjmp.h>  
#include <google/cmockery.h>  


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/queue.h>


struct _node_list {  
    char buf[250];  
    TAILQ_ENTRY(_node_list)  next;  

};  


TAILQ_HEAD(node_head, _node_list);




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
buff_del_key(buff_array *p_buff,const char *key)
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


int
_test_bufdayu(buff_array *p_buf, int ndata)
{
    int idx = 0;
    int idx_next = 0;
    for(int i =0; i < p_buf->used-1; i++){
        idx = p_buf->sort[i];
        idx_next = p_buf->sort[i+1];
        if(strcmp(p_buf->data[idx], p_buf->data[idx_next]) >0 ){
            return -1;
        }
    }
    assert_int_equal(p_buf->used, ndata);
    return 0;
}

//删除
void
_test_del(buff_array *p_buf, struct node_head *head, const char *key)
{

    buff_del_key(p_buf, key);
    struct _node_list *tmp_node = NULL;
    struct _node_list *tmp_item = NULL;
//删除一个元素
    printf("Deleting item with value 3: ");
    for(tmp_node = TAILQ_FIRST(head); tmp_node != NULL; tmp_node = tmp_item) {
        if ( strcmp(tmp_node->buf, key) == 0) {
            //删除一个元素
            TAILQ_REMOVE(head, tmp_item, next);
            //释放不需要的内存单元
            free(tmp_node);
            break;
        }
        tmp_item = TAILQ_NEXT(tmp_node, next);
    }



}

//对比数据
int
_test_cmp_serial_head_buf(buff_array *p_buf, struct node_head *head   )
{

	struct _node_list *item = NULL; 
	int i =0;
	int idx = 0;
	char *tmp = NULL;
    TAILQ_FOREACH(item, head, next) {
					
		idx =  p_buf->sort[i++];
		tmp = p_buf->data[idx];

		assert_int_equal(strcmp(tmp, item->buf), 0);
	
    }
	return i;
}

void test_new(void **state){

    int flag = 0;
    struct node_head head;
    TAILQ_INIT(&head);
    struct _node_list *tmp_node = NULL;

    char *pp_str[] = {"11", "22", "33", "44", "55"};
    size_t tot = sizeof(pp_str)/sizeof(char *);
    for(int i=0; i<tot; i++){
        tmp_node = calloc(1, sizeof(struct _node_list));
        memmove(tmp_node->buf, pp_str[i], strlen(pp_str[i])*sizeof(char) );
        TAILQ_INSERT_TAIL(&head, tmp_node, next);

    }

    buff_array *p_buf = buff_init();
    assert_true(p_buf);            
    TAILQ_FOREACH(tmp_node, &head, next){
        buff_insert(p_buf, tmp_node->buf);
    }



    assert_int_equal(p_buf->used, tot);

    //buff_del_key(p_buf, "33");
	_test_del(p_buf, &head, "33");	

    flag = _test_bufdayu(p_buf, tot-1);

	_test_cmp_serial_head_buf(p_buf, &head);

    printf("flag=%d\n", flag);


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
    
    flag = _test_bufdayu(p_buf, 4);

    assert_int_equal(flag, 0);


    return ;

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
        //unit_test(test_sub)  
        unit_test(test_new)  

    };  
    return run_tests(tests);  

}




