#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <stdint.h>
#include <event.h>
#include <sys/queue.h>

#define EVBUFFER_CHAIN_MAX 0x7fffffffL

#define EVBUFFER_CHAIN_SIZE sizeof(struct evbuffer_chain)

#define EVBUFFER_CHAIN_EXTRA(t, c) (t *)((struct evbuffer_chain *)(c) + 1)


struct evbuffer_chain;
struct evbuffer {
	/** The first chain in this buffer's linked list of chains. */
	struct evbuffer_chain *first;
	/** The last chain in this buffer's linked list of chains. */
	struct evbuffer_chain *last;

	/** Pointer to the next pointer pointing at the 'last_with_data' chain.
	 *
	 * To unpack:
	 *
	 * The last_with_data chain is the last chain that has any data in it.
	 * If all chains in the buffer are empty, it is the first chain.
	 * If the buffer has no chains, it is NULL.
	 *
	 * The last_with_datap pointer points at _whatever 'next' pointer_
	 * points at the last_with_datap chain.  If the last_with_data chain
	 * is the first chain, or it is NULL, then the last_with_datap pointer
	 * is &buf->first.
	 */
	struct evbuffer_chain **last_with_datap;
	/** Total amount of bytes stored in all chains.*/
	size_t total_len;
	/** Number of bytes we have added to the buffer since we last tried to
	 * invoke callbacks. */
	size_t n_add_for_cb;
	/** Number of bytes we have removed from the buffer since we last
	 * tried to invoke callbacks. */
	size_t n_del_for_cb;
	/** Zero or more EVBUFFER_FLAG_* bits */
	uint32_t flags;
	/** Used to implement deferred callbacks. */
	struct event_base *cb_queue;
	/** A reference count on this evbuffer.	 When the reference count
	 * reaches 0, the buffer is destroyed.	Manipulated with
	 * evbuffer_incref and evbuffer_decref_and_unlock and
	 * evbuffer_free. */
	int refcnt;

};

/** A single item in an evbuffer. */
struct evbuffer_chain {
    /** points to next buffer in the chain */
    struct evbuffer_chain *next;

    /** total allocation available in the buffer field. */
    size_t buffer_len;

    /** unused space at the beginning of buffer or an offset into a
     *   * file for sendfile buffers. */
    ssize_t misalign;

    /** Offset into buffer + misalign at which to start writing.
     *   * In other words, the total number of bytes actually stored
     *       * in buffer. */
    size_t off;

    /** Set if special handling is required for this chain */
    unsigned flags;
#define EVBUFFER_FILESEGMENT    0x0001  /**< A chain used for a file segment */
#define EVBUFFER_SENDFILE   0x0002  /**< a chain used with sendfile */
#define EVBUFFER_REFERENCE  0x0004  /**< a chain with a mem reference */
#define EVBUFFER_IMMUTABLE  0x0008  /**< read-only chain */
    /** a chain that mustn't be reallocated or freed, or have its contents
     *   * memmoved, until the chain is un-pinned. */
#define EVBUFFER_MEM_PINNED_R   0x0010
#define EVBUFFER_MEM_PINNED_W   0x0020
#define EVBUFFER_MEM_PINNED_ANY (EVBUFFER_MEM_PINNED_R|EVBUFFER_MEM_PINNED_W)
    /** a chain that should be freed, but can't be freed until it is
     *   * un-pinned. */
#define EVBUFFER_DANGLING   0x0040
    /** a chain that is a referenced copy of another chain */
#define EVBUFFER_MULTICAST  0x0080

    /** number of references to this chain */
    int refcnt;

    /** Usually points to the read-write memory belonging to this
     *   * buffer allocated as part of the evbuffer_chain allocation.
     *       * For mmap, this can be a read-only buffer and
     *           * EVBUFFER_IMMUTABLE will be set in flags.  For sendfile, it
     *               * may point to NULL.
     *                   */
    unsigned char *buffer;

};




struct evbuffer * evbuffer_new(void);
int evbuffer_add(struct evbuffer *buf, const void *data_in, size_t datlen);

#endif
