#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <math.h>
#include <time.h>
#include <pthread.h>

int fds[2] = {0};
int child_run()
{
	sleep(2);
	printf("children run....\n");
	char buf[10] = {0};
	int n = recv(fds[1], buf, 10, 0);
	assert(n > 0);
	printf("rev=%s\n", buf);
	sleep(30);
	return 1;
}

int father_run()
{

	char buf[10] = "123";
	int n = send(fds[0], buf, 10, 0 );
	assert(n > 0 );

	printf("father run ....\n");
	sleep(30);
	return 1;
}


int main()
{
	
	int ret = socketpair( PF_UNIX, SOCK_STREAM, 0,fds);
	pid_t t_pid = 0;
	assert(ret != -1);
	 t_pid =  fork();	
	if(t_pid == 0){
		//child
		//close(fds[0]);
		child_run();
	
	}else if(t_pid > 0){
		//father
		//close(fds[1]);
		father_run();	
	}else{
		printf("fork error\n");
		exit(1);
	
	}



	printf("ok\n");	


	



}
