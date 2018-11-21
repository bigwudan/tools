#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#include "time_wheel.h"

struct tw_timer_header *  
tw_timer_init(int *cur)
{
	struct tw_timer_header *p_header = NULL;
	p_header =  (struct tw_timer_header *)calloc(sizeof(struct tw_timer_header), N);
	for(int i=0; i < N; i++){
	
		p_header[i].slots = i;
		p_header[i].p_next = NULL;
	}
	*cur = 1;
	return p_header;
}

int
tw_timer_add(struct tw_timer_header *p_timer_header , int timer , const int cur)
{
	int ticket = 0;
	int rotation = 0;
	int slots = 0;
	if(timer < SI){
		ticket = 1;
	}else{
		ticket = timer/SI;
	}
	rotation = ticket/N;
	slots = (cur + (ticket%N))%N;
	struct tw_timer *p_timer = (struct tw_timer *)calloc(sizeof(struct tw_timer), 1);
	p_timer->count = 0;
	p_timer->timer = timer;
	p_timer->rotation = rotation;

    p_timer->p_header = &(p_timer_header[slots]);

	p_timer->p_next = NULL;
	p_timer->p_prev = NULL;
    struct tw_timer *p_tw_timer = NULL;
	if(p_timer_header[slots].p_next){
        	p_tw_timer = p_timer_header[slots].p_next;
            p_timer_header[slots].p_next = p_timer;
            p_timer->p_next = p_tw_timer;
            p_tw_timer->p_prev = p_timer;
	}else{
            p_timer_header[slots].p_next = p_timer;
	}
    return 1;
}

int
tw_timer_del(struct tw_timer *p_tw_timer)
{
    if(!p_tw_timer){
        return 0;
    }
    if(p_tw_timer->p_prev == NULL){
        p_tw_timer->p_header->p_next = p_tw_timer->p_next;
        if(p_tw_timer->p_next != NULL){
            p_tw_timer->p_prev = NULL;
        }
    }else{
        p_tw_timer->p_prev->p_next = p_tw_timer->p_next;
        if(p_tw_timer->p_next != NULL){
            p_tw_timer->p_next->p_prev = p_tw_timer->p_prev; 
        }
    }
    free(p_tw_timer);
    p_tw_timer = NULL;
    return 1;
}

int
wt_timer_wheel(struct tw_timer_header *p_tw_header, int *p_solts )
{
    struct tw_timer_header tw_header = p_tw_header[*p_solts];
    struct tw_timer *p_tw_timer = tw_header.p_next;
    while(p_tw_timer){
        if(p_tw_timer->rotation > 0){
            p_tw_timer->rotation--;
        }else{
            tw_timer_del(p_tw_timer); 
        }
        p_tw_timer = p_tw_timer->p_next;
    }
    return 1;
}




