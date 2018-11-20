#ifndef TIME_WHEEL_H
#define TIME_WHEEL_H


#define N 30
#define SI 2

struct tw_timer{
	int count;
	int timer;
	int rotation;
	struct tw_timer *p_next;
	struct tw_timer *p_prev;

};

struct tw_timer_header{
	struct tw_timer *p_next;
	int slots;

};


extern struct tw_timer_header *tw_timer_init();


#endif
