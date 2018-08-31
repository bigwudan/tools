#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#define MAX_NUM 5


enum
{
        thread_idle,
            thread_busy
} 


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



Pthread_queue_t *p_pthread_queue_idle = NULL;
Pthread_queue_t *p_pthread_queue_busy = NULL;
Task_queue_t *p_task_queue = NULL;


void *worker(void *arg)
{

        return NULL;

}

//初始化
//int sys_init()
//{
//  p_task_queue = calloc(sizeof(p_task_queue), 1);
//      if(p_task_queue == NULL){
//              perror("malloc error\n");exit(1);
//                  }
//                      p_task_queue->number = 0;
//                          p_task_queue->head = NULL;
//                              pthread_mutex_init(&p_task_queue->mutex);
//                                  pthread_cond_init(&p_task_queue->cond);
//                                      p_pthread_queue_idle = calloc(sizeof(Pthread_queue_t), 1);
//                                          if(p_task_queue == NULL){
//                                                  perror("malloc error\n");exit(1);
//                                                      }
//                                                          p_pthread_queue_idle->number = 0;
//                                                              p_pthread_queue_idle->head = NULL;
//                                                                  p_pthread_queue_idle->rear = NULL;
//                                                                      pthread_mutex_init(&p_pthread_queue_idle->mutex);
//                                                                          pthread_cond_init(&p_pthread_queue_idle->cond);
//                                                                              p_pthread_queue_busy = calloc(sizeof(Pthread_queue_t), 1);
//                                                                                  if(p_task_queue == NULL){
//                                                                                          perror("malloc error\n");exit(1);
//                                                                                              }
//                                                                                                  p_pthread_queue_busy->number = 0;
//                                                                                                      p_pthread_queue_busy->head = NULL;
//                                                                                                          p_pthread_queue_busy->rear = NULL;
//                                                                                                              pthread_mutex_init(&p_pthread_queue_busy->mutex);
//                                                                                                                  pthread_cond_init(&p_pthread_queue_busy->cond);
//
//                                                                                                                      Thread_node *p_thread_node = calloc(sizeof(Thread_node), MAX_NUM);
//
//                                                                                                                          if(p_thread_node == NULL){
//                                                                                                                                  perror("malloc error\n");
//                                                                                                                                          exit(1);
//
//                                                                                                                                              }
//                                                                                                                                                  for(int n=0; n <MAX_NUM; n++){
//                                                                                                                                                          if(n == 0){
//                                                                                                                                                                      p_thread_node[n]->next = p_thread_node[n+1];
//                                                                                                                                                                                  p_thread_node[n]->prev = NULL;
//                                                                                                                                                                                          }else{
//                                                                                                                                                                                                      p_thread_node[n]->next = p_thread_node[n+1];
//                                                                                                                                                                                                                  p_thread_node[n]->prev = p_thread_node[n-1];
//                                                                                                                                                                                                                          }           
//                                                                                                                                                                                                                                  p_thread_node[n]->tid = 0;
//                                                                                                                                                                                                                                          p_thread_node[n]->flag = thread_idle;
//                                                                                                                                                                                                                                                  p_thread_node[n]->work = NULL;
//                                                                                                                                                                                                                                                          p_thread_node[n]->next = p_thread_node[n+1];
//                                                                                                                                                                                                                                                                  p_thread_node[n]->prev = p_thread_node[n-1];
//                                                                                                                                                                                                                                                                          pthread_create(&p_thread_node[n]->tid, NULL, worker, (void*)p_thread_node[n]);
//                                                                                                                                                                                                                                                                                  pthread_mutex_init(&p_thread_node[n]->mutex);
//                                                                                                                                                                                                                                                                                          pthread_cond_init(&p_thread_node[n]->cond);
//                                                                                                                                                                                                                                                                                              }
//                                                                                                                                                                                                                                                                                                  p_pthread_queue_idle->number = MAX_NUM;
//                                                                                                                                                                                                                                                                                                      p_pthread_queue_idle->head = p_thread_node[0];
//                                                                                                                                                                                                                                                                                                          p_pthread_queue_idle->rear = p_thread_node[MAX_NUM];
//
//
//                                                                                                                                                                                                                                                                                                          }
//
//                                                                                                                                                                                                                                                                                                          int create_task()
//                                                                                                                                                                                                                                                                                                          {
//                                                                                                                                                                                                                                                                                                              return 1;
//
//                                                                                                                                                                                                                                                                                                              }
//
//                                                                                                                                                                                                                                                                                                              void *task_manage(void *arg){
//                                                                                                                                                                                                                                                                                                                  int data = 0;
//                                                                                                                                                                                                                                                                                                                      while(1){
//                                                                                                                                                                                                                                                                                                                              data = create_task();
//                                                                                                                                                                                                                                                                                                                                      Task_node *p_task_node = calloc(sizeof(Task_node), 1);
//                                                                                                                                                                                                                                                                                                                                              p_task_node->arg = data;
//                                                                                                                                                                                                                                                                                                                                                      p_task_node->tid = 0;
//                                                                                                                                                                                                                                                                                                                                                              p_task_node->flag = 0;
//                                                                                                                                                                                                                                                                                                                                                                      p_task_node->next = NULL;
//                                                                                                                                                                                                                                                                                                                                                                              pthread_mutex_init(&p_task_node->mutex);
//                                                                                                                                                                                                                                                                                                                                                                                      pthread_mutex_lock(&p_task_queue->mutex);
//                                                                                                                                                                                                                                                                                                                                                                                              if(p_task_queue->number == 0){
//                                                                                                                                                                                                                                                                                                                                                                                                          p_task_queue->head = p_task_node;
//                                                                                                                                                                                                                                                                                                                                                                                                                  }else{
//                                                                                                                                                                                                                                                                                                                                                                                                                              Task_node *p_tmp_task_node = p_task_queue->head;
//                                                                                                                                                                                                                                                                                                                                                                                                                                          p_task_queue->head = p_task_node;
//                                                                                                                                                                                                                                                                                                                                                                                                                                                      p_task_node->next = p_tmp_task_node;
//                                                                                                                                                                                                                                                                                                                                                                                                                                                              }
//                                                                                                                                                                                                                                                                                                                                                                                                                                                                      p_task_queue->number++;
//                                                                                                                                                                                                                                                                                                                                                                                                                                                                              pthread_cond_signal(&p_task_queue->cond);
//                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      pthread_mutex_unlock(&p_task_queue->mutex);
//                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          }
//
//                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          }
//
//
//
//
//
