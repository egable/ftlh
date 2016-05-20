
#define _GNU_SOURCE
#include "ftlh.h"
#include <sys/select.h>
#include <sys/time.h>
#include <assert.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>

#include "ftlh_private.h"

FTLH_PRIVATE_FUNC inline void * ftlh_zalloc_aligned(size_t size) __attribute__ ((malloc, alloc_size(1), warn_unused_result, always_inline));
FTLH_PRIVATE_FUNC inline void ftlh_free_aligned(void *ptr) __attribute__ ((always_inline));

FTLH_PRIVATE_FUNC void *ftlh_hash_worker_thread(void *thid);


ftlh_queue_t ftlh_queue_create(uint_fast32_t size)
{
	ftlh_queue_t queue = NULL;

	/* Perform basic sanity checks first. */
	if (size < 64) size = 64;

	/* We want to malloc an aligned section of memory for the queue */
	queue = ftlh_zalloc_aligned(sizeof(struct ftlh_queue_s));
	if (!queue) {
		/* out of memory error */
		goto done;
	}

	/* Store the size of the queue we are creating */
	queue->size = size;

	/* Allocate an aligned memory segment for the nodes */
	queue->nodes = (struct ftlh_queue_node_s *)ftlh_zalloc_aligned(sizeof(struct ftlh_queue_node_s) * size);
	if (!queue->nodes) {
		/* out of memory error */
		goto fail;
	}

	//ftlh_atomic64_set(&queue->prod_cons_pos, ((uint_fast64_t)(size-1) << 32));
  
	goto done;

 fail:
	__attribute__ ((cold));
	/* If we failed after alloc'ing queue, clean it up */
	ftlh_free_aligned(queue);
	queue = NULL;

 done:
	__attribute__ ((hot));
	return queue;
}


void ftlh_queue_destroy(ftlh_queue_t *queue_in)
{
	ftlh_queue_t queue = NULL;

	/* Basic sanity checks */
	if (!queue) {
		goto done;
	}

	queue = *queue_in;
	queue->size = 0;
	ftlh_free_aligned(queue->nodes);
	ftlh_free_aligned(queue);

	/* This is for debugging only. It is removed by the compiler
	 * when optimizing. Don't rely on it. */
	*queue_in = NULL;

 done:
	return;
}

uint_fast64_t ftlh_queue_size(ftlh_queue_t queue)
{
	if (!queue) {
		return 0;
	}
	return queue->size;
}


uint_fast64_t ftlh_queue_enqueue(ftlh_queue_t queue, void *value)
{
	uint_fast64_t items = 0;
	//uint_fast64_t prod_cons_pos = 0, new_prod_cons_pos = 0;
	uint_fast64_t orig_prod_pos = 0, prod_pos = 0, cons_pos = 0;

	assert(queue);

	/* Obtain the producer / consumer position value
	 * Update the producer location
	 * Re-encode the values into a new 64-bit value
	 * Attempt to store the new value
	 * If it was changed on us, try it all over again
	 * Also, back off if we fill the queue
	 */
 again:
	do {
		void *node_value = NULL;

		prod_pos = ftlh_atomic64_get(&queue->prod_pos);

		/* Producers might catch the consumers and need to wait for the clearing of data from a node before writing */
		node_value = ftlh_atomic_ptr_get(&queue->nodes[prod_pos].data);
		if (node_value) {
			ftlh_yield(10);
			goto again;
		}

		cons_pos = ftlh_atomic64_get(&queue->cons_pos);
		if (prod_pos == cons_pos+2 || (prod_pos == 0 && cons_pos == ftlh_atomic64_get(&queue->size) - 2) ||
			(prod_pos == 1 && cons_pos == ftlh_atomic64_get(&queue->size) - 1))
		{
			/* Producer wrapped and caught consumer; we should back off */
			ftlh_yield(10);
			goto again;
		}

		/* The node is empty, and we didn't wrap and catch the consumer(s) */
		orig_prod_pos = prod_pos;
		if (prod_pos != 0) {
			--prod_pos;
		} else {
			prod_pos = ftlh_atomic64_get(&queue->size) - 1;
		}

	} while (!ftlh_atomic64_bool_cas(&queue->prod_pos, orig_prod_pos, prod_pos));
	
	/* At this point, we just moved the producer to a new node (we won the race),
	 * and no other producer should see this node. A consumer may be inspecting
	 * this node, but won't touch it because the state is not full. It is now
	 * safe to store the values in this node.
	 */
	ftlh_atomic_ptr_set(&queue->nodes[orig_prod_pos].data, value);

	/* We can now increment the number of items in the queue. */
	ftlh_atomic64_inc(&queue->items);

	/* Read the current number of items -- which may have already changed, or may
	 * change again before we return from this function. */
	items = ftlh_atomic64_get(&queue->items);

	return items;
}

void *ftlh_queue_dequeue(ftlh_queue_t queue)
{
	void *value = NULL;
	//uint_fast64_t prod_cons_pos = 0, new_prod_cons_pos = 0;
	uint_fast64_t cons_pos = 0, orig_cons_pos = 0;

	assert(queue);

	/* Obtain the producer / consumer position value
	 * Validate we are pointing at a full node
	 * Update the consumer location
	 * Re-encode the values into a new 64-bit value
	 * Attempt to store the new value
	 * If it was changed on us, try it all over again
	 */
	do {
		cons_pos = ftlh_atomic64_get(&queue->cons_pos);

		value = ftlh_atomic_ptr_get(&queue->nodes[cons_pos].data);
		if (!value) {
			goto fail;
		}

		orig_cons_pos = cons_pos;
		if (cons_pos != 0) {
			--cons_pos;
		} else {
			cons_pos = ftlh_atomic64_get(&queue->size) - 1;
		}

	} while (!ftlh_atomic64_bool_cas(&queue->cons_pos, orig_cons_pos, cons_pos));
	
	/* At this point, we just moved the consumer position to a new node,
	 * and no other consumer should see this node. A producer may see
	 * this node, but should not touch it because it is full.
	 */
	ftlh_atomic_ptr_set(&queue->nodes[orig_cons_pos].data, NULL);

	/* We can now decrement the number of items in the queue. */
	ftlh_atomic64_dec(&queue->items);


 fail:
	return value;
}


uint_fast64_t ftlh_queue_approx_items(ftlh_queue_t queue)
{
	if (queue) {
		return ftlh_atomic64_get(&queue->items);
	}
	return 0;
}



struct ftlh_globals_s ftlh_globals = {0};

uint_fast8_t ftlh_start()
{
	return ftlh_start_advanced(4096, 2);
}

uint_fast8_t ftlh_start_advanced(uint64_t max_tables, uint32_t threads)
{
	uint_fast8_t ret_val = 0;
	pthread_attr_t attr;

	if ((errno = pthread_attr_init(&attr)) != 0) {
		ret_val = 1;
		goto fail1;
	}

	if ((errno = pthread_attr_setstacksize(&attr, 262144)) != 0) {
		ret_val = 1;
		goto fail2;
	}

	ret_val = ftlh_start_expert(max_tables, threads, &attr);

 fail2:
	if ((errno = pthread_attr_destroy(&attr)) != 0) {
		ret_val = 1;
	}

 fail1:
	return ret_val;
}

uint_fast8_t ftlh_start_expert(uint64_t max_tables, uint32_t threads, const pthread_attr_t *attr)
{
	uint_fast8_t status = 0;
	uint_fast32_t th = 0;

	ftlh_globals.tables = ftlh_zalloc_aligned(sizeof(ftlh_atomicptr_t) * max_tables);
	if (!ftlh_globals.tables) {
		errno = ENOMEM;
		status = 1;
		goto done;
	}

	ftlh_globals.table_count = max_tables;

	ftlh_globals.threads = ftlh_zalloc_aligned(sizeof(struct ftlh_thread_status_s) * threads);

	if (!ftlh_globals.threads) {
		errno = ENOMEM;
		status = 1;
		goto fail;
	}

	ftlh_globals.thread_count = threads;
	ftlh_atomic_ptr_set(&ftlh_globals.running, (void*)1);

	for (th = 0; th < ftlh_globals.thread_count; ++th) {
		ftlh_globals.threads[th].id = th;
		pthread_create(&ftlh_globals.threads[th].thread, attr, ftlh_hash_worker_thread, (void *)&ftlh_globals.threads[th]);
	}

	for (th = 0; th < ftlh_globals.thread_count; ++th) {
		while(ftlh_atomic64_get(&ftlh_globals.threads[th].running) != 1) {
			ftlh_yield(100);
		}
	}

	goto done;
 fail:
	__attribute__ ((cold));
	ftlh_free_aligned((void*)ftlh_globals.tables);
	
 done:
	__attribute__ ((hot));
	return status;
}

void ftlh_stop()
{
	uint_fast32_t th = 0;

	ftlh_atomic_ptr_set(&ftlh_globals.running, (void*)0);

	for (th = 0; th < ftlh_globals.thread_count; ++th) {
		pthread_join(ftlh_globals.threads[th].thread, NULL);
	}
	ftlh_free_aligned(ftlh_globals.threads);
	ftlh_free_aligned((void*)ftlh_globals.tables);
}


void *ftlh_hash_worker_thread(void *info)
{
	uint_fast64_t thread_id = 0;
	uint_fast64_t table_count = ftlh_globals.table_count;
	struct ftlh_thread_status_s *thread_info = (struct ftlh_thread_status_s *)info;

	ftlh_atomic64_set(&thread_info->running, 1);
	thread_id = ftlh_atomic64_get(&thread_info->id);

	while ((uintptr_t)ftlh_atomic_ptr_get(&ftlh_globals.running) == 1) {
		int_fast64_t table_num = table_count - 1;
		for (; table_num != 0; --table_num) {
			/* Skip tables not handled by this thread */
			if (table_num % table_count != thread_id) continue;

			
		}
		ftlh_yield(10000);
	}
	ftlh_atomic64_set(&thread_info->running, 0);


	return NULL;
}





/**************************************************
 * Private functions follow
 *************************************************/

void * ftlh_zalloc_aligned(size_t size)
{
	uintptr_t *storage = NULL;

	/* Allocate zeroed memory big enough for an extra pointer and alignment */
	void *start = calloc(1, size + 15 + sizeof(uintptr_t));

	/* Compute the aligned location */
	void *aligned = (void *)(((uintptr_t)start + 15 + sizeof(uintptr_t)) & ~((uintptr_t)0x0F));

	storage = (uintptr_t *)((uintptr_t)aligned - sizeof(uintptr_t));
	/* Back up by the size of uintptr_t and store the start of the block */
	*storage = (uintptr_t)start;

	return aligned;
}

void ftlh_free_aligned(void *ptr)
{
	/* Back up the size of uintptr_t and retrieve the start of the block */
	/* Syntactic explaination: Convert ptr to uintptr_t so we can do arithmetic on it
	 * without emitting a warning. Then, subtract the size of a uintptr_t to find
	 * where we stored the start address. Convert that computation into a uintptr_t pointer
	 * so that we can dereference the location and retrieve the value. Finally,
	 * convert that retrieved numeric value into a void pointer so we can free it.
	 */
	void *start = (void *)(*(uintptr_t *)((uintptr_t)ptr - sizeof(uintptr_t)));
	free(start);
}

void ftlh_yield(uint_fast64_t usec)
{
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = usec;
	select(0, NULL, NULL, NULL, &tv);
}


void ftlh_current_time(struct timeval *dest)
{
	gettimeofday(dest, NULL);
}

uint_fast64_t ftlh_time_diff_usec(struct timeval *present, struct timeval *past)
{
	uint_fast64_t present64 = 0, past64 = 0;

	if (!present || !past) return 0;

	present64 = ((uint_fast64_t)present->tv_sec * 1000000) + present->tv_usec;
	past64 = ((uint_fast64_t)past->tv_sec * 1000000) + past->tv_usec;

	return present64 - past64;
}

double ftlh_time_diff_sec(struct timeval *present, struct timeval *past)
{
	double presentd = 0, pastd = 0;

	if (!present || !past) return 0;

	presentd = ((double)present->tv_sec + ((double)present->tv_usec / (double)1000000));
	pastd = ((double)past->tv_sec + ((double)present->tv_usec / (double)1000000));

	return presentd - pastd;
}


/* For Emacs:
 * Local Variables:
 * mode:c
 * indent-tabs-mode:t
 * tab-width:4
 * c-basic-offset:4
 * End:
 * For VIM:
 * vim:set softtabstop=4 shiftwidth=4 tabstop=4 noet:
 */
