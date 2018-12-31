#include <sys/mman.h> 
#include <sys/types.h> 
#include <fcntl.h> 
#include <unistd.h> 
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <assert.h>

#define M_CHAR 10
#define PROCESS_NUM 10
int *a = NULL;

int
main()
{
	a = malloc(sizeof(int)*10);

	a[0] = 1;
	a[1] = 10;

	printf("testi len=%d\n", a[0]);
}
