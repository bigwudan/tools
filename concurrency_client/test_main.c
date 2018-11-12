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




#include "concurrency_client.h"
#include "config.h"
#include "event_process.h"
int m_epollfd;
int sig_pipefd[2];


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
    addfd( m_epollfd, sig_pipefd[0], EPOLLIN | EPOLLET);

    addsig( SIGCHLD, sig_handler, 1);
    addsig( SIGTERM, sig_handler, 1);
    addsig( SIGINT, sig_handler, 1);
    addsig( SIGPIPE, SIG_IGN, 1);

    for(int i =0; i < PROCESS_NUM ; i++){
        addfd(m_epollfd, p_process_data[i].pipe_fd[0], EPOLLIN | EPOLLET); 
    }


    struct epoll_event events[ 10000 ];







    printf("father \n");
}

int child_run()
{

    printf("children \n");
}



int main(int argc, char **argv)
{
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
