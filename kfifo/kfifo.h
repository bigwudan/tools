#ifndef _KFIFO_H_
#define _KFIFO_H_
#include <stdlib.h>

//判断x是否是2的次方  
#define is_power_of_2(x) ((x) != 0 && (((x) & ((x) - 1)) == 0))  
//取a和b中最小值  
#define min(a, b) (((a) < (b)) ? (a) : (b))


struct kfifo {
	unsigned char *buffer;    /* the buffer holding the data : 用于存放数据的缓存 */
	unsigned int size;    /* the size of the allocated buffer : 空间的大小，在初化时将它向上扩展成2的幂，为了高效的进行与操作取余，后面会详解 */
	unsigned int in;    /* data is added at offset (in % size) ： 如果使用不能保证任何时间最多只有一个读线程和写线程，需要使用该lock实施同步*/
	unsigned int out;    /* data is extracted from off. (out % size) ：一起构成一个循环队列。 in指向buffer中队头，而且out指向buffer中的队尾 */
};


//根据给定buffer创建一个kfifo
struct kfifo *kfifo_init(unsigned char *buffer, unsigned int size);

//给定size分配buffer和kfifo
struct kfifo *kfifo_alloc(unsigned int size);
//释放kfifo空间
void kfifo_free(struct kfifo *fifo);

//向kfifo中添加数据
unsigned int kfifo_put(struct kfifo *fifo,
		const unsigned char *buffer, unsigned int len);

//从kfifo中取数据
unsigned int kfifo_get(struct kfifo *fifo,
		unsigned char *buffer, unsigned int len);

//获取kfifo中有数据的buffer大小 
unsigned int kfifo_len(struct kfifo *fifo);


unsigned int __kfifo_put(struct kfifo *fifo, const unsigned char *buffer, unsigned int len);

unsigned int __kfifo_get(struct kfifo *fifo, unsigned char *buffer, unsigned int len);


#endif 

