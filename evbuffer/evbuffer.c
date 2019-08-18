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
evbuffer_add(struct evbuffer *buf, const void *data_in, size_t datlen)
{




}





int main()
{
    printf("Hello world\n");
    return 0;
}

