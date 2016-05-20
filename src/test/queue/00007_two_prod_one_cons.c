
#define _GNU_SOURCE
#include "ftlh.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>

#define _UNUSED __attribute__ ((unused))
#define PASS 0
#define FAIL 100
#define SKIP 77
#define HARD_FAIL 99

#include "ftlh_private.h"

#define ITEM_COUNT 3000000

ftlh_queue_t queue = NULL;
size_t odd_count = 0, even_count = 0;
pthread_mutex_t start_mutex;
ftlh_atomic64_t prod1_done = 0, prod2_done = 0, cons_done = 0;

void *producer_thread(void *start_val)
{
	uint64_t cur_val = (uint64_t)start_val, inc = 2, count = 0;
	pthread_mutex_lock(&start_mutex);
	pthread_mutex_unlock(&start_mutex);

	while(count < ITEM_COUNT) {
		ftlh_queue_enqueue(queue, (void*)cur_val);
		cur_val += inc;
		++count;
	}
	if ((uintptr_t)start_val == 1) {
		ftlh_atomic64_set(&prod1_done, 1);
	} else if ((uintptr_t)start_val == 2) {
		ftlh_atomic64_set(&prod2_done, 1);
	}
	return NULL;
}

void *consumer_thread(void *ignored __attribute__((unused)))
{
	void *value = NULL;
	pthread_mutex_lock(&start_mutex);
	pthread_mutex_unlock(&start_mutex);

	do {
		value = ftlh_queue_dequeue(queue);
		if (value == NULL) continue;
		if ((uintptr_t)value % 2 == 0) {
			ftlh_atomic64_inc(&even_count);
		} else {
			ftlh_atomic64_inc(&odd_count);
		}
	} while (!ftlh_atomic64_get(&prod1_done) || !ftlh_atomic64_get(&prod2_done) ||
			 value != NULL || ftlh_queue_approx_items(queue) != 0);
	
	ftlh_atomic64_inc(&cons_done);
	printf("Consumer finished dequeing items.\n");

	return NULL;
}

int main()
{
	pthread_t prod1, prod2, cons1;
	struct timeval start, end;
	double duration = 0;

	/* Ensure we have a queue big enough to not be over filled. We are not
	 * testing that aspect in this test case.
	 */
	printf("Creating queue...\n");
	queue = ftlh_queue_create((ITEM_COUNT * 2) + 1);
	
	printf("Checking if queue exists...\n");
	if (!queue) {
		printf("Failed to create queue.\n");
		return FAIL;
	}

	printf("Testing multi-thread enqueue with single thread dequeue...\n");
	pthread_mutex_init(&start_mutex, NULL);
	pthread_mutex_lock(&start_mutex);
	
	pthread_create(&prod1, NULL, producer_thread, (void*)1);
	pthread_create(&prod2, NULL, producer_thread, (void*)2);
	pthread_create(&cons1, NULL, consumer_thread, NULL);
	ftlh_yield(10000);

	/* Let the race begin! */
	ftlh_current_time(&start);
	pthread_mutex_unlock(&start_mutex);

	while (!ftlh_atomic64_get(&prod1_done) || !ftlh_atomic64_get(&prod2_done) ||
		   ftlh_atomic64_get(&cons_done) != 1)
	{
		ftlh_yield(1);
	}
	ftlh_current_time(&end);

	pthread_join(prod1, NULL);
	pthread_join(prod2, NULL);
	pthread_join(cons1, NULL);

	duration = (double)ftlh_time_diff_usec(&end, &start) / (double)1000000;

	/* All threads have finished, so check the counts */
	printf("Odd items received: %lu\n", ftlh_atomic64_get(&odd_count));
	printf("Even items received: %lu\n", ftlh_atomic64_get(&even_count));
	printf("Time taken in seconds: %lf\n", duration);
	printf("Enqueue / Dequeue op pairs per second: %lf\n", (double)(ITEM_COUNT * 2) / duration);
	if (ftlh_atomic64_get(&odd_count) != ITEM_COUNT ||
		ftlh_atomic64_get(&even_count) != ITEM_COUNT)
	{
		printf("Something went wrong!\n");
		return 101;
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
