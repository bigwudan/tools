#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


#include "kfifo.h"

static inline unsigned int kfifo_unused(struct __kfifo *fifo)
{
        return (fifo->mask + 1) - (fifo->in - fifo->out);
}

uint32_t roundup_pow_of_two(const uint32_t x)
{
    if (x == 0){ return 0; }
    if (x == 1){ return 2; }
    uint32_t ret = 1;
    while (ret < x)
    {
        ret = ret << 1;
    }
    return ret;
}




int __kfifo_alloc(struct __kfifo *fifo, unsigned int size,
                size_t esize)
{
    /*
     *   * round up to the next power of 2, since our 'let the indices
     *       * wrap' technique works only in this case.
     *           */
    size = roundup_pow_of_two(size);

    fifo->in = 0;
    fifo->out = 0;
    fifo->esize = esize;

    if (size < 2) {
        fifo->data = NULL;
        fifo->mask = 0;
        return -1;
    }

    fifo->data = malloc(size*esize);

    if (!fifo->data) {
        fifo->mask = 0;
        return -1;
    }
    fifo->mask = size - 1;

    return 0;
}


int __kfifo_init(struct __kfifo *fifo, void *buffer,
        unsigned int size, size_t esize)
{
    size /= esize;

    size = roundup_pow_of_two(size);

    fifo->in = 0;
    fifo->out = 0;
    fifo->esize = esize;
    fifo->data = buffer;

    if (size < 2) {
        fifo->mask = 0;
        return -1;
    }
    fifo->mask = size - 1;

    return 0;
}

void __kfifo_free(struct __kfifo *fifo)
{
    free(fifo->data);
    fifo->in = 0;
    fifo->out = 0;
    fifo->esize = 0;
    fifo->data = NULL;
    fifo->mask = 0;
}


static unsigned int
min(int a, int b)
{
    if(a > b){
        return b;
    }else{
        return a;
    }
}


static void kfifo_copy_in(struct __kfifo *fifo, const void *src,
                unsigned int len, unsigned int off)
{
    unsigned int size = fifo->mask + 1;
    unsigned int esize = fifo->esize;
    unsigned int l;

    off &= fifo->mask;
    if (esize != 1) {
        off *= esize;
        size *= esize;
        len *= esize;
    }
    l = min(len, size - off);

    memcpy(fifo->data + off, src, l);
    memcpy(fifo->data, src + l, len - l);


}

unsigned int __kfifo_in(struct __kfifo *fifo,
        const void *buf, unsigned int len)
{
    unsigned int l;

    l = kfifo_unused(fifo);
    if (len > l)
        len = l;

    kfifo_copy_in(fifo, buf, len, fifo->in);
    fifo->in += len;
    return len;
}








