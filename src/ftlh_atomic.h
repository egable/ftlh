/* 
 * TODO: BSD / NetBSD Atomic Operation Support
 * TODO: C11 Atomic Operation Support
 * TODO: Fix OS X Atomic Operation Support
 * TODO: Test Windows Atomic Operation Support
 *
 */
/*! @file atomic.h
    @brief Atomic Operations
	
	Basic atomic operations.

*/
#ifndef ATOMIC_H
#define ATOMIC_H

#include <stdint.h>


typedef volatile uint32_t ftlh_atomic32_t;
typedef volatile uint64_t ftlh_atomic64_t;
typedef void * volatile  ftlh_atomicptr_t;


/**
 * @defgroup atomic Multi-Threaded Atomic Operations
 * @ingroup atomic
 * @{
 */

FTLH_PUBLIC_FUNC uint32_t ftlh_atomic32_fetch_and_add(ftlh_atomic32_t *loc, uint32_t value);
FTLH_PUBLIC_FUNC uint32_t ftlh_atomic32_fetch_and_sub(ftlh_atomic32_t *loc, uint32_t value);
FTLH_PUBLIC_FUNC uint32_t ftlh_atomic32_fetch_and_or(ftlh_atomic32_t *loc, uint32_t value);
FTLH_PUBLIC_FUNC uint32_t ftlh_atomic32_fetch_and_and(ftlh_atomic32_t *loc, uint32_t value);
FTLH_PUBLIC_FUNC uint32_t ftlh_atomic32_fetch_and_xor(ftlh_atomic32_t *loc, uint32_t value);

FTLH_PUBLIC_FUNC uint64_t ftlh_atomic64_fetch_and_add(ftlh_atomic64_t *loc, uint64_t value);
FTLH_PUBLIC_FUNC uint64_t ftlh_atomic64_fetch_and_sub(ftlh_atomic64_t *loc, uint64_t value);
FTLH_PUBLIC_FUNC uint64_t ftlh_atomic64_fetch_and_or(ftlh_atomic64_t *loc, uint64_t value);
FTLH_PUBLIC_FUNC uint64_t ftlh_atomic64_fetch_and_and(ftlh_atomic64_t *loc, uint64_t value);
FTLH_PUBLIC_FUNC uint64_t ftlh_atomic64_fetch_and_xor(ftlh_atomic64_t *loc, uint64_t value);

FTLH_PUBLIC_FUNC uint32_t ftlh_atomic32_add_and_fetch(ftlh_atomic32_t *loc, uint32_t value);
FTLH_PUBLIC_FUNC uint32_t ftlh_atomic32_sub_and_fetch(ftlh_atomic32_t *loc, uint32_t value);
FTLH_PUBLIC_FUNC uint32_t ftlh_atomic32_or_and_fetch(ftlh_atomic32_t *loc, uint32_t value);
FTLH_PUBLIC_FUNC uint32_t ftlh_atomic32_and_and_fetch(ftlh_atomic32_t *loc, uint32_t value);
FTLH_PUBLIC_FUNC uint32_t ftlh_atomic32_xor_and_fetch(ftlh_atomic32_t *loc, uint32_t value);

FTLH_PUBLIC_FUNC uint64_t ftlh_atomic64_add_and_fetch(ftlh_atomic64_t *loc, uint64_t value);
FTLH_PUBLIC_FUNC uint64_t ftlh_atomic64_sub_and_fetch(ftlh_atomic64_t *loc, uint64_t value);
FTLH_PUBLIC_FUNC uint64_t ftlh_atomic64_or_and_fetch(ftlh_atomic64_t *loc, uint64_t value);
FTLH_PUBLIC_FUNC uint64_t ftlh_atomic64_and_and_fetch(ftlh_atomic64_t *loc, uint64_t value);
FTLH_PUBLIC_FUNC uint64_t ftlh_atomic64_xor_and_fetch(ftlh_atomic64_t *loc, uint64_t value);

FTLH_PUBLIC_FUNC uint32_t ftlh_atomic32_inc(ftlh_atomic32_t *loc);
FTLH_PUBLIC_FUNC uint32_t ftlh_atomic32_dec(ftlh_atomic32_t *loc);
FTLH_PUBLIC_FUNC uint32_t ftlh_atomic32_get(ftlh_atomic32_t *loc);
FTLH_PUBLIC_FUNC uint32_t ftlh_atomic32_set(ftlh_atomic32_t *loc, uint32_t value);

FTLH_PUBLIC_FUNC uint64_t ftlh_atomic64_inc(ftlh_atomic64_t *loc);
FTLH_PUBLIC_FUNC uint64_t ftlh_atomic64_dec(ftlh_atomic64_t *loc);
FTLH_PUBLIC_FUNC uint64_t ftlh_atomic64_get(ftlh_atomic64_t *loc);
FTLH_PUBLIC_FUNC uint64_t ftlh_atomic64_set(ftlh_atomic64_t *loc, uint64_t value);

FTLH_PUBLIC_FUNC uint_fast8_t ftlh_atomic32_bool_cas(ftlh_atomic32_t *loc, uint32_t old_value, uint32_t new_value);
FTLH_PUBLIC_FUNC uint32_t ftlh_atomic32_val_cas(ftlh_atomic32_t *loc, uint32_t old_value, uint32_t new_value);

FTLH_PUBLIC_FUNC uint_fast8_t ftlh_atomic64_bool_cas(ftlh_atomic64_t *loc, uint64_t old_value, uint64_t new_value);
FTLH_PUBLIC_FUNC uint64_t ftlh_atomic64_val_cas(ftlh_atomic64_t *loc, uint64_t old_value, uint64_t new_value);

FTLH_PUBLIC_FUNC void *ftlh_atomic_ptr_get(ftlh_atomicptr_t *loc);
FTLH_PUBLIC_FUNC void *ftlh_atomic_ptr_set(ftlh_atomicptr_t *loc, void *value);
FTLH_PUBLIC_FUNC void *ftlh_atomic_ptr_cas(ftlh_atomicptr_t *loc, void *old_value, void *new_value);

FTLH_PUBLIC_FUNC uint32_t ftlh_atomic64_inc_high32(ftlh_atomic64_t *loc);
FTLH_PUBLIC_FUNC uint32_t ftlh_atomic64_inc_low32(ftlh_atomic64_t *loc);


/** @} */
#endif

/* For Emacs:
 * Local Variables:
 * mode:c
 * indent-tabs-mode:t
 * tab-width:4
 * c-basic-offset:4
 * End:
 * For VIM:
 * vim:set softtabstop=4 shiftwidth=4 tabstop=4:
 */
