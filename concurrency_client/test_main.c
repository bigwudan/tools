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

#include "config.h"
#include "event_process.h"
#include "concurrency_client.h"
int m_epollfd;
int sig_pipefd[2];
int m_stop = 1;

struct thread_node_head m_thread_node_head[THREAD_NUM];	

//int concurrent_num = ceil(CONCURRENCY_NUM / PROCESS_NUM);
int concurrent_num = 4;

int file_fd=0;

struct connect_data m_connect_data[4];
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
   // struct log_data log_data_list[CONCURRENCY_NUM];
    m_epollfd = epoll_create( 5 );
    assert( m_epollfd != -1 );
    int ret = socketpair( PF_UNIX, SOCK_STREAM, 0, sig_pipefd );
    assert( ret != -1 );
    setnonblocking( sig_pipefd[1] );
    struct event_msg m_event_msg;
    m_event_msg.fd = sig_pipefd[0];
    m_event_msg.m_event_type = SIG_FD;
    m_event_msg.count = 0;

	struct epoll_event m_epoll_event;
	m_epoll_event.data.ptr = &(m_event_msg);
	m_epoll_event.events = EPOLLIN|EPOLLET;


	epoll_ctl(m_epollfd, EPOLL_CTL_ADD, sig_pipefd[0], &m_epoll_event);

    addfd( m_epollfd, &m_event_msg, EPOLLIN | EPOLLET);
    addsig( SIGCHLD, sig_handler, 1);
    addsig( SIGTERM, sig_handler, 1);
    addsig( SIGINT, sig_handler, 1);
    addsig( SIGPIPE, SIG_IGN, 1);

//    struct event_msg m_1_event_msg;
//    for(int i =0; i < PROCESS_NUM ; i++){
//        m_1_event_msg.fd = p_process_data[i].pipe_fd[0];
//        m_1_event_msg.count = i;
//        m_1_event_msg.m_event_type = PIPE_FD;
//        addfd(m_epollfd,&m_1_event_msg , EPOLLIN | EPOLLET); 
//    }
    struct epoll_event events[ 10000 ];
    int number = 0;
    while(m_stop == 1){
        number = epoll_wait( m_epollfd, events, 10000, -1 );
        if ( ( number < 0 ) && ( errno != EINTR ) )
        {
            printf( "epoll failure\n" );
            break;
        }
        for(int i=0; i < number ; i++){
            //接受fd 接受数据
            struct event_msg *p_m_event_msg =  events[i].data.ptr;        
            //接受数据
            if((p_m_event_msg->m_event_type== SIG_FD)&&(events[i].events & EPOLLIN)){
				int sig;
				char signals[1024];
				ret = recv( p_m_event_msg->fd, signals, sizeof( signals ), 0 );
				if( ret <= 0 )
				{
					continue;
				}
				else
				{
					for( int i = 0; i < ret; ++i )
					{
						switch( signals[i] )
						{
							case SIGCHLD:
								{
									pid_t t_pid;
									int stat;
									while((t_pid = waitpid(-1, &stat, WNOHANG)  ) >0   ){
										for(int i =0; i< PROCESS_NUM; i++){
											if(p_process_data[i].pid == t_pid){
												p_process_data[i].pid = -1;	
											}
										}
									
									}
									m_stop = 0;
									for(int i =0; i< PROCESS_NUM; i++){
										if(p_process_data[i].pid != -1 ){
											m_stop = 1;
										}
									}
									break;
								}
							case SIGTERM:
							case SIGINT:
								{
									for(int i=0; i < PROCESS_NUM; i++){
										kill( p_process_data[i].pid, SIGTERM);
									}
									break;
								}
							default:
								{
									break;
								}
						}
					}
				}
			}
        }
    }
	printf("father over\n");
	exit(1);
}

//inser linke
void insert_link_node(struct thread_node_head *p_head, int count)
{
	struct thread_node *m_thread_node = calloc(1, sizeof(struct thread_node));
	if(!m_thread_node) exit(1);
	m_thread_node->count = count;
	m_thread_node->p_next = NULL;
	struct thread_node *p_old_thread_node = p_head->p_thread_node;
	p_head->p_thread_node = m_thread_node;
	m_thread_node->p_next = p_old_thread_node;
}

//get link
int get_link_node(struct thread_node_head *p_head)
{
	struct thread_node *m_thread_node = p_head->p_thread_node;
	if(!m_thread_node){
		return -1;
	}
	int count = m_thread_node->count;
	p_head->p_thread_node = m_thread_node->p_next;
	free(m_thread_node);
	return count;
}

void* fun_thread(void *p_avg ){
	while(1){
		int tmp_int = (int)p_avg;
		int flag = m_thread_node_head[tmp_int].thread_count;
		struct thread_node *p_node = m_thread_node_head[tmp_int].p_thread_node;	
		if(p_node){
			pthread_mutex_lock(&(m_thread_node_head[tmp_int].m_mutex));
			int count= get_link_node(&m_thread_node_head[tmp_int]);
			//printf("count=%d, buf=%s\n", count, m_connect_data[count].buf);	
			write(file_fd, m_connect_data[count].buf, sizeof(m_connect_data[count].buf));
			pthread_mutex_unlock(&(m_thread_node_head[tmp_int].m_mutex));
		}
	}
	return NULL;
}

int child_run()
{
	return 1;
	int tot_rev = 0;
	for(int i=0; i < THREAD_NUM; i++){
		m_thread_node_head[i].thread_count = i;
		m_thread_node_head[i].p_thread_node = NULL;
		pthread_mutex_init(&(m_thread_node_head[i].m_mutex), NULL);
	}	
	
	pthread_t m_tid[THREAD_NUM];
	for(int i=0; i < THREAD_NUM; i++){
		pthread_create(&m_tid[i], NULL,fun_thread ,i);
	}
    file_fd = open("out.txt", O_CREAT|O_APPEND|O_RDWR);
    if(file_fd == -1){
    	printf("fd error\n");
	exit(1);
    }


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
    
    char tmp_buf[80] = {0};
    struct sockaddr_in server_address;
    bzero( &server_address, sizeof( server_address ) );
    server_address.sin_family = AF_INET;
    inet_pton( AF_INET, IP, &server_address.sin_addr );
    server_address.sin_port = htons(PORT);
    for(int i=0; i <concurrent_num; i++){
            m_connect_data[i].fd = socket( PF_INET, SOCK_STREAM, 0 );
            if(m_connect_data[i].fd> 0 ){
                if ( connect( m_connect_data[i].fd, ( struct sockaddr* )&server_address, sizeof( server_address ) ) < 0 )
                {
                    printf( "connection failed\n" );
                    printf("error = %d\n", errno);
                }
                memset(tmp_buf, 0, sizeof(tmp_buf));
                snprintf(tmp_buf, sizeof(tmp_buf), p_2_request, i);
                //printf("tmp=%s\n", tmp_buf);
                write(m_connect_data[i].fd, tmp_buf, strlen(tmp_buf));
                m_connect_data[i].count = i;
                m_connect_data[i].state = WAIT;
                m_connect_data[i].beg_time = time(0);
                m_connect_data[i].m_event_msg.fd = m_connect_data[i].fd;
				m_connect_data[i].m_event_msg.count = i;
				m_connect_data[i].m_event_msg.m_event_type = SOCK_FD;
				struct epoll_event m_epoll_event;
				m_epoll_event.data.ptr = &(m_connect_data[i].m_event_msg);
				m_epoll_event.events = EPOLLIN|EPOLLET;
				//addfd( m_epollfd, &m_event_msg, EPOLLIN | EPOLLET);
				epoll_ctl(m_epollfd, EPOLL_CTL_ADD, m_connect_data[i].fd, &m_epoll_event);
			}
    }
    struct event_msg *p_m_event_msg;
    struct epoll_event events[ 10000 ];
    int number = 0;        
    int flag_count = 0;
    while(m_stop == 1){
        flag_count++;
        number = epoll_wait( m_epollfd, events, 10000, -1 );
	    printf("number=%d\n", number);	
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
            if((p_m_event_msg->m_event_type== SOCK_FD)&&(events[i].events & EPOLLIN)){
                while(1){
                    char tmp_rev[1200] = {0};
                    int recv_num= recv(p_m_event_msg->fd, tmp_rev, 1200, 0);
                    if(recv_num < 0 ){
                        if( ( errno == EAGAIN ) || ( errno == EWOULDBLOCK ) ){
                            break;
                        }
                        close(p_m_event_msg->fd);		
                        break;	
                    }else if(recv_num == 0){
                        close(p_m_event_msg->fd);
                    }else{
                        int thread_num = tot_rev%THREAD_NUM; 
                        tot_rev++;
                        pthread_mutex_lock(&(m_thread_node_head[thread_num].m_mutex));
                        insert_link_node(&(m_thread_node_head[thread_num]), p_m_event_msg->count );
                        strncpy((m_connect_data[p_m_event_msg->count]).buf, tmp_rev, strlen(tmp_rev));
                        //printf("tmp_rev=%s\n", tmp_rev);
                        pthread_mutex_unlock(&(m_thread_node_head[thread_num].m_mutex));
                    }
                }
                //		recv(events[i].data.fd, tmp_rev, 1200, 0);
            }
        }

    }
    exit(1);
    printf("children \n");
}



int main(int argc, char **argv)
{
	int tmp_num =PROCESS_NUM ;
	int m_idx = -1;
    struct process_data process_data_list[PROCESS_NUM];
    for(int i=0; i<PROCESS_NUM ; i++){
        process_data_list[i].pid = 0;
        process_data_list[i].pipe_fd[0] = 0;
        process_data_list[i].pipe_fd[1] = 0;
        process_data_list[i].state = WAIT;
    
    }

    for(int i = 0; i < PROCESS_NUM ; i++){
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
            process_data_list[i].pid = t_pid;
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

	while(1);

    return 1;


    printf("test\n");
}
