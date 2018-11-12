#ifndef EVENT_PROCESS_H
#define EVENT_PROCESS_H


extern int setnonblocking( int fd );
extern void addfd( int epollfd, int fd, unsigned int events );
extern void removefd( int epollfd, int fd );

extern void addsig( int sig, void( handler )(int), int restart );
#endif

