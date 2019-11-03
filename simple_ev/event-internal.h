#ifndef _EVENT_INTERNAL_H_
#define _EVENT_INTERNAL_H_


#include "min_heap.h"

//事件驱动配置
struct eventop {
    const char *name;
    void *(*init)(struct event_base *);
    int (*add)(void *, struct event *);
    int (*del)(void *, struct event *);
    int (*dispatch)(struct event_base *, void *, struct timeval *);
    void (*dealloc)(struct event_base *, void *);
    int need_reinit;
};
//基础事件
struct event_base {
    const struct eventop *evsel;
    void *evbase;
    int event_count;        /* counts number of total events */
    int event_count_active; /* counts number of active events */

    int event_gotterm;      /* Set to terminate loop */
    int event_break;        /* Set to terminate loop immediately */

    /* 激活事件 */
    struct event_list activequeues;

    /* 事件链表 */

    struct event_list eventqueue;

    struct min_heap timeheap;

};








#endif
