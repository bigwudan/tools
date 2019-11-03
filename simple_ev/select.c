#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "event-internal.h"

#define        howmany(x, y)   (((x)+((y)-1))/(y))

struct selectop {
    int event_fds;		/* Highest fd in fd set */
    int event_fdsz;
    fd_set *event_readset_in;
    fd_set *event_writeset_in;
    fd_set *event_readset_out;
    fd_set *event_writeset_out;
    struct event **event_r_by_fd;
    struct event **event_w_by_fd;
};



static void *select_init	(struct event_base *);
static int select_add		(void *, struct event *);
static int select_del		(void *, struct event *);
static int select_dispatch	(struct event_base *, void *, struct timeval *);
static void select_dealloc     (struct event_base *, void *);

const struct eventop selectops = {
    "select",
    select_init,
    select_add,
    select_del,
    select_dispatch,
    select_dealloc,
    0
};

static int select_resize(struct selectop *sop, int fdsz);

    static void *
select_init(struct event_base *base)
{
    struct selectop *sop;
    /* Disable select when this environment variable is set */

    if (!(sop = calloc(1, sizeof(struct selectop))))
        return (NULL);
    select_resize(sop, 2000);
    return (sop);
}

    static int
select_dispatch(struct event_base *base, void *arg, struct timeval *tv)
{
    int res, i, j;
    struct selectop *sop = arg;
    memcpy(sop->event_readset_out, sop->event_readset_in,
            sop->event_fdsz);
    memcpy(sop->event_writeset_out, sop->event_writeset_in,
            sop->event_fdsz);
    res = select(sop->event_fds + 1, sop->event_readset_out,
            sop->event_writeset_out, NULL, tv);
    if (res == -1) {
        if (errno != EINTR) {
            return (-1);
        }
        return (0);
    }
    i = random() % (sop->event_fds+1);
    for (j = 0; j <= sop->event_fds; ++j) {
        struct event *r_ev = NULL, *w_ev = NULL;
        if (++i >= sop->event_fds+1)
            i = 0;

        res = 0;
        if (FD_ISSET(i, sop->event_readset_out)) {
            r_ev = sop->event_r_by_fd[i];
            res |= EV_READ;
        }
        if (FD_ISSET(i, sop->event_writeset_out)) {
            w_ev = sop->event_w_by_fd[i];
            res |= EV_WRITE;
        }
        if (r_ev && (res & r_ev->ev_events)) {
            event_active(r_ev, res & r_ev->ev_events);
        }
        if (w_ev && w_ev != r_ev && (res & w_ev->ev_events)) {
            event_active(w_ev, res & w_ev->ev_events);
        }
    }
    return (0);
}


    static int
select_resize(struct selectop *sop, int fdsz)
{
    int n_events, n_events_old;

    fd_set *readset_in = NULL;
    fd_set *writeset_in = NULL;
    fd_set *readset_out = NULL;
    fd_set *writeset_out = NULL;
    struct event **r_by_fd = NULL;
    struct event **w_by_fd = NULL;

    n_events = (fdsz/sizeof(fd_mask)) * NFDBITS;
    n_events_old = (sop->event_fdsz/sizeof(fd_mask)) * NFDBITS;


    if ((readset_in = realloc(sop->event_readset_in, fdsz)) == NULL)
        goto error;
    sop->event_readset_in = readset_in;
    if ((readset_out = realloc(sop->event_readset_out, fdsz)) == NULL)
        goto error;
    sop->event_readset_out = readset_out;
    if ((writeset_in = realloc(sop->event_writeset_in, fdsz)) == NULL)
        goto error;
    sop->event_writeset_in = writeset_in;
    if ((writeset_out = realloc(sop->event_writeset_out, fdsz)) == NULL)
        goto error;
    sop->event_writeset_out = writeset_out;
    if ((r_by_fd = realloc(sop->event_r_by_fd,
                    n_events*sizeof(struct event*))) == NULL)
        goto error;
    sop->event_r_by_fd = r_by_fd;
    if ((w_by_fd = realloc(sop->event_w_by_fd,
                    n_events * sizeof(struct event*))) == NULL)
        goto error;
    sop->event_w_by_fd = w_by_fd;
    memset((char *)sop->event_readset_in + sop->event_fdsz, 0,
            fdsz - sop->event_fdsz);
    memset((char *)sop->event_writeset_in + sop->event_fdsz, 0,
            fdsz - sop->event_fdsz);
    memset(sop->event_r_by_fd + n_events_old, 0,
            (n_events-n_events_old) * sizeof(struct event*));
    memset(sop->event_w_by_fd + n_events_old, 0,
            (n_events-n_events_old) * sizeof(struct event*));
    sop->event_fdsz = fdsz;
    return (0);
error:
    return (-1);
}


    static int
select_add(void *arg, struct event *ev)
{
    struct selectop *sop = arg;


    /*
     *   * Keep track of the highest fd, so that we can calculate the size
     *       * of the fd_sets for select(2)
     *           */
    if (sop->event_fds < ev->ev_fd) {
        int fdsz = sop->event_fdsz;

        if (fdsz < sizeof(fd_mask))
            fdsz = sizeof(fd_mask);

        while (fdsz <
                (howmany(ev->ev_fd + 1, NFDBITS) * sizeof(fd_mask)))
            fdsz *= 2;

        if (fdsz != sop->event_fdsz) {
            if (select_resize(sop, fdsz)) {
                return (-1);
            }
        }

        sop->event_fds = ev->ev_fd;
    }

    if (ev->ev_events & EV_READ) {
        FD_SET(ev->ev_fd, sop->event_readset_in);
        sop->event_r_by_fd[ev->ev_fd] = ev;
    }
    if (ev->ev_events & EV_WRITE) {
        FD_SET(ev->ev_fd, sop->event_writeset_in);
        sop->event_w_by_fd[ev->ev_fd] = ev;
    }

    return (0);
}

static int
select_del(void *arg, struct event *ev)
{
    struct selectop *sop = arg;
//    if (ev->ev_events & EV_SIGNAL)
//        return (evsignal_del(ev));
    if (sop->event_fds < ev->ev_fd) {
        return (0);
    }
    if (ev->ev_events & EV_READ) {
        FD_CLR(ev->ev_fd, sop->event_readset_in);
        sop->event_r_by_fd[ev->ev_fd] = NULL;
    }
    if (ev->ev_events & EV_WRITE) {
        FD_CLR(ev->ev_fd, sop->event_writeset_in);
        sop->event_w_by_fd[ev->ev_fd] = NULL;
    }
    return (0);
}

static void
select_dealloc(struct event_base *base, void *arg)
{
    struct selectop *sop = arg;

    if (sop->event_readset_in)
        free(sop->event_readset_in);
    if (sop->event_writeset_in)
        free(sop->event_writeset_in);
    if (sop->event_readset_out)
        free(sop->event_readset_out);
    if (sop->event_writeset_out)
        free(sop->event_writeset_out);
    if (sop->event_r_by_fd)
        free(sop->event_r_by_fd);
    if (sop->event_w_by_fd)
        free(sop->event_w_by_fd);
    memset(sop, 0, sizeof(struct selectop));
    free(sop);
}



