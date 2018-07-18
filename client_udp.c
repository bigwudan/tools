#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
#define SERVICE "192.168.1.109"
#define PORT 8111
int main()
{
	struct sockaddr_in s_addr;
	int sock;
	int addr_len;
	int len;	
	char buff[10000];
	char write_buff[20];

	int flag = 0;
	int count = 0;
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock == -1){
		perror("socket error");
	}else{
		printf("socket ok\n");
	}		
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(PORT);
	s_addr.sin_addr.s_addr = inet_addr(SERVICE);
	addr_len = sizeof(s_addr);
	while(1){

		memset(buff, 0, sizeof(buff));
		//sprintf(buff, "%d", count);
		snprintf(buff, 10000, "%d", count);


		len = sendto(sock, buff, strlen(buff), 0, (struct sockaddr *) &s_addr, addr_len);
		if(len > 0){

			memset(write_buff, 0, sizeof(write_buff));
			sprintf(write_buff, "%s\n", buff);
			FILE *fd;
			fd = fopen("test.txt", "a+");
			fputs(write_buff, fd);
			fclose(fd);




			printf("send ok\n");
		}else{
			printf("send error\n");
		}


	}
	

	close(sock);
}
