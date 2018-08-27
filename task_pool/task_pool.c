#include "task_pool.h"


Task_Head my_task_head;

Thread_Pool_Head my_thread_pool_head;

int create_task()
{

	return 1;

}


void *get_task( void *arg)
{
	int data = 0;
	while(1){
		data = create_task();
		Task *my_task =   (Task *)calloc(sizeof(Task), 1 );
		my_task->data = data;
		my_task->th = 0;

		pthread_mutex_lock(&my_task_head.mutex_t);
		Task *p_task =  my_task_head.p_next;
		if(p_task == NULL){
			my_task_head.p_next	 = my_task;
		}else{
			my_task_head.p_next = my_task;
			my_task->p_next = p_task;
		}
		my_task_head.task_num++;
		my_task_head.free_num++;	
		pthread_cond_signal(&my_task_head.cond_t);
		pthread_mutex_unlock(&my_task_head.mutex_t);
	}
	return NULL;
}


void *assemble_task( void *arg  )
{
	while(1){
		pthread_mutex_lock(&my_task_head.mutex_t);	
		while( my_task_head.free_num  == 0 ){
			pthread_cond_wait(&my_task_head.cond_t, &my_task_head.mutex_t);	
		}
		//寻找未分配的任务	
		Task *p_task = my_task_head.p_next;
		while( p_task ){
			if(p_task->th == 0){
				break;	
			}	
			p_task = p_task->p_next;
		}	
		my_task_head.free_num--;
		Thread_Pool *p_thread_pool = my_thread_pool_head.p_next;
		while( p_thread_pool  ){
			pthread_mutex_lock(&p_thread_pool->mutex_t);	
			if(p_thread_pool->task_num <5){
			
				p_task->th = p_thread_pool->th;
				break;
			}
			p_thread_pool->task_num++;
			pthread_mutex_unlock(&p_thread_pool->mutex_t);
			p_thread_pool = p_thread_pool->p_next;
		}	
	}
	return NULL;
}

void *process_pthread( void *arg)
{

	pthread_t th =  pthread_self();
	Thread_Pool *p_thread_pool = my_thread_pool_head.p_next;
	while( p_thread_pool ){
		if( p_thread_pool->th == th ){
			break;
		}	
	}
	while(1){
		pthread_mutex_lock( &p_thread_pool->mutex_t );		
		while( p_thread_pool->task_num>0  ){
				
		
		}




		
	
	}





	return NULL;

}




