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
	pthread_mutex_init(&p_task_queue->mutex, NULL);
	pthread_cond_init(&p_task_queue->cond, NULL);
	p_pthread_queue_idle = calloc(sizeof(Pthread_queue_t), 1);
	if(p_task_queue == NULL){
		perror("malloc error\n");exit(1);
	}
	p_pthread_queue_idle->number = 0;
	p_pthread_queue_idle->head = NULL;
	p_pthread_queue_idle->rear = NULL;
	pthread_mutex_init(&p_pthread_queue_idle->mutex, NULL);
	pthread_cond_init(&p_pthread_queue_idle->cond, NULL);
	p_pthread_queue_busy = calloc(sizeof(Pthread_queue_t), 1);
	if(p_task_queue == NULL){
		perror("malloc error\n");exit(1);
	}
	p_pthread_queue_busy->number = 0;
	p_pthread_queue_busy->head = NULL;
	p_pthread_queue_busy->rear = NULL;
	pthread_mutex_init(&p_pthread_queue_busy->mutex, NULL);
	pthread_cond_init(&p_pthread_queue_busy->cond, NULL);

	Thread_node *p_thread_node = (Thread_node * )calloc(sizeof(Thread_node), MAX_NUM);

	if(p_thread_node == NULL){
		perror("malloc error\n");
		exit(1);

	}
	for(int n=0; n <MAX_NUM; n++){
		if(n == 0){
			p_thread_node[n].next = &p_thread_node[n+1];
			p_thread_node[n].prev = NULL;
		}else{
			p_thread_node[n].next = &p_thread_node[n+1];
			p_thread_node[n].prev = &p_thread_node[n-1];
		}			
		p_thread_node[n].tid = 0;
		p_thread_node[n].flag = thread_idle;
		p_thread_node[n].work = NULL;
		p_thread_node[n].next = &p_thread_node[n+1];
		p_thread_node[n].prev = &p_thread_node[n-1];
		pthread_create(&p_thread_node[n].tid, NULL, worker, (void*)&p_thread_node[n]);
		pthread_mutex_init(&p_thread_node[n].mutex, NULL);
		pthread_cond_init(&p_thread_node[n].cond, NULL);
	}
	p_pthread_queue_idle->number = MAX_NUM;
	p_pthread_queue_idle->head = &p_thread_node[0];
	p_pthread_queue_idle->rear = &p_thread_node[MAX_NUM];


}


int create_task()
{
	return 1;

}

void *task_manage(void *arg){
	int data = 0;
	while(1){
		data = create_task();
		Task_node *p_task_node = calloc(sizeof(Task_node), 1);
		p_task_node->arg = data;
		p_task_node->tid = 0;
		p_task_node->flag = 0;
		p_task_node->next = NULL;
		pthread_mutex_init(&p_task_node->mutex, NULL);
		pthread_mutex_lock(&p_task_queue->mutex);
		if(p_task_queue->number == 0){
			p_task_queue->head = p_task_node;
		}else{
			Task_node *p_tmp_task_node = p_task_queue->head;
			p_task_queue->head = p_task_node;
			p_task_node->next = p_tmp_task_node;
		}
		p_task_queue->number++;
		pthread_cond_signal(&p_task_queue->cond);
		pthread_mutex_unlock(&p_task_queue->mutex);
	}

}

void *pthread_manage(void *arg)
{

	while(1){
		pthread_mutex_lock(&p_task_queue->mutex);
		while( p_task_queue->number <= 0  ){
			pthread_cond_wait(&p_task_queue->cond, &p_task_queue->mutex  );
		}
		Task_node *p_task_node = NULL; 
		Task_node *p_pre_task_node = NULL;
		p_task_node = p_task_queue->head;
		if( p_task_queue->number == 1 ){
			p_task_queue->head = NULL;
		}else{
			while(p_task_node){
				if(p_task_node->next == NULL){
					p_pre_task_node->next = NULL;	
					break;
				}
				p_pre_task_node = p_task_node;
				p_task_node = p_task_node->next;
			}
		}
		p_task_queue->number--;
		pthread_mutex_unlock( &p_task_queue->mutex );
		
		pthread_mutex_lock( &p_pthread_queue_idle->mutex );


		while( p_pthread_queue_idle->number <=0  ){
			pthread_cond_wait( &p_pthread_queue_idle->cond, &p_pthread_queue_idle->mutex );	

		}

		Thread_node *p_thread_node = NULL;
		Thread_node *p_pre_thread_node = NULL;
		p_thread_node  = p_pthread_queue_idle->head;

		while( p_thread_node ){
			if( p_thread_node->next == NULL ){
				
				p_pre_thread_node->next = NULL;
			
			}	
			p_pre_thread_node = p_thread_node;
			p_thread_node = p_thread_node->next;
		
		
		}	
		p_pthread_queue_idle->number--;		
		pthread_mutex_lock( &p_task_node->mutex );	
		p_task_node->tid = p_thread_node->tid;
		p_task_node->flag = 1;
		p_task_node->next = NULL;
		pthread_mutex_unlock(&p_task_node->mutex);
		pthread_mutex_lock(&p_thread_node->mutex);
		p_thread_node->work = p_task_node;
		pthread_cond_signal(&p_thread_node->cond);
		pthread_mutex_unlock(&p_thread_node->mutex);






	
	
	}





}


int main(int argc, char **argv )
{
	sys_init();
	printf("test\n");

}
