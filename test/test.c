#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define IP   "127.0.0.1"
#define PORT  8888
#define PROCESS_NUM 4
#define MAXEVENTS 64
int stop = 1;
void handle_term(int sig)
{
	stop = 0;

}

int main (int argc, char *argv[])
{
	int fd = socket(PF_INET, SOCK_STREAM, 0);
	int ret;
	if(fd == -1){
		perror("socket error\n");
		exit(1);
	}
	struct sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	inet_pton( AF_INET, IP, &serveraddr.sin_addr);  
	serveraddr.sin_port = htons(PORT);
	serveraddr.sin_addr.s_addr = INADDR_ANY;
	ret = bind(fd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if(ret == -1){
		perror("bind error\n");
	}
	ret = listen(fd, 1);
	if(ret == -1) perror("listen error\n");
	char buf[120] = {0};
	int conn = 0;
	while(stop){
	
		memset(buf, 0, 120);
		conn = accept(fd, NULL, NULL);
		if(conn > 0 ){
			printf("ok conn\n");
		}
		
		
		sleep(1);
	}
	close(fd);
	return 0;
}
