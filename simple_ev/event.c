#include <stdio.h>
#include <stdlib.h>
#include "event-internal.h"
#include "event.h"
#include "min_heap.h"


extern const struct eventop selectops;

struct event_base *current_base = NULL;


/* In order of preference */
static const struct eventop *eventops[] = {
    &selectops,
    NULL
};



struct event_base *
event_init(void)
{
    int i =0;
    struct event_base *base = NULL;
    if ((base = calloc(1, sizeof(struct event_base))) == NULL){
        return NULL;
    }

    min_heap_ctor(&base->timeheap);

    //事件初始化
    TAILQ_INIT(&base->eventqueue);
    //activequeues初始化
    TAILQ_INIT(&base->activequeues);

    base->evbase = NULL;
    for (i = 0; eventops[i] && !base->evbase; i++) {
        base->evsel = eventops[i];
        base->evbase = base->evsel->init(base);
    }
    if (base->evbase == NULL)
        return NULL;
        
    current_base = base;
    return base;
}


void
event_set(struct event *ev, int fd, short events,
      void (*callback)(int, short, void *), void *arg)
{
    ev->ev_base = current_base;
    ev->ev_callback = callback;
    ev->ev_arg = arg;
    ev->ev_fd = fd;
    ev->ev_events = events;
    ev->ev_res = 0;
    ev->ev_flags = EVLIST_INIT;
    min_heap_elem_init(ev);
}


int
event_add(struct event *ev, const struct timeval *tv)
{
    struct event_base *base = ev->ev_base;
    const struct eventop *evsel = base->evsel;
    void *evbase = base->evbase;
    int res = 0;
    /*
     * prepare for timeout insertion further below, if we get a
     * failure on any step, we should not change any state.
     */
    if (tv != NULL && !(ev->ev_flags & EVLIST_TIMEOUT)) {
        if (min_heap_reserve(&base->timeheap,
            1 + min_heap_size(&base->timeheap)) == -1)
            return (-1);  /* ENOMEM == errno */
    }

    if ((ev->ev_events & (EV_READ|EV_WRITE|EV_SIGNAL)) &&
        !(ev->ev_flags & (EVLIST_INSERTED|EVLIST_ACTIVE))) {
        res = evsel->add(evbase, ev);
        if (res != -1)
            event_queue_insert(base, ev, EVLIST_INSERTED);
    }

    /* 
     * we should change the timout state only if the previous event
     * addition succeeded.
     */
    if (res != -1 && tv != NULL) {
        struct timeval now;

        /* 
         * we already reserved memory above for the case where we
         * are not replacing an exisiting timeout.
         */
        if (ev->ev_flags & EVLIST_TIMEOUT)
            event_queue_remove(base, ev, EVLIST_TIMEOUT);

        /* Check if it is active due to a timeout.  Rescheduling
         * this timeout before the callback can be executed
         * removes it from the active list. */
        if ((ev->ev_flags & EVLIST_ACTIVE) &&
            (ev->ev_res & EV_TIMEOUT)) {
            /* See if we are just active executing this
             * event in a loop
             */
            event_queue_remove(base, ev, EVLIST_ACTIVE);
        }
        gettimeofday(&now, NULL);
        evutil_timeradd(&now, tv, &ev->ev_timeout);
        event_queue_insert(base, ev, EVLIST_TIMEOUT);
    }
    return (res);
}

//删除
int
event_del(struct event *ev)
{
	struct event_base *base;
	const struct eventop *evsel;
	void *evbase;


	/* An event without a base has not been added */
	if (ev->ev_base == NULL)
		return (-1);

	base = ev->ev_base;
	evsel = base->evsel;
	evbase = base->evbase;



	if (ev->ev_flags & EVLIST_TIMEOUT)
		event_queue_remove(base, ev, EVLIST_TIMEOUT);

	if (ev->ev_flags & EVLIST_ACTIVE)
		event_queue_remove(base, ev, EVLIST_ACTIVE);

	if (ev->ev_flags & EVLIST_INSERTED) {
		event_queue_remove(base, ev, EVLIST_INSERTED);
		return (evsel->del(evbase, ev));
	}

	return (0);
}


void
event_queue_insert(struct event_base *base, struct event *ev, int queue)
{
    if (ev->ev_flags & queue) {
        /* Double insertion is possible for active events */
        if (queue & EVLIST_ACTIVE)
            return;
    }

    if (~ev->ev_flags & EVLIST_INTERNAL)
        base->event_count++;

    ev->ev_flags |= queue;
    switch (queue) {
        case EVLIST_INSERTED:
            TAILQ_INSERT_TAIL(&base->eventqueue, ev, ev_next);
            break;
        case EVLIST_ACTIVE:
            base->event_count_active++;
            TAILQ_INSERT_TAIL(&base->activequeues, ev, ev_active_next);
            break;
        case EVLIST_TIMEOUT: 
            min_heap_push(&base->timeheap, ev);
            break;
        default:
            break;
    }
}


void
event_queue_remove(struct event_base *base, struct event *ev, int queue)
{

    if (~ev->ev_flags & EVLIST_INTERNAL)
        base->event_count--;

    ev->ev_flags &= ~queue;
    switch (queue) {
    case EVLIST_INSERTED:
        TAILQ_REMOVE(&base->eventqueue, ev, ev_next);
        break;
    case EVLIST_ACTIVE:
        base->event_count_active--;
        TAILQ_REMOVE(&base->activequeues, ev, ev_active_next);
        break;

    case EVLIST_TIMEOUT:
        min_heap_erase(&base->timeheap, ev);
        break;
    default:
        break;
    }
}


static int
timeout_next(struct event_base *base, struct timeval **tv_p)
{
    struct timeval now;
    struct event *ev;
    struct timeval *tv = *tv_p;
    if ((ev = min_heap_top(&base->timeheap)) == NULL) {
        /* if no time-based events are active wait for I/O */
        *tv_p = NULL;
        return (0);
    }
    gettimeofday(&now, NULL);
    if (evutil_timercmp(&ev->ev_timeout, &now, <=)) {
        evutil_timerclear(tv);
        return (0);
    }
    evutil_timersub(&ev->ev_timeout, &now, tv);
    return (0);
}

static void
event_process_active(struct event_base *base)
{
    struct event *ev;
    struct event_list *activeq = &base->activequeues;
    int i;
    for (ev = TAILQ_FIRST(activeq); ev; ev = TAILQ_FIRST(activeq)) {
        if (ev->ev_events & EV_PERSIST)
            event_queue_remove(base, ev, EVLIST_ACTIVE);
        else
            event_del(ev);
        (*ev->ev_callback)((int)ev->ev_fd, ev->ev_res, ev->ev_arg);
    }
}


int
event_base_dispatch(struct event_base *base, int flags)
{

    const struct eventop *evsel = base->evsel;
    void *evbase = base->evbase;
    struct timeval tv;
    struct timeval *tv_p;
    int res, done;
    done = 0;
    while (!done) {
        /* Terminate the loop if we have been asked to */
        if (base->event_gotterm) {
            base->event_gotterm = 0;
            break;
        }

        if (base->event_break) {
            base->event_break = 0;
            break;
        }
        tv_p = &tv;
        timeout_next(base, &tv_p);

        res = evsel->dispatch(base, evbase, tv_p);

        timeout_process(base);
        if (base->event_count_active) {
            event_process_active(base);
            if (!base->event_count_active && (flags & EVLOOP_ONCE))
                done = 1;
        } else if (flags & EVLOOP_NONBLOCK)
            done = 1;
    }
    return (0); 
}

void
event_active(struct event *ev, int res)
{
    /* We get different kinds of events, add them together */
    if (ev->ev_flags & EVLIST_ACTIVE) {
        ev->ev_res |= res;
        return;
    }
    ev->ev_res = res;
    event_queue_insert(ev->ev_base, ev, EVLIST_ACTIVE);
}

//时间超时
void
timeout_process(struct event_base *base)
{
	struct timeval now;
	struct event *ev;

	if (min_heap_empty(&base->timeheap))
		return;
	gettimeofday(&now, NULL); 
	while ((ev = min_heap_top(&base->timeheap))) {
		if (evutil_timercmp(&ev->ev_timeout, &now, >))
			break;
		event_del(ev);
		event_active(ev, EV_TIMEOUT);
	}
}


