#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>

void *pthread_run1(void *arg)
{
    (void)arg;
    while(1){
        printf("I am thread1, ID:%lu\n", pthread_self());
        
        printf("test\n");

        sleep(2);    
    
    }



}


void* pthread_run2(void* arg)
{
    (void)arg;
    while(1)
    {
        printf("I am thread2,ID: %lu\n",pthread_self());
        sleep(2);
    }
}

int main()
{
    pthread_t tid1;
    pthread_t tid2;

    printf("I am main thread\n");

    pthread_create(&tid2, NULL, pthread_run2, NULL);
    pthread_create(&tid1, NULL, pthread_run1, NULL);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);




}



