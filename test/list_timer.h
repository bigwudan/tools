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


#define BUFFER_SIZE 100

typedef struct util_timer util_timer;

typedef struct client_data

{
    int sock_fd;
    struct sockaddr_in address;
    char buf[BUFFER_SIZE];
    util_timer *p_util_timer;

}client_data;

struct util_timer
{
    util_timer *prev;
    util_timer *next;
    int expire;
    client_data *p_client_data;
    void (*cb_fun)(void *);

};

typedef struct sort_timer_list
{
    util_timer *head;
    util_timer *tail;


}sort_timer_list;



