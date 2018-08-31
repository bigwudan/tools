#include "thread_pool.h"
Pthread_queue_t *p_pthread_queue_idle = NULL;
Pthread_queue_t *p_pthread_queue_busy = NULL;
Task_queue_t *p_task_queue = NULL;

void *worker(void *arg)
{
	return NULL;
}

//初始化
int sys_init()
{
	p_task_queue = calloc(sizeof(p_task_queue), 1);
	if(p_task_queue == NULL){
		perror("malloc error\n");exit(1);
	}
	p_task_queue->number = 0;
	p_task_queue->head = NULL;
	pthread_mutex_init(&p_task_queue->mutex);
	pthread_cond_init(&p_task_queue->cond);
	p_pthread_queue_idle = calloc(sizeof(Pthread_queue_t), 1);
	if(p_task_queue == NULL){
		perror("malloc error\n");exit(1);
	}
	p_pthread_queue_idle->number = 0;
	p_pthread_queue_idle->head = NULL;
	p_pthread_queue_idle->rear = NULL;
	pthread_mutex_init(&p_pthread_queue_idle->mutex);
	pthread_cond_init(&p_pthread_queue_idle->cond);
	p_pthread_queue_busy = calloc(sizeof(Pthread_queue_t), 1);
	if(p_task_queue == NULL){
		perror("malloc error\n");exit(1);
	}
	p_pthread_queue_busy->number = 0;
	p_pthread_queue_busy->head = NULL;
	p_pthread_queue_busy->rear = NULL;
	pthread_mutex_init(&p_pthread_queue_busy->mutex);
	pthread_cond_init(&p_pthread_queue_busy->cond);

	Thread_node *p_thread_node = calloc(sizeof(Thread_node), MAX_NUM);

	if(p_thread_node == NULL){
		perror("malloc error\n");
		exit(1);

	}
	for(int n=0; n <MAX_NUM; n++){
		if(n == 0){
			p_thread_node[n]->next = p_thread_node[n+1];
			p_thread_node[n]->prev = NULL;
		}else{
			p_thread_node[n]->next = p_thread_node[n+1];
			p_thread_node[n]->prev = p_thread_node[n-1];
		}			
		p_thread_node[n]->tid = 0;
		p_thread_node[n]->flag = thread_idle;
		p_thread_node[n]->work = NULL;
		p_thread_node[n]->next = p_thread_node[n+1];
		p_thread_node[n]->prev = p_thread_node[n-1];
		pthread_create(&p_thread_node[n]->tid, NULL, worker, (void*)p_thread_node[n]);
		pthread_mutex_init(&p_thread_node[n]->mutex);
		pthread_cond_init(&p_thread_node[n]->cond);
	}
	p_pthread_queue_idle->number = MAX_NUM;
	p_pthread_queue_idle->head = p_thread_node[0];
	p_pthread_queue_idle->rear = p_thread_node[MAX_NUM];


}



int main(int argc, char **argv )
{
	sys_init();
	printf("test\n");

}
