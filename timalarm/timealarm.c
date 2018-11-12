#include "timealarm.h"

time_list_head *p_time_list_head = NULL;
int cur_circle = 0;
int old_time = 0;

void my_fun(int avg)
{
    printf("my_fun num=%d\n", avg);

}


void init() 
{
    p_time_list_head =  (time_list_head *)calloc(sizeof(time_list_head), CIRCLE);
    if(!p_time_list_head) perror("calloc error \n");
    for(int i =0; i < CIRCLE ; i++){
        p_time_list_head[i].p_head = NULL;
        p_time_list_head[i].p_tail = NULL;
    }
    return ;
}

void add_time( time_t my_time)
{
    time_t now_t = time(NULL);
    int cur_time = my_time - now_t;
    int num_count = 0;
    int circle_num = (cur_time)/CIRCLE;
    num_count = (cur_circle + cur_time)%CIRCLE;
    time_list_node *p_my_time_list_node = calloc(sizeof(time_list_node), 1);
    p_my_time_list_node->count_num = circle_num;
    p_my_time_list_node->expire_time = my_time;
    p_my_time_list_node->fun = my_fun;
    p_my_time_list_node->p_next = NULL;
    p_my_time_list_node->p_front = NULL;       
    if(p_time_list_head[num_count].p_head == NULL){
        p_time_list_head[num_count].p_head = p_my_time_list_node;
        p_time_list_head[num_count].p_tail = p_my_time_list_node;
    }else{
        time_list_node *p_tmp_node = p_time_list_head[num_count].p_tail;
        p_tmp_node->p_next = p_my_time_list_node;
        p_my_time_list_node->p_front = p_tmp_node;
        p_time_list_head[num_count].p_tail = p_my_time_list_node;
    }
}


void run_timebynum()
{
    time_list_node *p_my_list_node =   p_time_list_head[cur_circle].p_head;
    cur_circle++;
    cur_circle = cur_circle%CIRCLE;
    if(p_my_list_node == NULL) return;
    while(p_my_list_node){
        if(p_my_list_node->count_num == 0){
    //        p_my_list_node->fun(12);

            
            if(p_time_list_head[cur_circle].p_head == p_time_list_head[cur_circle].p_tail){
                p_time_list_head[cur_circle].p_head = NULL;
                p_time_list_head[cur_circle].p_tail = NULL;
                free(p_my_list_node);
                printf("dur=%ld\n", time(NULL)- old_time);
                exit(1);
                return ;
            }else{
            
                p_my_list_node->p_front->p_next = p_my_list_node->p_next;
                time_list_node *tmp_list_node = NULL;
                tmp_list_node = p_my_list_node;
                p_my_list_node = p_my_list_node->p_next;
                free(tmp_list_node);
            }

            printf("dur=%ld\n", time(NULL)- old_time);
            exit(1);
        }else{
            p_my_list_node->count_num--;
            p_my_list_node = p_my_list_node->p_next; 
        }
    }
}

void sig_alarm(int sig) 
{ 
    run_timebynum();
    alarm(TIMESLOT);
}

int main(int arg, char **argv)
{
    printf("run \n");
    signal(SIGALRM, sig_alarm); 
    alarm(TIMESLOT);
    time_t t =time(NULL) + 210; 
    printf("now time=%ld \n", time(NULL));
    old_time = time(NULL);
    init();
    add_time(t);

    while(1){
        sleep(5);
    
    }


}
