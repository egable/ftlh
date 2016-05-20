
#define _GNU_SOURCE
#include "ftlh.h"
#include <stdlib.h>
#include <stdio.h>

#define _UNUSED __attribute__ ((unused))
#define PASS 0
#define FAIL 100
#define SKIP 77
#define HARD_FAIL 99

#include "ftlh_private.h"

int main()
{
	ftlh_queue_t queue = NULL;
	void *value = NULL;
	uintptr_t data = 0;
	uint64_t prod_pos = 0, cons_pos = 0;
	
	printf("Creating queue...\n");
	queue = ftlh_queue_create(256);
	
	printf("Checking if queue exists...\n");
	if (!queue) {
		printf("Failed to create queue.\n");
		return FAIL;
	}

	if (sizeof(void*) == 4) {
		data = 0xABCDEFABU;
	} else if (sizeof(void*) == 8) {
		data = 0xABCDEFABCDEFABCDUL;
	}

	printf("Adding item (0xABCDEFABCDEFABCD) to queue...\n");
	if (ftlh_queue_enqueue(queue, (void *)data) != 1) {
		printf("Failed to add item to queue.\n");
		return 101;
	}

	printf("Checking number of items in queue...\n");
	if (ftlh_queue_approx_items(queue) != 1) {
		printf("Number of items in queue not correct.\n");
		return 102;
	}

	printf("Checking location of producer and consumer pointers...\n");
	prod_pos = ftlh_atomic64_get(&queue->prod_pos);
	cons_pos = ftlh_atomic64_get(&queue->cons_pos);
	printf("prod_pos = %lu\n", prod_pos);
	printf("cons_pos = %lu\n", cons_pos);
	if (prod_pos != (ftlh_atomic64_get(&queue->size) - 1)) {
		printf("Producer position is incorrect.\n");
		return 103;
	}
	if (cons_pos != 0) {
		printf("Consumer position is incorrect.\n");
		return 104;
	}

	printf("Checking data at node 0...\n");
	if ((uintptr_t)ftlh_atomic_ptr_get(&queue->nodes[0].data) != data) {
		printf("Data stored at node 0 is incorrect.\n");
		return 105;
	}

	printf("Dequeing item from queue...\n");
	value = ftlh_queue_dequeue(queue);
	printf("Checking validity of item dequeued from queue...\n");
	if ((uintptr_t)value != data) {
		printf("Dequed item not correct: %p.\n", value);
		return 103;
	}

	printf("Checking number of items remaining in queue...\n");
	if (ftlh_queue_approx_items(queue) != 0) {
		printf("Number of items remaining in queue not correct.\n");
		return 104;
	}

	printf("Checking location of producer and consumer pointers...\n");
	prod_pos = ftlh_atomic64_get(&queue->prod_pos);
	cons_pos = ftlh_atomic64_get(&queue->cons_pos);
	printf("prod_pos = %lu\n", prod_pos);
	printf("cons_pos = %lu\n", cons_pos);
	if (prod_pos != (ftlh_atomic64_get(&queue->size) - 1)) {
		printf("Producer position is incorrect.\n");
		return 103;
	}
	if (cons_pos != prod_pos) {
		printf("Consumer position is incorrect.\n");
		return 104;
	}

	/* If this doesn't segfault, then it passes. */
	printf("Destroying queue...\n");
	ftlh_queue_destroy(&queue);

	return PASS;
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
