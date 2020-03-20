#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


#include "kfifo.h"

static inline int fls(int x)
{
	int position;
	int i;
	if(0 != x)
	{
		for (i = (x >> 1), position = 0; i != 0; ++position)
			i >>= 1;
	}
	else
	{
		position = -1;
	} 
	return position+1;
}

static inline unsigned int roundup_pow_of_two(unsigned int x)
{
	return 1UL << fls(x - 1);
}



/* 创建队列 */
struct kfifo *kfifo_init(unsigned char *buffer, unsigned int size)
{
	struct kfifo *fifo;
	/* size must be a power of 2 ：判断是否为2的幂*/
	fifo = malloc(sizeof(struct kfifo));
	if (!fifo)
		return NULL;
	fifo->buffer = buffer;
	fifo->size = size;
	fifo->in = fifo->out = 0;
	return fifo;
}

/* 分配空间 */
struct kfifo *kfifo_alloc(unsigned int size)
{
	unsigned char *buffer;
	struct kfifo *ret;
	if (!is_power_of_2(size)) {  /* 判断是否为2的幂 */
		size = roundup_pow_of_two(size); /* 如果不是则向上扩展成2的幂 */
	}
	buffer = malloc(size);
	if (!buffer)
		return NULL;
	ret = kfifo_init(buffer, size);
	if(ret == NULL){
		free(buffer);	
	}	 

	return ret;
}

unsigned int kfifo_put(struct kfifo *fifo,
                 const unsigned char *buffer, unsigned int len)
 {
     unsigned int ret;
     ret = __kfifo_put(fifo, buffer, len);
     return ret;
 }
 
unsigned int kfifo_get(struct kfifo *fifo,
                      unsigned char *buffer, unsigned int len)
 {
     unsigned int ret;
     ret = __kfifo_get(fifo, buffer, len);
     //当fifo->in == fifo->out时，buufer为空
     if (fifo->in == fifo->out)
         fifo->in = fifo->out = 0;
     return ret;
 }
 
 unsigned int __kfifo_put(struct kfifo *fifo,
             const unsigned char *buffer, unsigned int len)
 {
     unsigned int l;
     //buffer中空的长度
     len = min(len, fifo->size - fifo->in + fifo->out);
     /*
      * Ensure that we sample the fifo->out index -before- we
      * start putting bytes into the kfifo.
      */
     /* first put the data starting from fifo->in to buffer end */
     l = min(len, fifo->size - (fifo->in & (fifo->size - 1)));
     memcpy(fifo->buffer + (fifo->in & (fifo->size - 1)), buffer, l);
     /* then put the rest (if any) at the beginning of the buffer */
     memcpy(fifo->buffer, buffer + l, len - l);
 
     /*
      * Ensure that we add the bytes to the kfifo -before-
      * we update the fifo->in index.
      */
     fifo->in += len;  //每次累加，到达最大值后溢出，自动转为0
     return len;
 }
 
 unsigned int __kfifo_get(struct kfifo *fifo,
              unsigned char *buffer, unsigned int len)
 {
     unsigned int l;
     //有数据的缓冲区的长度
     len = min(len, fifo->in - fifo->out);
     /*
      * Ensure that we sample the fifo->in index -before- we
      * start removing bytes from the kfifo.
      */
     /* first get the data from fifo->out until the end of the buffer */
     l = min(len, fifo->size - (fifo->out & (fifo->size - 1)));
     memcpy(buffer, fifo->buffer + (fifo->out & (fifo->size - 1)), l);
     /* then get the rest (if any) from the beginning of the buffer */
     memcpy(buffer + l, fifo->buffer, len - l);
     /*
      * Ensure that we remove the bytes from the kfifo -before-
      * we update the fifo->out index.
      */
     fifo->out += len; //每次累加，到达最大值后溢出，自动转为0
     return len;
 }



