#include "epoll_service.h"





void
setup_sig_pipe()
{
    m_epollfd = epoll_create( 5 );
    assert( m_epollfd != -1 );

    int ret = socketpair( PF_UNIX, SOCK_STREAM, 0, sig_pipefd );
    assert( ret != -1 );

    setnonblocking( sig_pipefd[1] );
    addfd( m_epollfd, sig_pipefd[0] );

    addsig( SIGCHLD, sig_handler );
    addsig( SIGTERM, sig_handler );
    addsig( SIGINT, sig_handler );
    addsig( SIGPIPE, SIG_IGN );

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
