#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wl_buffer.h"


struct wl_evbuffer *
wl_evbuffer_new(void)
{
    struct wl_evbuffer *buffer;
    buffer = calloc(1, sizeof(struct wl_evbuffer));
    return (buffer);
}


void
wl_evbuffer_free(struct wl_evbuffer *buffer)
{
    if (buffer->orig_buffer != NULL)
        free(buffer->orig_buffer);
    free(buffer);
}

#define SWAP(x,y) do { \
	(x)->buffer = (y)->buffer; \
	(x)->orig_buffer = (y)->orig_buffer; \
	(x)->misalign = (y)->misalign; \
	(x)->totallen = (y)->totallen; \
	(x)->off = (y)->off; \
} while (0)

static void
wl_evbuffer_align(struct wl_evbuffer *buf)
{
	memmove(buf->orig_buffer, buf->buffer, buf->off);
	buf->buffer = buf->orig_buffer;
	buf->misalign = 0;
}

#define WL_SIZE_MAX ((size_t)-1)

int
wl_evbuffer_expand(struct wl_evbuffer *buf, size_t datlen)
{
	size_t used = buf->misalign + buf->off;
	size_t need;
	//assert(buf->totallen >= used);
	/* If we can fit all the data, then we don't have to do anything */
	if (buf->totallen - used >= datlen)
		return (0);
	/* If we would need to overflow to fit this much data, we can't
	 * do anything. */
	if (datlen > WL_SIZE_MAX - buf->off)
		return (-1);

	/*
	 * If the misalignment fulfills our data needs, we just force an
	 * alignment to happen.  Afterwards, we have enough space.
	 */
	if (buf->totallen - buf->off >= datlen) {
		wl_evbuffer_align(buf);
	} else {
		void *newbuf;
		size_t length = buf->totallen;
		size_t need = buf->off + datlen;

		if (length < 256)
			length = 256;
		if (need < WL_SIZE_MAX / 2) {
			while (length < need) {
				length <<= 1;
			}
		} else {
			length = need;
		}

		if (buf->orig_buffer != buf->buffer)
			wl_evbuffer_align(buf);
		if ((newbuf = realloc(buf->buffer, length)) == NULL)
			return (-1);

		buf->orig_buffer = buf->buffer = newbuf;
		buf->totallen = length;
	}
	return (0);
}


int
wl_evbuffer_add(struct wl_evbuffer *buf, const void *data, size_t datlen)
{
	size_t used = buf->misalign + buf->off;
	size_t oldoff = buf->off;
	if (buf->totallen - used < datlen) {
		if (wl_evbuffer_expand(buf, datlen) == -1)
			return (-1);
	}
	memcpy(buf->buffer + buf->off, data, datlen);
	buf->off += datlen;
	return (0);
}

void wl_evbuffer_drain(struct wl_evbuffer *buf, size_t len)
{
	size_t oldoff = buf->off;
	if (len >= buf->off) {
		buf->off = 0;
		buf->buffer = buf->orig_buffer;
		buf->misalign = 0;
        goto done;

	}
	buf->buffer += len;
	buf->misalign += len;
	buf->off -= len;
 done:
    return ;
}
