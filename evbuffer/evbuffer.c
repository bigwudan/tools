#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "evbuffer.h"

#define MAX_CHIAN 200000
#define GETEVBUFFER(t, c) ((unsigned char *)(struct evbuffer_chain *)t + 1)

static struct evbuffer_chain *
evbuffer_chain_new(size_t size)
{
    struct evbuffer_chain *chain;
    size_t to_alloc;
    if(size > EVBUFFER_CHAIN_MAX - EVBUFFER_CHAIN_SIZE)
        return NULL;
    size += EVBUFFER_CHAIN_SIZE;
    if(size < EVBUFFER_CHAIN_MAX / 2){
        to_alloc = 512;
        while(to_alloc < size){
            to_alloc <<= 1;
        }

    }else{
        to_alloc = size;
    }
    if((chain = calloc(to_alloc, sizeof(char))) == NULL)
        return NULL;
    chain->buffer_len = to_alloc - EVBUFFER_CHAIN_SIZE;
    chain->buffer = EVBUFFER_CHAIN_EXTRA(unsigned char, chain);
    chain->refcnt = 1;
    return chain;


}

static inline void
evbuffer_chain_free(struct evbuffer_chain *chain){

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
        evbuffer_free_all_chains(*ch);
        *ch = NULL;

    }
    return ch;

}



static void
evbuffer_chain_insert(struct evbuffer *buf,
                      struct evbuffer_chain *chain)
{
    if (*buf->last_with_datap == NULL) {
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

#define MAX_TO_COPY_IN_EXPAND 4096
#define MAX_TO_REALIGN_IN_EXPAND 2048

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

#define EVBUFFER_CHAIN_MAX_AUTO_SIZE 4096


int
evbuffer_add(struct evbuffer *buf, const void *data_in, size_t datlen)
{

    struct evbuffer_chain *chain, *tmp;
    const unsigned char *data = data_in;
    size_t remain, to_alloc;
    int result = -1;


    /* Prevent buf->total_len overflow */
    if (datlen > EV_SIZE_MAX - buf->total_len) {
        goto done;

    }

    if (*buf->last_with_datap == NULL) {
        chain = buf->last;

    } else {
        chain = *buf->last_with_datap;

    }

    /* If there are no chains allocated for this buffer, allocate one
     *   * big enough to hold all the data. */
    if (chain == NULL) {
        chain = evbuffer_chain_new(datlen);
        if (!chain)
            goto done;
        evbuffer_chain_insert(buf, chain);

    }

    if ((chain->flags & EVBUFFER_IMMUTABLE) == 0) {
        /* Always true for mutable buffers */
        remain = chain->buffer_len - (size_t)chain->misalign - chain->off;
        if (remain >= datlen) {
            /* there's enough space to hold all the data in the
             *           * current last chain */
            memcpy(chain->buffer + chain->misalign + chain->off,
                   data, datlen);
            chain->off += datlen;
            buf->total_len += datlen;
            buf->n_add_for_cb += datlen;
            goto out;

        } else if ( evbuffer_chain_should_realign(chain, datlen)) {
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

