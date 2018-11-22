#include <sys/mman.h> 
#include <sys/types.h> 
#include <fcntl.h> 
#include <unistd.h> 
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/socket.h>

#define MAX_EVENT_NUMBER 10000


static int setnonblocking( int fd )
{
    int old_option = fcntl( fd, F_GETFL );
    int new_option = old_option | O_NONBLOCK;
    fcntl( fd, F_SETFL, new_option );
    return old_option;
}

static void addfd( int epollfd, int fd )
{
    struct epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET;
    epoll_ctl( epollfd, EPOLL_CTL_ADD, fd, &event );
    setnonblocking( fd );
}

static int sig_pipefd[2];

static void sig_handler( int sig )
{
    printf("sig=%d\n", sig);
    int save_errno = errno;
    int msg = sig;
    send( sig_pipefd[1], ( char* )&msg, 1, 0 );
    errno = save_errno;
}


static void addsig( int sig, void( handler )(int))
{
    struct sigaction sa;
    memset( &sa, '\0', sizeof( sa ) );
    sa.sa_handler = handler;
    sa.sa_flags |= SA_RESTART;
    int flag = sigfillset( &sa.sa_mask );
    sigaction( sig, &sa, NULL );
}


int main(int argc, char **argv)
{

    int m_epollfd = epoll_create( 5 );
    int ret = socketpair( PF_UNIX, SOCK_STREAM, 0, sig_pipefd );
    if(ret == -1){
        perror("socketpair\n");
        exit(1);
    }



    setnonblocking( sig_pipefd[1] );
    addfd( m_epollfd, sig_pipefd[0] );



    addsig( SIGCHLD, sig_handler );
    addsig( SIGTERM, sig_handler );
    addsig( SIGINT, sig_handler ); 
    addsig( SIGPIPE, SIG_IGN ); 
    struct epoll_event events[ MAX_EVENT_NUMBER ];



    int number = 0;
    ret = -1;
    while(1){
    
        number = epoll_wait( m_epollfd, events, MAX_EVENT_NUMBER, -1 );
        if(number > 0){
            for(int i=0; i< number; i++){
                int sockfd = events[i].data.fd;
                if( ( sockfd == sig_pipefd[0] ) && ( events[i].events & EPOLLIN ) ){
                    printf("sockfd=%d\n",sockfd);
                    int sig;
                    char signals[1024];
                    ret = recv( sig_pipefd[0], signals, sizeof( signals ), 0 );
                    printf("ret=%d, signals[0]=%d , signals[1]=%d\n",ret, signals[0], signals[1]);
                    exit(1);



                }

            
            }

        } 
    
    }




    sleep(30);
    printf("test\n");

}

