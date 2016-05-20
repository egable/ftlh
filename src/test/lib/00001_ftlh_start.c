
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
	printf("Starting FTLH library...\n");
	ftlh_start();

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
	if (ftlh_globals.table_count != 4096) {
		printf("Default tables array size mismatch.\n");
		return 102;
	}

	printf("Checking default threads array size...\n");
	if (ftlh_globals.thread_count != 2) {
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
		return 105;
	}

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
