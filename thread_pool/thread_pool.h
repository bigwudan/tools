#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#define MAX_NUM 5
enum
{
	thread_idle,
	thread_busy
}; 


typedef struct _task_node
{
	int arg;
	void *(*fun)(void *);
	pthread_t tid;
	int flag;
	struct _task_node *next;
	pthread_mutex_t mutex;
}Task_node;


typedef struct _task_queue_t
{
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	struct task_node *head;
	int number; 
}Task_queue_t;


typedef struct _thread_node
{
	pthread_t tid;
	int flag;
	struct _task_node *work;
	struct _pthread_node *next;
	struct _pthread_node *prev;
	pthread_cond_t cond;
	pthread_mutex_t mutex;

}Thread_node;



typedef struct _pthread_queue_t
{
	int number;
	struct _thread_node *head;
	struct _thread_node *rear;
	pthread_cond_t cond;
	pthread_mutex_t mutex;
}Pthread_queue_t;







