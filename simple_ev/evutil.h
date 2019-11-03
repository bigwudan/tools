#ifndef _EVUTIL_H_
#define _EVUTIL_H_


#include <sys/time.h>


#define evutil_timeradd(tvp, uvp, vvp) timeradd((tvp), (uvp), (vvp))
#define evutil_timersub(tvp, uvp, vvp) timersub((tvp), (uvp), (vvp))
#define evutil_timerclear(tvp) timerclear(tvp)

#define evutil_timercmp(tvp, uvp, cmp)                          \
    (((tvp)->tv_sec == (uvp)->tv_sec) ?                         \
     ((tvp)->tv_usec cmp (uvp)->tv_usec) :                      \
     ((tvp)->tv_sec cmp (uvp)->tv_sec))



#endif
