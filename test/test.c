#include <stdio.h>
#include <unistd.h>
#include <sys/epoll.h>

int main(int argc, char *argv[])
{
    int epfd, nfds;
    struct epoll_event ev, events[5];
    epfd =epoll_create(1);
    ev.data.fd = STDIN_FILENO;
    ev.events =EPOLLIN;
    epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &ev);
    for(;;)
    {
        nfds = epoll_wait(epfd, events, 5, -1);
        for(int i =0; i <nfds; i++){
            if(events[i].data.fd == STDIN_FILENO){
                char buf[1024] = {0};
                read( events[i].data.fd,buf, sizeof(buf));
                printf("welcome to epoll's word!\n");
            }
        }
    }
    return 1;
}
