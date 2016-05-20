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
	pthread_attr_t attr;
	size_t stack_size = 0;
	void *stack_addr = NULL;

	pthread_attr_init(&attr);
	pthread_attr_setstacksize(&attr, 524288);

	printf("Starting FTLH library...\n");
	ftlh_start_expert(16384,3,&attr);

	pthread_attr_destroy(&attr);

	printf("Checking if table pointers were created...\n");
	if (!ftlh_globals.tables) {
		printf("Failed to create table pointers.\n");
		return FAIL;
	}

	printf("Checking if threads array was created...\n");
	if (!ftlh_globals.threads) {
		printf("Failed to create threads array.\n");
		return 101;
	}

	printf("Checking default tables array size...\n");
	if (ftlh_globals.table_count != 16384) {
		printf("Default tables array size mismatch.\n");
		return 102;
	}

	printf("Checking default threads array size...\n");
	if (ftlh_globals.thread_count != 3) {
		printf("Default thread count mismatch.\n");
		return 103;
	}

	printf("Checking that FTLH is marked running...\n");
	if ((uintptr_t)ftlh_atomic_ptr_get(&ftlh_globals.running) != 1) {
		printf("FTLH is not marked running.\n");
		return 104;
	}

	printf("Checking that worker thread 1 is running...\n");
	if (ftlh_atomic64_get(&ftlh_globals.threads[0].running) != 1) {
		printf("Worker thread 1 is not running.\n");
		return 105;
	}

	printf("Checking that worker thread 2 is running...\n");
	if (ftlh_atomic64_get(&ftlh_globals.threads[1].running) != 1) {
		printf("Worker thread 2 is not running.\n");
		return 106;
	}

	printf("Checking that worker thread 3 is running...\n");
	if (ftlh_atomic64_get(&ftlh_globals.threads[2].running) != 1) {
		printf("Worker thread 3 is not running.\n");
		return 107;
	}

	/* Note: We don't always get the stack size we requested, but
	 *       we should get something close which is rounded up,
	 *       not down, and which is divisible by 16.
	 */
	pthread_getattr_np(ftlh_globals.threads[0].thread, &attr);
	printf("Checking worker thread 1 attributes...\n");
	pthread_attr_getstack(&attr, &stack_addr, &stack_size);
	if (!(stack_size >= 524288 && stack_size < 550000) || stack_size % 16 != 0) {
		printf("Worker thread 1 has incorrect stack size: %lu\n", stack_size);
		return 108;
	}
	pthread_attr_destroy(&attr);

	pthread_getattr_np(ftlh_globals.threads[1].thread, &attr);
	printf("Checking worker thread 2 attributes...\n");
	pthread_attr_getstack(&attr, &stack_addr, &stack_size);
	if (!(stack_size >= 524288 && stack_size < 550000) || stack_size % 16 != 0) {
		printf("Worker thread 2 has incorrect stack size: %lu\n", stack_size);
		return 109;
	}
	pthread_attr_destroy(&attr);

	pthread_getattr_np(ftlh_globals.threads[2].thread, &attr);
	printf("Checking worker thread 3 attributes...\n");
	pthread_attr_getstack(&attr, &stack_addr, &stack_size);
	if (!(stack_size >= 524288 && stack_size < 550000) || stack_size % 16 != 0) {
		printf("Worker thread 3 has incorrect stack size: %lu\n", stack_size);
		return 110;
	}
	pthread_attr_destroy(&attr);



	printf("Stopping FTLH...\n");
	ftlh_stop();

	printf("Success.\n");
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
