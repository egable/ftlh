
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
	
	printf("Creating queue...\n");
	queue = ftlh_queue_create(256);
	
	printf("Checking if queue exists...\n");
	if (!queue) {
		printf("Failed to create queue.\n");
		return FAIL;
	}

	/* If this doesn't segfault, then it passes. */
	printf("Destroying queue...\n");
	ftlh_queue_destroy(&queue);

	/* Inspecting the code for the queue, you might think we can test
	 * if the queue pointer is NULL. However, optimization will eliminate
	 * setting the pointer to NULL.
	 */

	printf("Queue apparently destroyed.\n");
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
