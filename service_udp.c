#include <stdio.h>  
#include <string.h>  
#include <errno.h>  
#include <sys/socket.h>  
#include <resolv.h>  
#include <stdlib.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <unistd.h>  
#include <sys/types.h>  
#include <sys/stat.h>  
#include <fcntl.h>  
#include <syslog.h>
#include <sys/time.h>
#include <signal.h>
#include <pthread.h>
#include "chainlinked/chain_linked.h"
#define PORT 8111
ChainList my_chain_list;


void sig_int(int num)
{
    printf("testover\n");
    exit(1);
};

void *fun_1(void *msg)
{
	struct timeval tv;
	int flag = 0;
	struct sockaddr_in s_addr;
	struct sockaddr_in c_addr;
	int sock_fd;
	socklen_t addr_len;
	int len;
	char buff[128];
	flag = create_chain_list(&my_chain_list);
	if(flag == 0 || flag == -1){
		printf("create chain_list error\n");
		exit(1);
	}


	if( (sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1  ){
		perror("create sock error");
	}else{
		printf("create sock_fd\n");
	}	
	memset(&s_addr, 0, sizeof(struct sockaddr_in));
	memset(&c_addr, 0, sizeof(struct sockaddr_in));
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(8111);
	s_addr.sin_addr.s_addr = INADDR_ANY;
	if( (bind(sock_fd, (struct sockaddr *) &s_addr, sizeof(s_addr))) == -1  ){
		perror("bind error");
	}
	printf("bind ok\n");
	addr_len = sizeof(c_addr);
	while(1){
		memset(&buff, 0, sizeof(buff));	
		len = recvfrom(sock_fd, buff, sizeof(buff) - 1, 0, (struct sockaddr *) &c_addr, &addr_len);
		if(len > 0 ){
			flag = in_chain_list(&my_chain_list, buff);
			if(flag == 0 || flag == -1){
				printf("in_chain_list error\n");
			}
			printf("buff = %s, time=%ld\n", buff, tv.tv_sec*1000000 + tv.tv_usec);
		}	
	}
	return NULL;

}

void *fun_2(void *msg)
{
	printf("msg2\n");
	sleep(1);
	return NULL;
}



int main()
{
	signal(SIGINT, sig_int);
	pthread_t thread_id_1, thread_id_2;
	pthread_create(&thread_id_1, NULL, (void *)fun_1, (void *)10);
	pthread_create(&thread_id_2, NULL, (void *)fun_2, (void *)20);
	while(1);	

	return 1;
}

