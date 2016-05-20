

AUTOMAKE_OPTIONS=subdir-objects

BASE_CFLAGS=-W -Wall -Werror -Wno-ignored-attributes -Wno-unknown-attributes
INCLUDE_FLAGS=-I. -I../cityhash
LIBPATH=-L.

if GCC_LINUX_DEBUG_GDB
AM_CFLAGS = $(BASE_CFLAGS) -g3 -ggdb -Og $(INCLUDE_FLAGS)
AM_CXXFLAGS = $(BASE_CFLAGS) -g3 -ggdb -Og $(INCLUDE_FLAGS)
else
AM_CFLAGS = $(BASE_CFLAGS) -O3 $(INCLUDE_FLAGS)
AM_CXXFLAGS = $(BASE_CFLAGS) -O3 $(INCLUDE_FLAGS)
endif

noinst_LIBRARIES=libftlh.a
libftlh_a_SOURCES=ftlh.c \
	ftlh_atomic.c \
	../cityhash/city.c
AM_DEFAULT_SOURCE_EXT=.c



SUBDIRS=test
