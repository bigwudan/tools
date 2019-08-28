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





int main()
{
    printf("Hello world\n");
    return 0;
}

