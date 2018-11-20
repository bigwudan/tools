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
	p_timer->p_next = NULL;
	p_timer->p_prev = NULL;

	if(p_timer_header[slots].p_next){
	
	}else{
		
	
	}
	






}


