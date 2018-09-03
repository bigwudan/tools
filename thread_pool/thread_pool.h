#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>


#define MAX_NUM 2
enum
{
	thread_idle,
	thread_busy
}; 


typedef struct Task_node
{
	int arg;
	void *(*fun)(void *);
	pthread_t tid;
	int flag;
	struct Task_node *next;
	pthread_mutex_t mutex;
}Task_node;


typedef struct Task_queue_t
{
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	Task_node *head;
	int number; 
}Task_queue_t;


typedef struct Thread_node
{
	pthread_t tid;
	int flag;
	Task_node *work;
	struct Thread_node *next;
	struct Thread_node *prev;
	pthread_cond_t cond;
	pthread_mutex_t mutex;

}Thread_node;



typedef struct Pthread_queue_t
{
	int number;
	Thread_node *head;
	Thread_node *rear;
	pthread_cond_t cond;
	pthread_mutex_t mutex;
}Pthread_queue_t;







