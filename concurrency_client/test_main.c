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

int father_run()
{
    struct log_data log_data_list[CONCURRENCY_NUM];
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
        father_run(); 
    }else{
        //children
        child_run();

    }









    printf("test\n");
}
