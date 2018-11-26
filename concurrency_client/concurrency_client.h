#ifndef CONCURRENCY_CLIENT_H
#define CONCURRENCY_CLIENT_H


enum STATES{ WAIT, WORK, STOPPING, STOP};
enum LOG_STATES{ OK_STATES, ERROR_STATES, EXPIRED_STATES };


struct process_data {
    pid_t pid;
    int pipe_fd[2];
    enum STATES state;
};


struct connect_data{
    int fd;
    char buf[1024];
    enum LOG_STATES state;
    int count;
    int beg_time;
    struct event_msg m_event_msg;
};

struct thread_args{
    int i;
    struct process_data *p_process_data;
};


#endif




