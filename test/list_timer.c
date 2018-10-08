#include "list_timer.h"

void 
init_timer(sort_timer_list *p_timer_list)
{
    p_timer_list->head = NULL;
    p_timer_list->tail = NULL;

}

void
del_timer(sort_timer_list *p_timer_list, util_timer *p_util_timer)
{
    if(p_timer_list->head == p_util_timer && p_timer_list->tail == p_util_timer ){
    
    
    }



}


int main()
{

    printf("test\n");
}


