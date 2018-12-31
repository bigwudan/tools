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
#include <sys/mman.h>

#include "config.h"
#include "event_process.h"
#include "concurrency_client.h"
#include "parse_header_fsm.h"

int m_epollfd;
int sig_pipefd[2];
int m_stop = 1;
int m_idx = -1;
struct thread_node_head m_thread_node_head[THREAD_NUM];	
//int concurrent_num = ceil(CONCURRENCY_NUM / PROCESS_NUM);
int concurrent_num = 0;
int file_fd=0;
int log_fd = 0;
int log_num_my = 0;

char *log_p = NULL;
char *log_p_my = NULL;

struct connect_data *m_connect_data;
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
    int suc_num =0;
    m_epollfd = epoll_create( 5 );
    assert( m_epollfd != -1 );
    int ret = socketpair( PF_UNIX, SOCK_STREAM, 0, sig_pipefd );
    assert( ret != -1 );
    setnonblocking( sig_pipefd[1] );
    //初始化event_msg
	struct event_msg *p_m_event_msg;
	p_m_event_msg = (struct event_msg *)calloc(sizeof(struct event_msg), 1);
    p_m_event_msg->fd = sig_pipefd[0];
    p_m_event_msg->m_event_type = SIG_FD;
    p_m_event_msg->count = 0;
	struct epoll_event m_epoll_event;
	m_epoll_event.data.ptr = p_m_event_msg;
	m_epoll_event.events = EPOLLIN|EPOLLET;
	epoll_ctl(m_epollfd, EPOLL_CTL_ADD, sig_pipefd[0], &m_epoll_event);
    addsig( SIGCHLD, sig_handler, 1);
    addsig( SIGTERM, sig_handler, 1);
    addsig( SIGINT, sig_handler, 1);
    addsig( SIGPIPE, SIG_IGN, 1);
	//进程通信
	for(int i=0; i < PROCESS_NUM; i++){
		p_m_event_msg = (struct event_msg *)calloc(sizeof(struct event_msg), 1);
		setnonblocking( p_process_data[i].pipe_fd[0] );
		p_m_event_msg->fd = p_process_data[i].pipe_fd[0];
		p_m_event_msg->m_event_type = PIPE_FD;
		p_m_event_msg->count = i;
		memset(&m_epoll_event, 0, sizeof(struct epoll_event));
		m_epoll_event.data.ptr = p_m_event_msg;
		m_epoll_event.events = EPOLLIN|EPOLLET;
		epoll_ctl(m_epollfd, EPOLL_CTL_ADD, p_process_data[i].pipe_fd[0], &m_epoll_event);
	}
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
    		else if((p_m_event_msg->m_event_type== PIPE_FD)&&(events[i].events & EPOLLIN)){
    			char _buf[10] = {0};
    			int _n = recv(p_m_event_msg->fd, _buf, 10, 0);
    			if(_n > 0 ){
                    suc_num++;
    			}
    		}
        }
    }
    int log_num = LOG_CHAR_NUM * CONCURRENCY_NUM + 200;    
    munmap(log_p, log_num+1);
    printf("suc_num=%d\n", suc_num);
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
        struct thread_args *p_thread_args = (struct thread_args *)p_avg;
        int tmp_int = p_thread_args->i;
		int flag = m_thread_node_head[tmp_int].thread_count;
		struct thread_node *p_node = m_thread_node_head[tmp_int].p_thread_node;	
		if(p_node){
			pthread_mutex_lock(&(m_thread_node_head[tmp_int].m_mutex));
			int count= get_link_node(&m_thread_node_head[tmp_int]);
            char *tmp_log_p = NULL;
            tmp_log_p = log_p_my + log_num_my*LOG_CHAR_NUM;
            char *_t = memcpy(tmp_log_p, m_connect_data[count].buf, LOG_CHAR_NUM);

            assert(_t);

            log_num_my++;
			pthread_mutex_unlock(&(m_thread_node_head[tmp_int].m_mutex));
			//write(file_fd, m_connect_data[count].buf, sizeof(m_connect_data[count].buf));
            int _t_num = parse_run(m_connect_data[count].buf);
            if(_t_num == 0){
                char _buf[10] = {0};
                sprintf(_buf, "idx:%d", m_idx);
                int t = send((p_thread_args->p_process_data)[m_idx].pipe_fd[1], _buf, 10, 0);	
            }


		}
	}
	return NULL;
}

int child_run(struct process_data *p_process_data)
{
    log_p_my = log_p + m_idx*LOG_CHAR_NUM*concurrent_num;

	int tot_rev = 0;
	for(int i=0; i < THREAD_NUM; i++){
		m_thread_node_head[i].thread_count = i;
		m_thread_node_head[i].p_thread_node = NULL;
		pthread_mutex_init(&(m_thread_node_head[i].m_mutex), NULL);
	}	
	pthread_t m_tid[THREAD_NUM];
	for(int i=0; i < THREAD_NUM; i++){
        struct thread_args *p_thread_args = (struct thread_args *)calloc(1, sizeof(struct thread_args));
        p_thread_args->i = i;
        p_thread_args->p_process_data = p_process_data;
		pthread_create(&m_tid[i], NULL,fun_thread ,p_thread_args);
	}

    m_epollfd = epoll_create( 5 );
    assert( m_epollfd != -1 );
    int ret = socketpair( PF_UNIX, SOCK_STREAM, 0, sig_pipefd );
    assert( ret != -1 );
    setnonblocking( sig_pipefd[1] );

	struct event_msg *p_m_event_msg;
	p_m_event_msg = (struct event_msg *)calloc(sizeof(struct event_msg), 1);
    p_m_event_msg->fd = sig_pipefd[0];
    p_m_event_msg->m_event_type = SIG_FD;
    p_m_event_msg->count = 0;

	struct epoll_event m_epoll_event;
	m_epoll_event.data.ptr = p_m_event_msg;
	m_epoll_event.events = EPOLLIN|EPOLLET;
	epoll_ctl(m_epollfd, EPOLL_CTL_ADD, sig_pipefd[0], &m_epoll_event);
    addsig( SIGCHLD, sig_handler, 1);
    addsig( SIGTERM, sig_handler, 1);
    addsig( SIGINT, sig_handler, 1);
    addsig( SIGPIPE, SIG_IGN, 1);

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
				printf("idx=%d\n", m_idx);
                memset(tmp_buf, 0, sizeof(tmp_buf));
                snprintf(tmp_buf, sizeof(tmp_buf), p_2_request, i);
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
				epoll_ctl(m_epollfd, EPOLL_CTL_ADD, m_connect_data[i].fd, &m_epoll_event);
			}
    }
    struct epoll_event events[ 10000 ];
    int number = 0;        
    int flag_count = 0;
    while(m_stop == 1){
        flag_count++;
        number = epoll_wait( m_epollfd, events, 10000, -1 );
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
                        pthread_mutex_unlock(&(m_thread_node_head[thread_num].m_mutex));
                    }
                }
                //		recv(events[i].data.fd, tmp_rev, 1200, 0);
            }
			else if((p_m_event_msg->m_event_type== SIG_FD)&&(events[i].events & EPOLLIN)){
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
									printf("children sigchld \n");
									break;
								}
							case SIGTERM:
							case SIGINT:
								{
									printf("children sigint \n");
									m_stop = 0;
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
    printf("children over \n");
}

void
log_mmap()
{
    log_fd = open("test.txt", O_CREAT|O_RDWR|O_TRUNC);
    int log_num = LOG_CHAR_NUM * CONCURRENCY_NUM + 200;    
    int flag = lseek(log_fd, log_num, SEEK_SET);
    assert(flag > 0);
    write(log_fd, "",1);
    log_p = (char *)mmap(NULL, log_num+1, PROT_READ|PROT_WRITE, MAP_SHARED, log_fd, 0);
}


int main(int argc, char **argv)
{
    concurrent_num = ceil(CONCURRENCY_NUM / PROCESS_NUM);
	concurrent_num = 1;	
	m_connect_data = (struct connect_data *)calloc(concurrent_num, sizeof(struct connect_data ));
	
	assert(m_connect_data != NULL);

	int tmp_num =PROCESS_NUM ;
    log_mmap();
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
        }
    }

	if(m_idx == -1){
		//father
		father_run(process_data_list); 

	}else{
		//children
		child_run(process_data_list);
	}
    printf("fun over\n");
    return 1;
}
