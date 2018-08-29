#ifndef __TASK_POOL_H__
#define __TASK_POOL_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PTHREAD_MAX 2

typedef struct _task{
	pthread_t th;
	struct _task *p_next;
	int data;
}Task;

typedef struct _task_head{
	int task_num;
	int free_num;
	pthread_mutex_t mutex_t;
	pthread_cond_t cond_t;
	Task *p_next;

}Task_Head;

typedef struct _thread_pool{
	pthread_t th;
	int task_num;
	pthread_mutex_t mutex_t;


	struct _thread_pool *p_next;	
}Thread_Pool;

typedef struct _thread_pool_head
{
	pthread_mutex_t mutex_t;
	Thread_Pool *p_next;
}Thread_Pool_Head;

#endif


