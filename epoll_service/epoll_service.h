#ifndef PROCESSPOOL_H
#define PROCESSPOOL_H
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

#define PROCESS_NUMBER 8

typedef struct process
{
    pid_t m_pid;
    int m_pipefd[2];
}process;



void processpool_run();
void run_child();

void run_parent();

void setup_sig_pipe();


















#endif
