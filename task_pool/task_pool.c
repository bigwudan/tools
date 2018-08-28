#include "task_pool.h"


Task_Head my_task_head;

Thread_Pool_Head my_thread_pool_head;

void * thread_work( void *arg)
{


	Thread_Pool *p_pool = (Thread_Pool *)arg;	

	printf("start thread work pthread_t = %ld  th=%ld \n", pthread_self(), p_pool->th );

	sleep(5);

	return NULL;
}


void init()
{
	my_task_head.task_num = 0;
	my_task_head.free_num = 0;
	my_task_head.p_next = NULL;

	pthread_mutex_init(&my_task_head.mutex_t, NULL);
	pthread_cond_init(&my_task_head.cond_t, NULL);


	pthread_mutex_init(&my_thread_pool_head.mutex_t, NULL);
	my_thread_pool_head.p_next = NULL;
	

	Thread_Pool *old_thread_pool = NULL;


	int i=0;
	for(i=0; i < PTHREAD_MAX; i++  ){
		Thread_Pool  *my_thread_pool = calloc(sizeof( Thread_Pool ), 1);	
		my_thread_pool->task_num = 0;
		pthread_mutex_init( &my_thread_pool->mutex_t, NULL );
		my_thread_pool->p_next = NULL;
		pthread_create( &my_thread_pool->th, NULL, thread_work,  (void *)my_thread_pool );	
		if(i == 0 ){
			my_thread_pool_head.p_next = my_thread_pool;

		}else{
			old_thread_pool->p_next = my_thread_pool;	

		}	
		old_thread_pool = my_thread_pool;
	}
	


	return; 
}


int create_task()
{
	return 1;

}


void *get_task(void *arg )
{
	int data = 0;

	while(1){
		while(1){
			if( data = create_task() ){
				break;	
			}		
		}
		Task *p_my_task = (Task *)calloc(sizeof(Task), 1);	
		p_my_task->data = data;
		p_my_task->th = 0;
		p_my_task->p_next = NULL;
		pthread_mutex_lock(&my_task_head.mutex_t  );
		Task *p_task = NULL;
		p_task =  my_task_head.p_next;
		my_task_head.p_next = p_my_task;
		p_my_task->p_next = p_task;
		my_task_head.task_num++;
		my_task_head.free_num++;
		pthread_cond_signal(&my_task_head.cond_t);
		pthread_mutex_unlock(&my_task_head.mutex_t);

		printf("create task over\n");
		sleep(10);
	}

}

void * allocate_task(void *arg)
{
	while(1){
		pthread_mutex_lock(&my_task_head.mutex_t);
		while( my_task_head.free_num <= 0  ){
			pthread_cond_wait(&my_task_head.cond_t, &my_task_head.mutex_t  );
		}
		Task *p_my_task = NULL;	
		p_my_task = my_task_head.p_next;
		while(p_my_task){
			if(p_my_task->th == 0){
				break;
			}	
			p_my_task = p_my_task->p_next;	
		}
		my_task_head.free_num--;
		Thread_Pool *my_thread_pool = NULL;
		my_thread_pool = my_thread_pool_head.p_next;
		while(  my_thread_pool ){
			pthread_mutex_lock( &my_thread_pool->mutex_t  );
			if( my_thread_pool->task_num < PTHREAD_MAX  ){
				break;
			}
			pthread_mutex_unlock( &my_thread_pool->mutex_t );
		}	
		p_my_task->th = my_thread_pool->th;
		my_thread_pool->task_num++;
		pthread_mutex_unlock( &my_thread_pool->mutex_t );
		pthread_mutex_unlock(  &my_task_head.mutex_t );
	
	}	


}



int main(int argc, char **argv)
{
	pthread_t get_task_th;
	pthread_t assemb_task_th;
	init();	
	//allocate_task(NULL);
	//get_task(NULL );
	pthread_create(&get_task_th, NULL ,get_task, NULL  );
	pthread_create(&assemb_task_th, NULL ,allocate_task, NULL  );
	while(1);

	printf("test\n");
	return 1;
}


