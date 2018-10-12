#include <string.h>
#include <stdio.h>
#include <stdlib.h>
typedef struct time_node time_node;
struct time_node
{
    int expire_time;
    void (*bk_fun)( void * arv );
    time_node *next;
    time_node *prev;
};

typedef struct time_list
{
    time_node *head;
    time_node *tail;
}time_list;


time_list *
init_fun(time_list **pp_time_list)
{
   *pp_time_list =  calloc(1, sizeof(time_list));
    (*pp_time_list)->head = NULL;
    (*pp_time_list)->tail = NULL;
    return *pp_time_list;
}

int
add_fun(time_list *pp_time_list, time_node *p_time_node)
{
    if(pp_time_list->head == NULL && pp_time_list->tail == NULL ){
        pp_time_list->head = p_time_node;
        pp_time_list->tail = p_time_node;
        p_time_node->next = NULL;
        p_time_node->prev = NULL;
        return 1; 
    }else{
        time_node *p_tmp = pp_time_list->head;
        while(p_tmp){
            if(p_tmp->expire_time >= p_time_node->expire_time ){
                break; 
            } 
            p_tmp = p_tmp->next;
        }
        //未找到
        if(p_tmp == NULL){
            p_time_node->prev = pp_time_list->tail;
            pp_time_list->tail->next = p_time_node;
            pp_time_list->tail = p_time_node;
            p_time_node->next = NULL;
            return 1;
        }
        //找第一个位置
        if(p_tmp->prev == NULL){
            pp_time_list->head = p_time_node;
            p_tmp->prev = p_time_node;
            p_time_node->prev = NULL;
            p_time_node->next = p_tmp;
            return 1; 
        }
        p_time_node->prev = p_tmp->prev;
        p_time_node->next = p_tmp;
        p_tmp->prev->next = p_time_node;
        p_tmp->prev = p_time_node;
        return 1;
    }
}

int
del_fun(time_list *pp_time_list, time_node *p_time_node)
{
    if(p_time_node->next == NULL && p_time_node->prev == NULL){
        pp_time_list->head = NULL;
        pp_time_list->tail = NULL;
    }else if(p_time_node->prev == NULL){
        pp_time_list->head = p_time_node->next;
        pp_time_list->head->prev = NULL;
    }else if(p_time_node->next == NULL){
        pp_time_list->tail = p_time_node->prev;
        pp_time_list->tail->next = NULL;
    }else{
        p_time_node->prev->next = p_time_node->next;
        p_time_node->next->prev = p_time_node->prev;
    }    
    p_time_node = NULL;
    return 0;
}

void
show_fun(time_list *pp_time_list)
{
    time_node *p_tmp = pp_time_list->head;
    while(p_tmp){
        printf("expire_time=%d\n", p_tmp->expire_time);
        p_tmp = p_tmp->next;
    }
}



int main(int argc, char **argv)
{
    
    time_list *p_my_list = NULL;
    printf("init_fun=%p\n", init_fun(&p_my_list)); 
    time_node time_node_1;
    time_node_1.expire_time = 10;
    time_node_1.prev = NULL;
    time_node_1.next = NULL;
    add_fun(p_my_list, &time_node_1);
    

    time_node time_node_2;
    time_node_2.expire_time = 3;
    time_node_2.prev = NULL;
    time_node_2.next = NULL;
    add_fun(p_my_list, &time_node_2);


    time_node time_node_3;
    time_node_3.expire_time = 20;
    time_node_3.prev = NULL;
    time_node_3.next = NULL;
    add_fun(p_my_list, &time_node_3);

    //del_fun(p_my_list, &time_node_1);

    del_fun(p_my_list, &time_node_3);

    //del_fun(p_my_list, &time_node_2);

    show_fun(p_my_list);


    return 0;

}
