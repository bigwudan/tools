
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


#define IP "114.215.85.234"
#define PORT 80


char *p_2_request = "GET /index.php?id=%d HTTP/1.1\r\nHost: www.bigwudan.com\r\n\r\nxxxxxxxxxxx";
struct client{
	int fd;
	int count;
	char buf[1000];
};


int main()
{
	int file_fd = open("out.txt", O_CREAT|O_APPEND|O_RDWR);
        int count = 100;	
	char tmp_buf[80] = {0};
	int m_epollfd = epoll_create( 5 );
	struct epoll_event events[ 10000 ];
	struct client m_client[count];
	for(int i=0; i<count; i++){
		int tmp_sock = 0;	
		memset(tmp_buf, 0, sizeof(tmp_buf));
		snprintf(tmp_buf, sizeof(tmp_buf), p_2_request, i);
		struct sockaddr_in m_server_addr;
		bzero(&m_server_addr, sizeof(struct sockaddr_in));	
		m_server_addr.sin_family = AF_INET;
		inet_pton( AF_INET, IP, &m_server_addr.sin_addr);
		m_server_addr.sin_port = htons(PORT);
		tmp_sock = socket(PF_INET, SOCK_STREAM, 0);
		m_client[i].fd = tmp_sock;
		m_client[i].count = i;
		memset(m_client[i].buf, '\0', sizeof(m_client[i].buf));
		if ( connect( m_client[i].fd, ( struct sockaddr* )&m_server_addr, sizeof( m_server_addr ) ) < 0 ){
			printf("connect failed\n");
			exit(1);
		}
		int w_num = write(m_client[i].fd, tmp_buf, strlen(tmp_buf));
		struct epoll_event m_epoll_event;
		m_epoll_event.data.fd = m_client[i].fd;
		m_epoll_event.events = EPOLLIN| EPOLLET;
		epoll_ctl ( m_epollfd, EPOLL_CTL_ADD, m_client[i].fd, &m_epoll_event );
	}
	int number = 0;
	char tmp_rev[1000] = {0};
	while(1){
		number = epoll_wait( m_epollfd, events, 10000, -1 );
		for(int i=0; i < number; i++){
			if(  events[i].events & EPOLLIN  ){
				memset(tmp_rev, '\0', 1000);
				recv(events[i].data.fd,tmp_rev, 1000, 0 );
				if(strlen(tmp_rev) > 0){
					write(file_fd,  tmp_rev, strlen( tmp_rev));
				}
			}
		}
	}







}
