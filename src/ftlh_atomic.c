/* 
 * The Initial Developer of the Original Code is
 * Eliot Gable <egable@gmail.com>
 * Portions created by the Initial Developer are Copyright (C)
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 * 
 * Eliot Gable <egable@gmail.com>
 *
 * atomic.c -- Atomic Operations
 *
 * TODO: BSD / NetBSD Atomic Operation Support
 * TODO: C11 Atomic Operation Support
 *
 *
 */

#include <ftlh.h>

#ifdef WIN32
#include <windows.h>
#endif

#ifdef DARWIN
#include <libkern/OSAtomic.h>
#endif

uint32_t ftlh_atomic32_fetch_and_add(ftlh_atomic32_t *loc, uint32_t value)
{
	uint32_t ret_val = 0;
#ifdef WIN32
	ret_val = (uint32_t)InterlockedExchangeAdd((LONG*)loc, (LONG)value);
#elif DARWIN
	ret_val = OSAtomicAdd32Barrier((int32_t)value, (volatile int32_t *)loc);
#else
	ret_val = __sync_fetch_and_add(loc, value);
#endif
	return ret_val;
}

uint32_t ftlh_atomic32_fetch_and_sub(ftlh_atomic32_t *loc, uint32_t value)
{
	uint32_t ret_val = 0;
#ifdef WIN32
	ret_val = (uint32_t)InterlockedExchangeAdd((LONG*)loc, (LONG)(-1 * value));
#elif DARWIN
	ret_val = OSAtomicAdd32Barrier((int32_t)(value * -1), (volatile int32_t *)loc);
#else
	ret_val = __sync_fetch_and_sub(loc, value);
#endif
	return ret_val;
}

uint32_t ftlh_atomic32_fetch_and_or(ftlh_atomic32_t *loc, uint32_t value)
{
	uint32_t ret_val = 0;
#ifdef WIN32
	ret_val = (uint32_t)InterlockedOr((LONG*)loc, (LONG)value);
#elif DARWIN
	ret_val = OSAtomicOr32OrigBarrier(value, loc);
#else
	ret_val = __sync_fetch_and_or(loc, value);
#endif
	return ret_val;
}


uint32_t ftlh_atomic32_fetch_and_and(ftlh_atomic32_t *loc, uint32_t value)
{
	uint32_t ret_val = 0;
#ifdef WIN32
	ret_val = (uint32_t)InterlockedAnd((LONG*)loc, (LONG)value);
#elif DARWIN
	ret_val = OSAtomicAnd32OrigBarrier(value, loc);
#else
	ret_val = __sync_fetch_and_and(loc, value);
#endif
	return ret_val;
}


uint32_t ftlh_atomic32_fetch_and_xor(ftlh_atomic32_t *loc, uint32_t value)
{
	uint32_t ret_val = 0;
#ifdef WIN32
	ret_val = (uint32_t)InterlockedXor((LONG*)loc, (LONG)value);
#elif DARWIN
	ret_val = OSAtomicXor32OrigBarrier(value, loc);
#else
	ret_val = __sync_fetch_and_xor(loc, value);
#endif
	return ret_val;
}



uint64_t ftlh_atomic64_fetch_and_add(ftlh_atomic64_t *loc, uint64_t value)
{
	uint64_t ret_val = 0;
#ifdef WIN32
	ret_val = InterlockedExchangeAdd64((LONGLONG*)loc, (LONGLONG)value);
#elif DARWIN
	ret_val = OSAtomicAdd64Barrier((int64_t)value, (volatile int64_t *)loc);
#else
	ret_val = __sync_fetch_and_add(loc, value);
#endif
	return ret_val;
}


uint64_t ftlh_atomic64_fetch_and_sub(ftlh_atomic64_t *loc, uint64_t value)
{
	uint64_t ret_val = 0;
#ifdef WIN32
	ret_val = InterlockedExchangeAdd64((LONGLONG*)loc, (LONGLONG)(-1 * value));
#elif DARWIN
	ret_val = OSAtomicAdd64Barrier((int64_t)(value * -1), (volatile int64_t *)loc);
#else
	ret_val = __sync_fetch_and_sub(loc, value);
#endif
	return ret_val;
}


uint64_t ftlh_atomic64_fetch_and_or(ftlh_atomic64_t *loc, ftlh_atomic64_t value)
{
	uint64_t ret_val = 0;
#ifdef WIN32
	ret_val = InterlockedOr64((LONGLONG*)loc, (LONGLONG)value);
#elif DARWIN
	uint64_t new_val = 0;
	do {
		ret_val = ftlh_atomic64_get(loc);
		new_val = ret_val | value;
	} while (!OSAtomicCompareAndSwap64Barrier(ret_val, (volatile int64_t)new_val, (volatile int64_t *)loc));
#else
	ret_val = __sync_fetch_and_or(loc, value);
#endif
	return ret_val;
}


uint64_t ftlh_atomic64_fetch_and_and(ftlh_atomic64_t *loc, uint64_t value)
{
	uint64_t ret_val = 0;
#ifdef WIN32
	ret_val = InterlockedAnd64((LONGLONG*)loc, (LONGLONG)value);
#elif DARWIN
	uint64_t new_val = 0;
	do {
		ret_val = ftlh_atomic64_get(loc);
		new_val = ret_val & value;
	} while (!OSAtomicCompareAndSwap64Barrier(ret_val, (volatile int64_t)new_val, (volatile int64_t *)loc));
#else
	ret_val = __sync_fetch_and_and(loc, value);
#endif
	return ret_val;
}


uint64_t ftlh_atomic64_fetch_and_xor(ftlh_atomic64_t *loc, uint64_t value)
{
	uint64_t ret_val = 0;
#ifdef WIN32
	ret_val = InterlockedXor64((LONGLONG*)loc, (LONGLONG)value);
#elif DARWIN
	uint64_t new_val = 0;
	do {
		ret_val = ftlh_atomic64_get(loc);
		new_val = ret_val ^ value;
	} while (!OSAtomicCompareAndSwap64Barrier(ret_val, (volatile int64_t)new_val, (volatile int64_t *)loc));
#else
	ret_val = __sync_fetch_and_xor(loc, value);
#endif
	return ret_val;
}



uint32_t ftlh_atomic32_add_and_fetch(ftlh_atomic32_t *loc, uint32_t value)
{
	uint32_t ret_val = 0;
#ifdef WIN32
	ret_val = InterlockedExchangeAdd((LONG*)loc, (LONG)value);
	ret_val += value; /* Compute the post-operation value for return */
#elif DARWIN
	ret_val = OSAtomicAdd32Barrier((int32_t) value, (volatile int32_t *)loc);
	ret_val += value; /* Compute the post-operation value for return */
#else
	ret_val = __sync_add_and_fetch(loc, value);
#endif
	return ret_val;
}


uint32_t ftlh_atomic32_sub_and_fetch(ftlh_atomic32_t *loc, uint32_t value)
{
	uint32_t ret_val = 0;
#ifdef WIN32
	ret_val = InterlockedExchangeAdd((LONG*)loc, (LONG)(-1 * value));
	ret_val -= value; /* Compute the post-operation value for return */
#elif DARWIN
	ret_val = OSAtomicAdd32Barrier((int32_t)(value * -1), (volatile int32_t *)loc);
	ret_val -= value; /* Compute the post-operation value for return */
#else
	ret_val = __sync_sub_and_fetch(loc, value);
#endif
	return ret_val;
}


uint32_t ftlh_atomic32_or_and_fetch(ftlh_atomic32_t *loc, uint32_t value)
{
	uint32_t ret_val = 0;
#ifdef WIN32
	ret_val = InterlockedOr((LONG*)loc, (LONG)value);
	ret_val |= value; /* Compute the post-operation value for return */
#elif DARWIN
	ret_val = OSAtomicOr32Barrier(value, loc);
#else
	ret_val = __sync_or_and_fetch(loc, value);
#endif
	return ret_val;
}

uint32_t ftlh_atomic32_and_and_fetch(ftlh_atomic32_t *loc, uint32_t value)
{
	uint32_t ret_val = 0;
#ifdef WIN32
	ret_val = InterlockedAnd((LONG*)loc, (LONG)value);
	ret_val &= value; /* Compute the post-operation value for return */
#elif DARWIN
	ret_val = OSAtomicAnd32Barrier(value, loc);
#else
	ret_val = __sync_and_and_fetch(loc, value);
#endif
	return ret_val;
}


uint32_t ftlh_atomic32_xor_and_fetch(ftlh_atomic32_t *loc, uint32_t value)
{
	uint32_t ret_val = 0;
#ifdef WIN32
	ret_val = InterlockedXor((LONG*)loc, (LONG)value);
	ret_val ^= value; /* Compute the post-operation value for return */
#elif DARWIN
	ret_val = OSAtomicXor32Barrier(value, loc);
#else
	ret_val = __sync_xor_and_fetch(loc, value);
#endif
	return ret_val;
}




uint64_t ftlh_atomic64_add_and_fetch(ftlh_atomic64_t *loc, uint64_t value)
{
	uint64_t ret_val = 0;
#ifdef WIN32
	ret_val = InterlockedExchangeAdd64((LONGLONG*)loc, (LONGLONG)value);
	ret_val += value; /* Compute the post-operation value for return */
#elif DARWIN
	ret_val = OSAtomicAdd64Barrier((int64_t)value, (volatile int64_t *)loc);
	ret_val += value; /* Compute the post-operation value for return */
#else
	ret_val = __sync_add_and_fetch(loc, value);
#endif
	return ret_val;
}


uint64_t ftlh_atomic64_sub_and_fetch(ftlh_atomic64_t *loc, uint64_t value)
{
	uint64_t ret_val = 0;
#ifdef WIN32
	ret_val = InterlockedExchangeAdd64((LONGLONG*)loc, (LONGLONG)(-1 * value));
	ret_val -= value; /* Compute the post-operation value for return */
#elif DARWIN
	ret_val = OSAtomicAdd64Barrier((int64_t)(value * -1), (volatile int64_t *)loc);
	ret_val -= value; /* Compute the post-operation value for return */
#else
	ret_val = __sync_sub_and_fetch(loc, value);
#endif
	return ret_val;
}


uint64_t ftlh_atomic64_or_and_fetch(ftlh_atomic64_t *loc, uint64_t value)
{
	uint64_t ret_val = 0;
#ifdef WIN32
	ret_val = InterlockedOr64((LONGLONG*)loc, (LONGLONG)value);
	ret_val |= value; /* Compute the post-operation value for return */
#elif DARWIN
	uint64_t new_val = 0;
	do {
		ret_val = ftlh_atomic64_get(loc);
		new_val = ret_val | value;
	} while (!OSAtomicCompareAndSwap64Barrier(ret_val, (volatile int64_t)new_val, (volatile int64_t *)loc));
	ret_val = new_val;
#else
	ret_val = __sync_or_and_fetch(loc, value);
#endif
	return ret_val;
}


uint64_t ftlh_atomic64_and_and_fetch(ftlh_atomic64_t *loc, uint64_t value)
{
	uint64_t ret_val = 0;
#ifdef WIN32
	ret_val = InterlockedAnd64((LONGLONG*)loc, (LONGLONG)value);
	ret_val &= value; /* Compute the post-operation value for return */
#elif DARWIN
	uint64_t new_val = 0;
	do {
		ret_val = ftlh_atomic64_get(loc);
		new_val = ret_val & value;
	} while (!OSAtomicCompareAndSwap64Barrier(ret_val, (volatile int64_t)new_val, (volatile int64_t *)loc));
	ret_val = new_val;
#else
	ret_val = __sync_and_and_fetch(loc, value);
#endif
	return ret_val;
}


uint64_t ftlh_atomic64_xor_and_fetch(ftlh_atomic64_t *loc, uint64_t value)
{
	uint64_t ret_val = 0;
#ifdef WIN32
	ret_val = InterlockedXor64((LONGLONG*)loc, (LONGLONG)value);
	ret_val ^= value; /* Compute the post-operation value for return */
#elif DARWIN
	uint64_t new_val = 0;
	do {
		ret_val = ftlh_atomic64_get(loc);
		new_val = ret_val ^ value;
	} while (!OSAtomicCompareAndSwap64Barrier(ret_val, (volatile int64_t)new_val, (volatile int64_t *)loc));
	ret_val = new_val;
#else
	ret_val = __sync_xor_and_fetch(loc, value);
#endif
	return ret_val;
}




uint32_t ftlh_atomic32_inc(ftlh_atomic32_t *loc)
{
	uint32_t ret_val = 0;
#ifdef WIN32
	ret_val = InterlockedIncrement((LONG*)loc);
#elif DARWIN
	ret_val = OSAtomicIncrement32Barrier((volatile int32_t *)loc);
#else
	ret_val = __sync_fetch_and_add(loc, 1);
#endif
	return ret_val;
}


uint32_t ftlh_atomic32_dec(ftlh_atomic32_t *loc)
{
	uint32_t ret_val = 0;
#ifdef WIN32
	ret_val = InterlockedDecrement((LONG*)loc);
#elif DARWIN
	ret_val = OSAtomicDecrement32Barrier((volatile int32_t *)loc);
#else
	ret_val = __sync_fetch_and_sub(loc, 1);
#endif
	return ret_val;
}


uint32_t ftlh_atomic32_get(ftlh_atomic32_t *loc)
{
	uint32_t ret_val = 0;
#ifdef WIN32
	ret_val = InterlockedOr((LONG*)loc, (LONG)0);
#elif DARWIN
	do {
		ret_val = *loc;
	} while (!OSAtomicCompareAndSwap32Barrier((int32_t)ret_val, (int32_t)ret_val, (volatile int32_t *)loc));
#else
	ret_val = __sync_fetch_and_or(loc, 0);
#endif
	return ret_val;
}


uint32_t ftlh_atomic32_set(ftlh_atomic32_t *loc, uint32_t value)
{
	uint32_t ret_val = 0;
#ifdef WIN32
	do {
		ret_val = *loc;
	} while (InterlockedCompareExchange((LONG*)loc, (LONG)value, (LONG)ret_val) != ret_val);
#elif DARWIN
	do {
		ret_val = *loc;
	} while (!OSAtomicCompareAndSwap32Barrier((int32_t)ret_val, (int32_t)value, (volatile int32_t *)loc));
#else
	do {
		ret_val = *loc;
	} while (!__sync_bool_compare_and_swap(loc, ret_val, value));
#endif
	return ret_val;
}



uint64_t ftlh_atomic64_inc(ftlh_atomic64_t *loc)
{
	uint64_t ret_val = 0;
#ifdef WIN32
	ret_val = InterlockedIncrement64((LONGLONG*)loc);
#elif DARWIN
	ret_val = OSAtomicIncrement64Barrier((volatile int64_t *)loc);
#else
	ret_val = __sync_fetch_and_add(loc, 1);
#endif
	return ret_val;
}


uint64_t ftlh_atomic64_dec(ftlh_atomic64_t *loc)
{
	uint64_t ret_val = 0;
#ifdef WIN32
	ret_val = InterlockedDecrement64((LONGLONG*)loc);
#elif DARWIN
	ret_val = OSAtomicDecrement64Barrier((volatile int64_t *)loc);
#else
	ret_val = __sync_fetch_and_sub(loc, 1);
#endif
	return ret_val;
}


uint64_t ftlh_atomic64_get(ftlh_atomic64_t *loc)
{
	uint64_t ret_val = 0;
#ifdef WIN32
	ret_val = InterlockedOr64((LONGLONG*)loc, (LONGLONG)0);
#elif DARWIN
	do {
		ret_val = *loc;
	} while (!OSAtomicCompareAndSwap64Barrier((int64_t)ret_val, (int64_t)ret_val, (volatile int64_t *)loc));
#else
	ret_val = __sync_fetch_and_or(loc, 0);
#endif
	return ret_val;
}


uint64_t ftlh_atomic64_set(ftlh_atomic64_t *loc, uint64_t value)
{
	uint64_t ret_val = 0;
#ifdef WIN32
	do {
		ret_val = *loc;
	} while (InterlockedCompareExchange64((LONGLONG*)loc, (LONGLONG)value, (LONGLONG)ret_val) != ret_val);
#elif DARWIN
	do {
		ret_val = *loc;
	} while (!OSAtomicCompareAndSwap64Barrier((int64_t)ret_val, (int64_t)value, (volatile int64_t *)loc));
#else
	do {
		ret_val = *loc;
	} while (!__sync_bool_compare_and_swap(loc, ret_val, value));
#endif
	return ret_val;
}



uint_fast8_t ftlh_atomic32_bool_cas(ftlh_atomic32_t *loc, uint32_t old_value, uint32_t new_value)
{
	uint_fast8_t ret_val = 1;
#ifdef WIN32
	ret_val = (InterlockedCompareExchange((LONG*)loc, (LONG)new_value, (LONG)old_value) == old_value ? 1 : 0);
#elif DARWIN
	ret_val = (OSAtomicCompareAndSwap32Barrier((int32_t)old_value, (int32_t)new_value, (volatile int32_t *)loc) ? 1 : 0);
#else
	ret_val = (__sync_bool_compare_and_swap(loc, old_value, new_value) ? 1 : 0);
#endif
	return ret_val;
}


uint32_t ftlh_atomic32_val_cas(ftlh_atomic32_t *loc, uint32_t old_value, uint32_t new_value)
{
	ftlh_atomic32_t ret_val = 0;
#ifdef WIN32
	ret_val = InterlockedCompareExchange((LONG*)loc, (LONG)new_value, (LONG)old_value);
#elif DARWIN
	ret_val = (OSAtomicCompareAndSwap32Barrier((int32_t)old_value, (int32_t)new_value, (volatile int32_t *)loc) ? old_value : ftlh_atomic32_get(loc));
#else
	ret_val = __sync_val_compare_and_swap(loc, old_value, new_value);
#endif
	return ret_val;
}



uint_fast8_t ftlh_atomic64_bool_cas(ftlh_atomic64_t *loc, uint64_t old_value, uint64_t new_value)
{
	uint_fast8_t ret_val = 0;
#ifdef WIN32
	ret_val = (InterlockedCompareExchange64((LONGLONG*)loc, (LONGLONG)new_value, (LONGLONG)old_value) == old_value ? 1 : 0);
#elif DARWIN
	ret_val = (OSAtomicCompareAndSwap64Barrier((int64_t)old_value, (int64_t)new_value, (volatile int64_t *)loc) ? 1 : 0);
#else
	ret_val = (__sync_bool_compare_and_swap(loc, old_value, new_value) ? 1 : 0);
#endif
	return ret_val;
}


uint64_t ftlh_atomic64_val_cas(ftlh_atomic64_t *loc, uint64_t old_value, uint64_t new_value)
{
	ftlh_atomic64_t ret_val = 0;
#ifdef WIN32
	ret_val = InterlockedCompareExchange64((LONGLONG*)loc, (LONGLONG)new_value, (LONGLONG)old_value);
#elif DARWIN
	ret_val = (OSAtomicCompareAndSwap64Barrier((int64_t)old_value, (int64_t)new_value, (volatile int64_t *)loc) ? old_value : ftlh_atomic64_get(loc));
#else
	ret_val = __sync_val_compare_and_swap(loc, old_value, new_value);
#endif
	return ret_val;
}


void *ftlh_atomic_ptr_get(ftlh_atomicptr_t *loc)
{
	void *ret_val = NULL;
#ifdef WIN32
	ret_val = InterlockedCompareExchangePointer((PVOID*)loc, (PVOID)0, (PVOID)0);
#elif DARWIN
	do {
		OSMemoryBarrier();
		ret_val = (void *)*loc;    /* Assumes the location is 32-bit aligned on 32-bit systems and 64-bit aligned on 64-bit systems */
	} while (!OSAtomicCompareAndSwapPtrBarrier(ret_val, ret_val, loc));
#else
	ret_val = __sync_val_compare_and_swap(loc, 0, 0);
#endif
	return ret_val;
}

void * ftlh_atomic_ptr_set(ftlh_atomicptr_t *loc, void * value)
{
	void * ret_val = NULL;
#ifdef WIN32
	do {
		ret_val = ftlh_atomic_ptr_get(loc);
	} while (InterlockedCompareExchangePointer((PVOID*)loc, (PVOID)value, (PVOID)ret_val) != ret_val);
#elif DARWIN
	do {
		OSMemoryBarrier();
		ret_val = (void *)*loc;    /* Assumes the location is 32-bit aligned on 32-bit systems and 64-bit aligned on 64-bit systems */
	} while (!OSAtomicCompareAndSwapPtrBarrier(ret_val, value, loc));
#else
	do {
		ret_val = ftlh_atomic_ptr_get(loc);
	} while (__sync_val_compare_and_swap(loc, ret_val, value) != ret_val);
#endif
	return ret_val;
}

void *ftlh_atomic_ptr_cas(ftlh_atomicptr_t* loc, void * old_value, void * new_value)
{
	void * ret_val = NULL;
#ifdef WIN32
	ret_val = InterlockedCompareExchangePointer((PVOID*)loc, (PVOID)new_value, (PVOID)old_value);
#elif DARWIN
	ret_val = (OSAtomicCompareAndSwapPtrBarrier(old_value, new_value, loc) ? old_value : ftlh_atomic_ptr_get(loc));
#else
	ret_val = __sync_val_compare_and_swap(loc, old_value, new_value);
#endif
	return ret_val;
}


uint32_t ftlh_atomic64_inc_high32(ftlh_atomic64_t *loc)
{
	uint32_t ret_val = 0;
	uint64_t old_val = 0, new_val = 0;
#ifdef WIN32
	do {
		old_val = ftlh_atomic64_get(loc);
		ret_val = (uint32_t) ((old_val & 0xFFFFFFFF00000000ULL) >> 32);
		new_val = (((uint64_t)(ret_val + 1)) << 32) | (old_val & 0x00000000FFFFFFFFULL);
	} while(InterlockedCompareExchange64((LONGLONG*)loc, (LONGLONG)new_val, (LONGLONG)old_val) != old_val);
#elif DARWIN
	do {
		OSMemoryBarrier();
		old_val = (uint64_t)*loc;
		ret_val = (uint32_t) ((old_val & 0xFFFFFFFF00000000ULL) >> 32);
		new_val = (((uint64_t)(ret_val + 1)) << 32) | (old_val & 0x00000000FFFFFFFFULL);
	} while(!OSAtomicCompareAndSwap64Barrier(old_val, new_val, (volatile int64_t *)loc));
#else
	do {
		old_val = ftlh_atomic64_get(loc);
		ret_val = (uint32_t) ((old_val & 0xFFFFFFFF00000000ULL) >> 32);
		new_val = (((uint64_t)(ret_val + 1)) << 32) | (old_val & 0x00000000FFFFFFFFULL);
	} while(__sync_val_compare_and_swap(loc, old_val, new_val) != old_val);
#endif
	return ret_val;
}


uint32_t ftlh_atomic64_inc_low32(ftlh_atomic64_t *loc)
{
	uint32_t ret_val = 0;
	uint64_t old_val = 0, new_val = 0;
#ifdef WIN32
	do {
		old_val = ftlh_atomic64_get(loc);
		ret_val = (uint32_t) (old_val & 0x00000000FFFFFFFFULL);
		new_val = (old_val & 0xFFFFFFFF00000000ULL) | (uint64_t)(ret_val + 1);
	} while(InterlockedCompareExchange64((LONGLONG*)loc, (LONGLONG)new_val, (LONGLONG)old_val) != old_val);
#elif DARWIN
	do {
		OSMemoryBarrier();
		old_val = (uint64_t)*loc;
		ret_val = (uint32_t) (old_val & 0x00000000FFFFFFFFULL);
		new_val = (old_val & 0xFFFFFFFF00000000ULL) | (uint64_t)(ret_val + 1);
	} while(!OSAtomicCompareAndSwap64Barrier(old_val, new_val, (volatile int64_t *)loc));
#else
	do {
		old_val = ftlh_atomic64_get(loc);
		ret_val = (uint32_t) (old_val & 0x00000000FFFFFFFFULL);
		new_val = (old_val & 0xFFFFFFFF00000000ULL) | (uint64_t)(ret_val + 1);
	} while(__sync_val_compare_and_swap(loc, old_val, new_val) != old_val);
#endif
	return ret_val;
}



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
