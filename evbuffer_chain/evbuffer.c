#include <stdio.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <limits.h>


#include "evbuffer.h"


static struct evbuffer_chain *
evbuffer_chain_new(size_t size)
{
    struct evbuffer_chain *chain;
    size_t to_alloc;

    if (size > EVBUFFER_CHAIN_MAX - EVBUFFER_CHAIN_SIZE)
        return (NULL);

    size += EVBUFFER_CHAIN_SIZE;

    /* get the next largest memory that can hold the buffer */
    if (size < EVBUFFER_CHAIN_MAX  ) {
        to_alloc = MIN_BUFFER_SIZE;
        while (to_alloc < size) {
            to_alloc <<= 1;
        }
    } else {
        to_alloc = size;
    }

    /* we get everything in one chunk */
    if ((chain = malloc(to_alloc)) == NULL)
        return (NULL);

    memset(chain, 0, EVBUFFER_CHAIN_SIZE);

    chain->buffer_len = to_alloc - EVBUFFER_CHAIN_SIZE;

    /* this way we can manipulate the buffer to different addresses,
     * which is required for mmap for example.
     */
    chain->buffer = EVBUFFER_CHAIN_EXTRA(unsigned char, chain);

    chain->refcnt = 1;

    return (chain);
}

static inline void
evbuffer_chain_free(struct evbuffer_chain *chain)
{
    free(chain);
}

static void
evbuffer_free_all_chains(struct evbuffer_chain *chain)
{
    struct evbuffer_chain *next;
    for (; chain; chain = next) {
        next = chain->next;
        evbuffer_chain_free(chain);
    }
}


static struct evbuffer_chain **
evbuffer_free_trailing_empty_chains(struct evbuffer *buf)
{
    struct evbuffer_chain **ch = buf->last_with_datap;
    /* Find the first victim chain.  It might be *last_with_datap */
    while ((*ch) && ((*ch)->off != 0 ))
        ch = &(*ch)->next;
    if (*ch) {
        //evbuffer_chains_all_empty(*ch);
        evbuffer_free_all_chains(*ch);
        *ch = NULL;
    }
    return ch;
}




/* Add a single chain 'chain' to the end of 'buf', freeing trailing empty
 * chains as necessary.  Requires lock.  Does not schedule callbacks.
 */
static void
evbuffer_chain_insert(struct evbuffer *buf,
    struct evbuffer_chain *chain)
{
    if (*buf->last_with_datap == NULL) {
        /* There are no chains data on the buffer at all. */
        buf->first = buf->last = chain;
    } else {
        struct evbuffer_chain **chp;
        chp = evbuffer_free_trailing_empty_chains(buf);
        *chp = chain;
        if (chain->off)
            buf->last_with_datap = chp;
        buf->last = chain;
    }
    buf->total_len += chain->off;
}

static inline struct evbuffer_chain *
evbuffer_chain_insert_new(struct evbuffer *buf, size_t datlen)
{
    struct evbuffer_chain *chain;
    if ((chain = evbuffer_chain_new(datlen)) == NULL)
        return NULL;
    evbuffer_chain_insert(buf, chain);
    return chain;
}

struct evbuffer *
evbuffer_new(void)
{
    struct evbuffer *buffer;

    buffer = calloc(1, sizeof(struct evbuffer));
    if (buffer == NULL)
        return (NULL);

    buffer->refcnt = 1;
    buffer->last_with_datap = &buffer->first;

    return (buffer);
}


int
evbuffer_set_flags(struct evbuffer *buf, uint64_t flags)
{
    buf->flags |= (uint32_t)flags;
    return 0;
}

int
evbuffer_clear_flags(struct evbuffer *buf, uint64_t flags)
{
    buf->flags &= ~(uint32_t)flags;
    return 0;
}


void
evbuffer_decref_and_unlock_(struct evbuffer *buffer)
{
    struct evbuffer_chain *chain, *next;
    if (--buffer->refcnt > 0) {
        return;
    }
    for (chain = buffer->first; chain != NULL; chain = next) {
        next = chain->next;
        evbuffer_chain_free(chain);
    }
    free(buffer);
}



void
evbuffer_free(struct evbuffer *buffer)
{
    evbuffer_decref_and_unlock_(buffer);
}

size_t
evbuffer_get_length(const struct evbuffer *buffer)
{
    size_t result;
    result = (buffer->total_len);
    return result;
}

static inline void
ZERO_CHAIN(struct evbuffer *dst)
{

    dst->first = NULL;
    dst->last = NULL;
    dst->last_with_datap = &(dst)->first;
    dst->total_len = 0;
}

int
evbuffer_drain(struct evbuffer *buf, size_t len)
{
    struct evbuffer_chain *chain, *next;
    size_t remaining, old_len;
    int result = 0;


    old_len = buf->total_len;

    if (old_len == 0)
        goto done;

    if (buf->freeze_start) {
        result = -1;
        goto done;
    }

    if (len >= old_len ) {
        len = old_len;
        for (chain = buf->first; chain != NULL; chain = next) {
            next = chain->next;
            evbuffer_chain_free(chain);
        }

        ZERO_CHAIN(buf);
    } else {
        if (len >= old_len)
            len = old_len;

        buf->total_len -= len;
        remaining = len;
        for (chain = buf->first;
             remaining >= chain->off;
             chain = next) {
            next = chain->next;
            remaining -= chain->off;

            if (chain == *buf->last_with_datap) {
                buf->last_with_datap = &buf->first;
            }
            if (&chain->next == buf->last_with_datap)
                buf->last_with_datap = &buf->first;


            evbuffer_chain_free(chain);
        }

        buf->first = chain;

        chain->misalign += remaining;
        chain->off -= remaining;
    }

    buf->n_del_for_cb += len;


done:

    return result;
}


ssize_t
evbuffer_copyout_from(struct evbuffer *buf, const struct evbuffer_ptr *pos,
    void *data_out, size_t datlen);

/* Reads data from an event buffer and drains the bytes read */
int
evbuffer_remove(struct evbuffer *buf, void *data_out, size_t datlen)
{
    ssize_t n;
    n = evbuffer_copyout_from(buf, NULL, data_out, datlen);
    if (n > 0) {
        if (evbuffer_drain(buf, n)<0)
            n = -1;
    }
    return (int)n;
}

ssize_t
evbuffer_copyout(struct evbuffer *buf, void *data_out, size_t datlen)
{
    return evbuffer_copyout_from(buf, NULL, data_out, datlen);
}


ssize_t
evbuffer_copyout_from(struct evbuffer *buf, const struct evbuffer_ptr *pos,
    void *data_out, size_t datlen)
{
    /*XXX fails badly on sendfile case. */
    struct evbuffer_chain *chain;
    char *data = data_out;
    size_t nread;
    ssize_t result = 0;
    size_t pos_in_chain;



    if (pos) {
        if (datlen > (size_t)(EV_UINT32_MAX - pos->pos)) {
            result = -1;
            goto done;
        }
        chain = pos->internal_.chain;
        pos_in_chain = pos->internal_.pos_in_chain;
        if (datlen + pos->pos > buf->total_len)
            datlen = buf->total_len - pos->pos;
    } else {
        chain = buf->first;
        pos_in_chain = 0;
        if (datlen > buf->total_len)
            datlen = buf->total_len;
    }


    if (datlen == 0)
        goto done;

    if (buf->freeze_start) {
        result = -1;
        goto done;
    }

    nread = datlen;

    while (datlen && datlen >= chain->off - pos_in_chain) {
        size_t copylen = chain->off - pos_in_chain;
        memcpy(data,
            chain->buffer + chain->misalign + pos_in_chain,
            copylen);
        data += copylen;
        datlen -= copylen;

        chain = chain->next;
        pos_in_chain = 0;
        
    }

    if (datlen) {


        memcpy(data, chain->buffer + chain->misalign + pos_in_chain,
            datlen);
    }

    result = nread;
done:

    return result;
}


static int
evbuffer_chain_should_realign(struct evbuffer_chain *chain,
    size_t datlen)
{
    return chain->buffer_len - chain->off >= datlen &&
        (chain->off < chain->buffer_len / 2) &&
        (chain->off <= MAX_TO_REALIGN_IN_EXPAND);
}


static void
evbuffer_chain_align(struct evbuffer_chain *chain)
{
    memmove(chain->buffer, chain->buffer + chain->misalign, chain->off);
    chain->misalign = 0;
}


int
evbuffer_add(struct evbuffer *buf, const void *data_in, size_t datlen)
{
    struct evbuffer_chain *chain, *tmp;
    const unsigned char *data = data_in;
    size_t remain, to_alloc;
    int result = -1;

    
    if (buf->freeze_end) {
        goto done;
    }
    /* Prevent buf->total_len overflow */
    if (datlen > EV_UINT64_MAX - buf->total_len) {
        goto done;
    }

    if (*buf->last_with_datap == NULL) {
        chain = buf->last;
    } else {
        chain = *buf->last_with_datap;
    }

    /* If there are no chains allocated for this buffer, allocate one
     * big enough to hold all the data. */
    if (chain == NULL) {
        chain = evbuffer_chain_new(datlen);
        if (!chain)
            goto done;
        evbuffer_chain_insert(buf, chain);
    }

    if (chain->flags ) {
        /* Always true for mutable buffers */

        remain = chain->buffer_len - (size_t)chain->misalign - chain->off;
        if (remain >= datlen) {
            /* there's enough space to hold all the data in the
             * current last chain */
            memcpy(chain->buffer + chain->misalign + chain->off,
                data, datlen);
            chain->off += datlen;
            buf->total_len += datlen;
            buf->n_add_for_cb += datlen;
            goto out;
        } else if (
            evbuffer_chain_should_realign(chain, datlen)) {
            /* we can fit the data into the misalignment */
            evbuffer_chain_align(chain);

            memcpy(chain->buffer + chain->off, data, datlen);
            chain->off += datlen;
            buf->total_len += datlen;
            buf->n_add_for_cb += datlen;
            goto out;
        }
    } else {
        /* we cannot write any data to the last chain */
        remain = 0;
    }

    /* we need to add another chain */
    to_alloc = chain->buffer_len;
    if (to_alloc <= EVBUFFER_CHAIN_MAX_AUTO_SIZE/2)
        to_alloc <<= 1;
    if (datlen > to_alloc)
        to_alloc = datlen;
    tmp = evbuffer_chain_new(to_alloc);
    if (tmp == NULL)
        goto done;

    if (remain) {
        memcpy(chain->buffer + chain->misalign + chain->off,
            data, remain);
        chain->off += remain;
        buf->total_len += remain;
        buf->n_add_for_cb += remain;
    }

    data += remain;
    datlen -= remain;

    memcpy(tmp->buffer, data, datlen);
    tmp->off = datlen;
    evbuffer_chain_insert(buf, tmp);
    buf->n_add_for_cb += datlen;

out:

    result = 0;
done:

    return result;
}






int main()
{
    printf("Hello world\n");
    return 0;
}

