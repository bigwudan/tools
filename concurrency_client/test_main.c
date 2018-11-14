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


#include "concurrency_client.h"
#include "config.h"
#include "event_process.h"
int m_epollfd;
int sig_pipefd[2];
int m_stop = 1;


//中断任务
void sig_handler( int sig )
{
    int save_errno = errno;
    int msg = sig;
    send( sig_pipefd[1], ( char* )&msg, 1, 0 );
    errno = save_errno;
}


int father_run(struct process_data *p_process_data)
{
    struct log_data log_data_list[CONCURRENCY_NUM];
    m_epollfd = epoll_create( 5 );
    assert( m_epollfd != -1 );
    int ret = socketpair( PF_UNIX, SOCK_STREAM, 0, sig_pipefd );
    assert( ret != -1 );
    setnonblocking( sig_pipefd[1] );
    struct event_msg m_event_msg;
    m_event_msg.fd = sig_pipefd[0];
    m_event_msg.m_event_type = SIG_FD;
    m_event_msg.count = 0;
    addfd( m_epollfd, &m_event_msg, EPOLLIN | EPOLLET);
    addsig( SIGCHLD, sig_handler, 1);
    addsig( SIGTERM, sig_handler, 1);
    addsig( SIGINT, sig_handler, 1);
    addsig( SIGPIPE, SIG_IGN, 1);
    struct event_msg m_1_event_msg;
    for(int i =0; i < PROCESS_NUM ; i++){
        m_1_event_msg.fd = p_process_data[i].pipe_fd[0];
        m_1_event_msg.count = i;
        m_1_event_msg.m_event_type = PIPE_FD;
        addfd(m_epollfd,&m_1_event_msg , EPOLLIN | EPOLLET); 
    }
    struct epoll_event events[ 10000 ];
    int number = 0;
    while(1);
//    while(m_stop == 1){
//        number = epoll_wait( m_epollfd, events, 10000, -1 );
//        if ( ( number < 0 ) && ( errno != EINTR ) )
//        {
//            printf( "epoll failure\n" );
//            break;
//        }
//        for(int i=0; i < number ; i++){
//            //接受fd 接受数据
//            
//
//
//
//        
//        
//        
//        }
//
//
//    
//    }
    printf("father \n");
}

int child_run()
{
    int file_fd = open("out.txt", O_CREAT|O_APPEND|O_RDWR);
    if(file_fd == -1){
    	printf("fd error\n");
	exit(1);
    }


    int concurrent_num = ceil(CONCURRENCY_NUM / PROCESS_NUM);
    concurrent_num = 30;
    m_epollfd = epoll_create( 5 );
    assert( m_epollfd != -1 );
  //  int ret = socketpair( PF_UNIX, SOCK_STREAM, 0, sig_pipefd );
  //  assert( ret != -1 );
  //  setnonblocking( sig_pipefd[1] );
    struct event_msg m_event_msg;
  //  m_event_msg.fd = sig_pipefd[0];
  //  m_event_msg.m_event_type = SIG_FD;
  //  m_event_msg.count = 0;
  //  addfd( m_epollfd, &m_event_msg, EPOLLIN | EPOLLET);
  //  addsig( SIGCHLD, sig_handler, 1);
  //  addsig( SIGTERM, sig_handler, 1);
  //  addsig( SIGINT, sig_handler, 1);
  //  addsig( SIGPIPE, SIG_IGN, 1);
    
    struct connect_data m_connect_data[concurrent_num];
    struct sockaddr_in server_address;
    bzero( &server_address, sizeof( server_address ) );
    server_address.sin_family = AF_INET;
    inet_pton( AF_INET, IP, &server_address.sin_addr );
    server_address.sin_port = htons(PORT);
    int sockfd = 0;
    for(int i=0; i <concurrent_num; i++){
            sockfd = socket( PF_INET, SOCK_STREAM, 0 );
            if(sockfd > 0 ){
                if ( connect( sockfd, ( struct sockaddr* )&server_address, sizeof( server_address ) ) < 0 )
                {
                    printf( "connection failed\n" );
                    printf("error = %d\n", errno);
                }
                //char buf[1200] = {0};
                write(sockfd, p_2_request, 69);
		//char buf[1200] = {0};
                //read(sockfd,buf, sizeof(buf) );
		//printf("buf=%s\n", buf);

                m_connect_data[i].fd = sockfd;
                m_connect_data[i].count = i;
                m_connect_data[i].state = WAIT;
                m_connect_data[i].beg_time = time(0);
                m_event_msg.count = i;
                m_event_msg.fd = sockfd;
                m_event_msg.m_event_type = SOCK_FD;
                addfd( m_epollfd, &m_event_msg, EPOLLIN | EPOLLET);
            }
    }
    struct event_msg *p_m_event_msg;
    struct epoll_event events[ 10000 ];
    int number = 0;        
    while(m_stop == 1){
        number = epoll_wait( m_epollfd, events, 10000, -1 );
		
	//printf("number=%d\n", number);
        if ( ( number < 0 ) && ( errno != EINTR ) )
        {
            printf( "epoll failure\n" );
            break;
        }
        for(int i=0; i < number ; i++){
            //接受fd 接受数据
            p_m_event_msg =  events[i].data.ptr;        
            //接受数据
            if( ( SOCK_FD == p_m_event_msg->m_event_type ) && ( events[i].events & EPOLLIN ) ){
                read(m_connect_data[i].fd, m_connect_data[i].buf, sizeof(m_connect_data[i].buf));             
		write(file_fd, m_connect_data[i].buf, strlen(m_connect_data[i].buf));

                //printf("buf=%s\n", m_connect_data[i].buf); 
            }
        }
    }
    exit(1);
    printf("children \n");
}



int main(int argc, char **argv)
{

   int tmp_num =10;
   
   for(int n =0 ; n < tmp_num; n++){
	pid_t tmp_pid=0;
   	tmp_pid = fork();
	if(tmp_pid == 0){
		child_run();
		break;
	}else{
		printf("father\n");
	}

   
   }

	while(1);

    return 1;
    int m_idx = -1;
    struct process_data process_data_list[PROCESS_NUM];
    for(int i=0; i<PROCESS_NUM ; i++){
        process_data_list[i].pid = 0;
        process_data_list[i].pipe_fd[0] = 0;
        process_data_list[i].pipe_fd[1] = 0;
        process_data_list[i].state = WAIT;
    
    }

    for(int i = 0; i < PROCESS_NUM-1 ; i++){
        pid_t t_pid = 0;
        int ret = socketpair( PF_UNIX, SOCK_STREAM, 0,process_data_list[i].pipe_fd );
        if(ret < 0) exit;
        t_pid = fork();
        if(t_pid == 0){
            //child
            m_idx = i;
            process_data_list[i].pid = t_pid;
            close(process_data_list[i].pipe_fd[0]);
            break;
        }else if(t_pid > 0 ){
            //father
            close(process_data_list[i].pipe_fd[1]);
            continue;
        }else{
            //error
            exit(0);
        }
    
    }

    if(m_idx == -1){
    
        //father
        father_run(process_data_list); 
    }else{
        //children
        child_run();

    }









    printf("test\n");
}
