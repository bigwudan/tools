
#ifndef EVBUFFER_INTERNAL_H_INCLUDED_
#define EVBUFFER_INTERNAL_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif


#include <sys/queue.h>
#include <stdint.h>


#define EV_UINT32_MAX ((unsigned int)0xffffffffUL)

#define MIN_BUFFER_SIZE	512

#define EVBUFFER_CHAIN_MAX EV_UINT32_MAX

#define EVBUFFER_CHAIN_SIZE sizeof(struct evbuffer_chain)
#define EVBUFFER_CHAIN_EXTRA(t, c) (t *)((struct evbuffer_chain *)(c) + 1)

#define CHAIN_SPACE_LEN(ch) ((ch)->flags  ? \
	    0 : (ch)->buffer_len - ((ch)->misalign + (ch)->off))


#define EVBUFFER_CHAIN_MAX_AUTO_SIZE 4096


#define MAX_TO_COPY_IN_EXPAND 4096
#define MAX_TO_REALIGN_IN_EXPAND 2048


#define EV_UINT64_MAX ((((uint64_t)0xffffffffUL) << 32) | 0xffffffffUL)



/** A single evbuffer callback for an evbuffer. This function will be invoked
 * when bytes are added to or removed from the evbuffer. */
struct evbuffer_cb_entry {
	/** Structures to implement a doubly-linked queue of callbacks */
	LIST_ENTRY(evbuffer_cb_entry) next;
	/** The callback function to invoke when this callback is called.
	    If EVBUFFER_CB_OBSOLETE is set in flags, the cb_obsolete field is
	    valid; otherwise, cb_func is valid. */

	/** Argument to pass to cb. */
	void *cbarg;
	/** Currently set flags on this callback. */
	uint32_t flags;
};


struct evbuffer_chain;
struct evbuffer {
	struct evbuffer_chain *first;
	struct evbuffer_chain *last;
	struct evbuffer_chain **last_with_datap;
	size_t total_len;
	size_t n_add_for_cb;
	size_t n_del_for_cb;
	unsigned own_lock : 1;
	unsigned freeze_start : 1;
	unsigned freeze_end : 1;
	unsigned deferred_cbs : 1;
	uint32_t flags;
	int refcnt;
};



/** A single item in an evbuffer. */
struct evbuffer_chain {
	/** points to next buffer in the chain */
	struct evbuffer_chain *next;

	/** total allocation available in the buffer field. */
	size_t buffer_len;

	/** unused space at the beginning of buffer or an offset into a
	 * file for sendfile buffers. */
	ssize_t misalign;

	/** Offset into buffer + misalign at which to start writing.
	 * In other words, the total number of bytes actually stored
	 * in buffer. */
	size_t off;

	/** Set if special handling is required for this chain */
	unsigned flags;


	/** number of references to this chain */
	int refcnt;

	/** Usually points to the read-write memory belonging to this
	 * buffer allocated as part of the evbuffer_chain allocation.
	 * For mmap, this can be a read-only buffer and
	 * EVBUFFER_IMMUTABLE will be set in flags.  For sendfile, it
	 * may point to NULL.
	 */
	unsigned char *buffer;
};








/** Increase the reference count of buf by one. */
void evbuffer_incref_(struct evbuffer *buf);
/** Increase the reference count of buf by one and acquire the lock. */
void evbuffer_incref_and_lock_(struct evbuffer *buf);
/** Pin a single buffer chain using a given flag. A pinned chunk may not be
 * moved or freed until it is unpinned. */
void evbuffer_chain_pin_(struct evbuffer_chain *chain, unsigned flag);
/** Unpin a single buffer chain using a given flag. */
void evbuffer_chain_unpin_(struct evbuffer_chain *chain, unsigned flag);
/** As evbuffer_free, but requires that we hold a lock on the buffer, and
 * releases the lock before freeing it and the buffer. */
void evbuffer_decref_and_unlock_(struct evbuffer *buffer);

/** As evbuffer_expand, but does not guarantee that the newly allocated memory
 * is contiguous.  Instead, it may be split across two or more chunks. */
int evbuffer_expand_fast_(struct evbuffer *, size_t, int);



/* Helper macro: copies an evbuffer_iovec in ei to a win32 WSABUF in i. */
#define WSABUF_FROM_EVBUFFER_IOV(i,ei) do {		\
		(i)->buf = (ei)->iov_base;		\
		(i)->len = (unsigned long)(ei)->iov_len;	\
	} while (0)
/* XXXX the cast above is safe for now, but not if we allow mmaps on win64.
 * See note in buffer_iocp's launch_write function */



void evbuffer_invoke_callbacks_(struct evbuffer *buf);

struct evbuffer_ptr {
	ssize_t pos;

	/* Do not alter or rely on the values of fields: they are for internal
	 * use */
	struct {
		void *chain;
		size_t pos_in_chain;
	} internal_;
};


#ifdef __cplusplus
}
#endif

#endif /* EVBUFFER_INTERNAL_H_INCLUDED_ */
