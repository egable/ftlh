
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
	uintptr_t data = 0;
	size_t n = 0;

	printf("Creating queue...\n");
	queue = ftlh_queue_create(256);

	printf("Checking if queue exists...\n");
	if (!queue) {
		printf("Failed to create queue.\n");
		return FAIL;
	}

	printf("Checking queue size...\n");
	if (ftlh_queue_size(queue) != 256) {
		printf("Queue size is not correct.\n");
		return 101;
	}

	printf("Checking if queue nodes were allocated...\n");
	if (!queue->nodes) {
		printf("Queue nodes were not allocated.\n");
		return 102;
	}

	printf("Checking that queue items is zero...\n");
	if (ftlh_atomic64_get(&queue->items) != 0) {
		printf("Queue items is not zero: %lu\n", ftlh_atomic64_get(&queue->items));
		return 103;
	}

	printf("Checking producer position...\n");
	if (ftlh_atomic64_get(&queue->prod_pos) != 0) {
		printf("Producer position variable not zero: %lu\n",
			   ftlh_atomic64_get(&queue->prod_cons_pos));
		return 104;
	}

	printf("Checking consumer position...\n");
	if (ftlh_atomic64_get(&queue->cons_pos) != 0) {
		printf("Consumer position variable not zero: %lu\n",
			   ftlh_atomic64_get(&queue->cons_pos));
		return 104;
	}

	printf("Writing to each node allocated...\n");
	if (sizeof(void*) == 4) {
		data = 0xABCDEFABU;
	} else if (sizeof(void*) == 8) {
		data = 0xABCDEFABCDEFABCDUL;
	}
	for (n = 0; n < ftlh_atomic64_get(&queue->size); ++n) {
		printf("Writing node %lu...\n", n);
		ftlh_atomic_ptr_set(&queue->nodes[n].data, (void*)data);
	}

	printf("Queue created successfully.\n");
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
