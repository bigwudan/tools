#ifndef _EVENT_H_
#define _EVENT_H_

#include "queue.h"
#include <stdint.h>
#include <sys/time.h>

#define EVLOOP_ONCE 0x01    /**< Block at most once. */
#define EVLOOP_NONBLOCK 0x02    /**< Do not block. */


#define EVLIST_TIMEOUT	0x01
#define EVLIST_INSERTED	0x02
#define EVLIST_SIGNAL	0x04
#define EVLIST_ACTIVE	0x08
#define EVLIST_INTERNAL	0x10
#define EVLIST_INIT	0x80

/* EVLIST_X_ Private space: 0x1000-0xf000 */
#define EVLIST_ALL	(0xf000 | 0x9f)

#define EV_TIMEOUT	0x01
#define EV_READ		0x02
#define EV_WRITE	0x04
#define EV_SIGNAL	0x08
#define EV_PERSIST	0x10	/* Persistant event */

struct event {
    TAILQ_ENTRY (event) ev_next;
    TAILQ_ENTRY (event) ev_active_next;
    TAILQ_ENTRY (event) ev_signal_next;
    uint32_t min_heap_idx;  /* for managing timeouts */

    struct event_base *ev_base;

    uint32_t ev_fd;
    uint16_t ev_events;
    
    //超时
    struct timeval ev_timeout;


    void (*ev_callback)(int, short, void *arg);
    void *ev_arg;

    int ev_res;     /* result passed to event callback */
    int ev_flags;
};

//事件链表
TAILQ_HEAD (event_list, event);

//初始化事件
struct event_base * event_init(void);

//设置事件

void event_set(struct event *ev, int fd, short events, void (*callback)(int, short, void *), void *arg);

//添加事件
int event_add(struct event *ev, const struct timeval *tv);
//删除事件
int event_del(struct event *ev);

//插入队列
void event_queue_insert(struct event_base *base, struct event *ev, int queue);

//删除事件队列
void event_queue_remove(struct event_base *base, struct event *ev, int queue);

//事件循环
int event_base_dispatch(struct event_base *base, int flags);
//事件激活
void event_active(struct event *ev, int res);
//时间超时
void timeout_process(struct event_base *base);

#endif
