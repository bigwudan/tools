#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>

#define NTY_POOL_WAIT           0x01
#define NTY_POOL_DESTROY        0x02

int main()
{
    int flag = 1;
    //int a = flag & NTY_POOL_DESTROY;
    flag &= ~NTY_POOL_WAIT;



    printf("a=%d\n", flag);


    
}
