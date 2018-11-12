#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#include <errno.h>


#define CIRCLE 60
#define TIMESLOT 1

/*typedef struct time_data
{
    time_t epire_time;
    void (*fun)(void *);
    int count_num;
}time_data;*/

typedef struct time_list_node time_list_node;


struct time_list_node
{
    int count_num;
    time_t expire_time;
    void (*fun)(int );

    time_list_node *p_next;
    time_list_node *p_front;
};

typedef struct time_list_head
{
    time_list_node *p_head;
    time_list_node *p_tail;


}time_list_head;



