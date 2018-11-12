#ifndef CONCURRENCY_CLIENT_H
#define CONCURRENCY_CLIENT_H


enum STATES{ WAIT, WORK, STOPPING, STOP};
enum LOG_STATES{ OK_STATES, ERROR_STATES, EXPIRED_STATES };


struct process_data {
    pid_t pid;
    int pipe_fd[2];
    enum STATES state;
};

struct log_data
{
    int fd;
    pid_t my_pid_t;
    char buf[1024];
    enum LOG_STATES state;


};




#endif




