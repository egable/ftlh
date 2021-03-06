
#ifndef FTLH_PRIVATE_H
#define FTLH_PRIVATE_H

#include "ftlh.h"
#include <pthread.h>

#define ftlh_safe_free(ptr) do { if(ptr) { free(ptr); ptr = NULL; } } while(0)

struct ftlh_key_s {
	size_t len;
	char *str;
	ftlh_hash_table_t owner;
};

struct ftlh_queue_node_s {
	ftlh_atomicptr_t data;
} __attribute__ ((aligned));

struct ftlh_queue_s {
	struct ftlh_queue_node_s *nodes; /* The nodes for the queue, aligned */

	ftlh_atomic64_t prod_pos;
	ftlh_atomic64_t cons_pos;
	ftlh_atomic64_t size;      /* Size of the queue in nodes */
	ftlh_atomic64_t items;     /* Number of items in the queue, approximately. */
} __attribute__ ((aligned));

struct ftlh_thread_status_s {
	pthread_t thread;
	ftlh_atomic64_t id;
	ftlh_atomic64_t running;
};

struct ftlh_globals_s {
	ftlh_atomicptr_t *tables;
	uint_fast64_t table_count;
	struct ftlh_thread_status_s *threads;
	ftlh_atomicptr_t running;
	uint_fast32_t thread_count;
};

extern struct ftlh_globals_s ftlh_globals;


#endif // FTLH_PRIVATE_H
