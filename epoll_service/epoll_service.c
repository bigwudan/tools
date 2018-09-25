#include "epoll_service.h"
int m_epollfd;
int sig_pipefd[2];
int m_stop;
int m_listenfd;


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


void del_user(user *p_user)
{
    user *p_tmp_user = p_user->p_next_user;
	while(p_tmp_user){
	
		user *p_tmp = p_tmp_user;
		p_tmp_user = p_tmp_user->p_next_user;
		free(p_tmp);
		p_tmp = NULL;
			
	}	
	return ;





}

void 
add_user(user *p_user, int m_epollfd, int connfd, struct sockaddr_in *p_sockaddr)
{
    user *p_tmp_user = p_user;
    user *my_user = calloc(sizeof(user), 1);
    my_user->m_epollfd = m_epollfd;
    my_user->connfd = connfd;
    my_user->client_address = *p_sockaddr;
    while(p_tmp_user->p_next_user){
        p_tmp_user = p_tmp_user->p_next_user;
    }
    p_tmp_user->p_next_user = my_user;
    return ;
}

user * 
get_userbyfd(user *p_user, int connfd)
{
    user *p_tmp_user = p_user;
    while(p_tmp_user->p_next_user){
        if(p_tmp_user->p_next_user->connfd == connfd){
            return p_tmp_user->p_next_user;
        }
        p_tmp_user = p_tmp_user->p_next_user;
    }
    return NULL;
}

void 
run_child(process *m_sub_process, int m_idx)
{
    setup_sig_pipe();
    int pipefd = m_sub_process[m_idx].m_pipefd[ 1 ];
    addfd( m_epollfd, pipefd );
    struct epoll_event events[ MAX_EVENT_NUMBER ];
    int number = 0;
    int ret = -1;
    
    user my_user;


    while( ! m_stop )
    {
        number = epoll_wait( m_epollfd, events, MAX_EVENT_NUMBER, -1 );
        if ( ( number < 0 ) && ( errno != EINTR ) )
        {
            printf( "epoll failure\n" );
            break;
        }

        for ( int i = 0; i < number; i++ )
        {
            int sockfd = events[i].data.fd;
            if( ( sockfd == pipefd ) && ( events[i].events & EPOLLIN ) )
            {
                int client = 0;
                ret = recv( sockfd, ( char* )&client, sizeof( client ), 0 );
                if( ( ( ret < 0 ) && ( errno != EAGAIN ) ) || ret == 0 ) 
                {
                    continue;
                }
                else
                {
                    struct sockaddr_in client_address;
                    socklen_t client_addrlength = sizeof( client_address );
                    int connfd = accept( m_listenfd, ( struct sockaddr* )&client_address, &client_addrlength );
                    if ( connfd < 0 )
                    {
                        printf( "errno is: %d\n", errno );
                        continue;
                    }
                    addfd( m_epollfd, connfd );
                    add_user(&my_user, m_epollfd, connfd, &client_address);
                }
            }
            else if( ( sockfd == sig_pipefd[0] ) && ( events[i].events & EPOLLIN ) )
            {
                int sig;
                char signals[1024];
                ret = recv( sig_pipefd[0], signals, sizeof( signals ), 0 );
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
                                    pid_t pid;
                                    int stat;
                                    while ( ( pid = waitpid( -1, &stat, WNOHANG ) ) > 0 )
                                    {
                                        continue;
                                    }
                                    break;
                                }
                            case SIGTERM:
                            case SIGINT:
                                {
                                    m_stop = 1;
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
            else if( events[i].events & EPOLLIN )
            {
                user *tmp_user = NULL;
                tmp_user = get_userbyfd(&my_user, events[i].data.fd);


            }
            else
            {
                continue;
            }
        }
    }



    close( pipefd );
    close( m_listenfd );
    close( m_epollfd );

    printf("run_child\n");
}

void 
run_parent(process *m_sub_process, int m_idx)
{
	setup_sig_pipe();
	addfd( m_epollfd, m_listenfd );
	struct epoll_event events[ MAX_EVENT_NUMBER ];
	int sub_process_counter = 0;
	int new_conn = 1;
	int number = 0;
	int ret = -1;
	int m_process_number = 0;


	while( ! m_stop )
	{
		number = epoll_wait( m_epollfd, events, MAX_EVENT_NUMBER, -1 );
		if ( ( number < 0 ) && ( errno != EINTR ) )
		{
			printf( "epoll failure\n" );
			break;
		}

		for ( int i = 0; i < number; i++ )
		{
			int sockfd = events[i].data.fd;
			if( sockfd == m_listenfd )
			{
				int i =  sub_process_counter;
				do
				{
					if( m_sub_process[i].m_pid != -1 )
					{
						break;
					}
					i = (i+1)%m_process_number;
				}
				while( i != sub_process_counter );

				if( m_sub_process[i].m_pid == -1 )
				{
					m_stop = 1;
					break;
				}
				sub_process_counter = (i+1)%m_process_number;
				send( m_sub_process[i].m_pipefd[0], ( char* )&new_conn, sizeof( new_conn ), 0 );
				printf( "send request to child %d\n", i );
			}
			else if( ( sockfd == sig_pipefd[0] ) && ( events[i].events & EPOLLIN ) )
			{
				int sig;
				char signals[1024];
				ret = recv( sig_pipefd[0], signals, sizeof( signals ), 0 );
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
									pid_t pid;
									int stat;
									while ( ( pid = waitpid( -1, &stat, WNOHANG ) ) > 0 )
									{
										for( int i = 0; i < m_process_number; ++i )
										{
											if( m_sub_process[i].m_pid == pid )
											{
												printf( "child %d join\n", i );
												close( m_sub_process[i].m_pipefd[0] );
												m_sub_process[i].m_pid = -1;
											}
										}
									}
									m_stop = 1;
									for( int i = 0; i < m_process_number; ++i )
									{
										if( m_sub_process[i].m_pid != -1 )
										{
											m_stop = 0;
										}
									}
									break;
								}
							case SIGTERM:
							case SIGINT:
								{
									printf( "kill all the clild now\n" );
									for( int i = 0; i < m_process_number; ++i )
									{
										int pid = m_sub_process[i].m_pid;
										if( pid != -1 )
										{
											kill( pid, SIGTERM );
										}
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
			else
			{
				continue;
			}
		}
	}

	close( m_listenfd );
	close( m_epollfd );


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
        run_child(m_sub_process, m_idx);
        return;
    }
    run_parent(m_sub_process, m_idx);


    printf("ok\n");
    exit(1);
    return ;
}



int main(int argc, char *args[])
{
    
    processpool_run();

    return 1;

}
