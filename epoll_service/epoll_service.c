#include "epoll_service.h"




int m_epollfd;
int sig_pipefd[2];

static void sig_handler( int sig )
{
    int save_errno = errno;
    int msg = sig;
    send( sig_pipefd[1], ( char* )&msg, 1, 0 );
    errno = save_errno;
}


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

static void addsig( int sig, void( handler )(int), int restart  )
{
    struct sigaction sa;
    memset( &sa, '\0', sizeof( sa ) );
    sa.sa_handler = handler;
    if( restart )
    {
        sa.sa_flags |= SA_RESTART;
    }
    sigfillset( &sa.sa_mask );
    assert( sigaction( sig, &sa, NULL ) != -1 );
}


void
setup_sig_pipe()
{
    m_epollfd = epoll_create( 5 );
    assert( m_epollfd != -1 );

    int ret = socketpair( PF_UNIX, SOCK_STREAM, 0, sig_pipefd );
    assert( ret != -1 );

    setnonblocking( sig_pipefd[1] );
    addfd( m_epollfd, sig_pipefd[0] );

    addsig( SIGCHLD, sig_handler, 1 );
    addsig( SIGTERM, sig_handler, 1 );
    addsig( SIGINT, sig_handler, 1 );
    addsig( SIGPIPE, SIG_IGN, 1 );

}










void 
run_child()
{

    printf("run_child\n");
}

void 
run_parent()
{
    int wstatus = 0;
    printf("run_parent");
    wait(&wstatus);
}

void 
processpool_run()
{
    int m_idx = -1;


    process *m_sub_process = calloc(sizeof(process), 1);
    if(!m_sub_process){
        perror("error calloc \n");
    }
    for( int i = 0; i < PROCESS_NUMBER; ++i )
    {
        int ret = socketpair( PF_UNIX, SOCK_STREAM, 0, m_sub_process[i].m_pipefd );
        assert( ret == 0 );

        m_sub_process[i].m_pid = fork();
        assert( m_sub_process[i].m_pid >= 0 );
        if( m_sub_process[i].m_pid > 0 )
        {
            close( m_sub_process[i].m_pipefd[1] );
            continue;
        }
        else
        {
            close( m_sub_process[i].m_pipefd[0] );
            m_idx = i;
            break;
        }
    }

    if( m_idx != -1 )
    {
        run_child();
        return;
    }
    run_parent();


    printf("ok\n");
    exit(1);
    return ;
}



int main(int argc, char *args[])
{
    
    processpool_run();

    return 1;

}
