
AUTOMAKE_OPTIONS=subdir-objects

BASE_CFLAGS=-W -Wall -Werror -Wno-ignored-attributes -Wno-unknown-attributes
INCLUDE_FLAGS=-I. -I../../cityhash -I../
LIBPATH=-L. -L../

if GCC_LINUX_DEBUG_GDB
AM_CFLAGS = $(BASE_CFLAGS) -g3 -ggdb -Og $(INCLUDE_FLAGS)
AM_CXXFLAGS = $(BASE_CFLAGS) -g3 -ggdb -Og $(INCLUDE_FLAGS)
else
AM_CFLAGS = $(BASE_CFLAGS) -O3 $(INCLUDE_FLAGS)
AM_CXXFLAGS = $(BASE_CFLAGS) -O3 $(INCLUDE_FLAGS)
endif


LDADD=../libftlh.a -lpthread
check_PROGRAMS=queue/00001_create \
	queue/00002_destroy \
	queue/00003_align_struct \
	queue/00004_align_nodes \
	queue/00005_single_thread_enqueue \
	queue/00006_single_thread_enqueue_dequeue \
	queue/00007_two_prod_one_cons \
	queue/00008_two_prod_two_cons \
	queue/00009_queue_full \
	lib/00001_ftlh_start \
	lib/00002_ftlh_start_advanced \
	lib/00003_ftlh_start_expert
AM_DEFAULT_SOURCE_EXT=.c
AM_COLOR_TESTS=always
TESTS=$(check_PROGRAMS)
