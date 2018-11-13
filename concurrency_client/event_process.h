#ifndef EVENT_PROCESS_H
#define EVENT_PROCESS_H

enum event_type{SOCK_FD, SIG_FD, PIPE_FD};


struct event_msg{
    int fd; //文件句柄
    enum event_type m_event_type ; 
    int count; 

};

extern int setnonblocking( int fd );
extern void addfd( int epollfd,  struct event_msg *p_event_msg, unsigned int events );
extern void removefd( int epollfd, int fd );

extern void addsig( int sig, void( handler )(int), int restart );



#endif

