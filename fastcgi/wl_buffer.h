#ifndef _WL_BUFFER_H_
#define _WL_BUFFER_H_

#include <stdint.h>
#include <stddef.h>


struct wl_evbuffer {
    uint8_t *buffer;
    uint8_t *orig_buffer;
    uint32_t misalign;
    uint32_t totallen;
    uint32_t off;
};

struct wl_evbuffer *wl_evbuffer_new(void);
void wl_evbuffer_free(struct wl_evbuffer *buffer);
int wl_evbuffer_expand(struct wl_evbuffer *buf, size_t datlen);
static void wl_evbuffer_align(struct wl_evbuffer *buf);
int wl_evbuffer_add(struct wl_evbuffer *buf, const void *data, size_t datlen);
void wl_evbuffer_drain(struct wl_evbuffer *buf, size_t len);
#endif
