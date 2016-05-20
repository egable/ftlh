

#ifndef FTLH_H
#define FTLH_H

#include <stdint.h>
#include "../cityhash/city.h"

#define FTLH_PUBLIC_FUNC __attribute__ ((used)) extern
#define FTLH_PRIVATE_FUNC static

#include "ftlh_atomic.h"


#if _WIN32 || _WIN64
#if _WIN64
#define OS64BIT
#else
#define OS32BIT
#endif
#endif

#if __GNUC__
#if __x86_64__ || __ppc64__
#define OS64BIT
#else
#define OS32BIT
#endif
#endif

#ifdef __cplusplus
#define FTLH_BEGIN_EXTERN_C       extern "C" {
#define FTLH_END_EXTERN_C         }
#else
#define FTLH_BEGIN_EXTERN_C
#define FTLH_END_EXTERN_C
#endif

/**
 * @defgroup hash Fast/Free Threaded/Threadsafe Lock-free Hash
 * @ingroup hash
 * @{
 */

/**
 * A call to ftlh_hash() will produce a hash value for the provided memory location
 * and specified number of bytes.
 *
 * @param loc The memory location to read while building the hash value.
 * @param siz The number of bytes to use to build the hash value.
 *
 * @return Returns a hash value of type ftlh_hash_t. Values of type ftlh_hash_t do
 *         not need to be freed.
 */
#define ftlh_hash(loc, siz) CityHash64(loc, siz)

/**
 * A call to ftlh_hash_key() will produce a hash value for the provided ftlh_key_t
 * object.
 *
 * @param key A validly constructed ftlh_key_t object.
 *
 * @return Returns a hash value of type ftlh_hash_t. Values of type ftlh_hash_t do
 *         not need to be freed. If the incoming ftlh_key_t is invalid, the
 *         ftlh_hash_t returned will map to hash index 0. 
 */
#define ftlh_hash_key(key) CityHash64(k->str, k->len)

/**
 * This function returns a value equivalent to a boolean TRUE if the provided
 * ftlh_hash_t values are equivalent. Otherwise, it will return a value equivalent
 * to a boolean FALSE.
 *
 * @param h1 A value of type ftlh_hash_t.
 * @param h2 A value of type ftlh_hash_t.
 */
#define ftlh_hash_eq(h1, h2) (h1 == h2) /* Done this way in case we redefine the type in the future */

/**
 * This function returns a value equivalent to a boolean FALSE if the provided
 * ftlh_hash_t values are equivalent. Otherwise, it will return a value equivalent
 * to a boolean TRUE.
 *
 * @param h1 A value of type ftlh_hash_t.
 * @param h2 A value of type ftlh_hash_t.
 */
#define ftlh_hash_neq(h1, h2) (h1 != h2) /* Done this way in case we redefine the type in the future */

/**
 * This type represents a hash value and is used to compute a hash index in the
 * hash table.
 */
typedef uint64_t ftlh_hash_t;

FTLH_BEGIN_EXTERN_C // ;

/**
 * Before calling any other ftlh_* functions, you must call ftlh_start(). You
 * *should* call ftlh_start() immediately upon entering your program.
 *
 * @return If successful, ftlh_start() will return 0. Otherwise, it will return a
 *         positive error code.
 */
FTLH_PUBLIC_FUNC uint_fast8_t ftlh_start();

/**
 * Before calling any other ftlh_* functions, you must call ftlh_start(). You
 * *should* call ftlh_start() immediately upon entering your program. If you need
 * create more than 4096 hash tables, or you need more than 2 worker threads, you can
 * use ftlh_start_advanced(). 
 *
 * @param max_tables This parameter controls the maximum number of tables the
 *                   system will support. If you hit the limit, you will receive
 *                   an error the next time you try to create a new table. Each
 *                   table requires an entry in a global array. Each entry
 *                   consumes 8 bytes of memory on a 64-bit system and 4 bytes
 *                   of memory on a 32-bit system.
 * @param threads This field specifies how many worker threads to create. A worker
 *                thread fulfills requests for the hash tables. In general, you
 *                should avoid creating a lot of worker threads. Typically, you
 *                will only need one or two, and the default is two threads. If
 *                you can concurrently execute 64 or more threads on the system,
 *                and you typically have lots of threads reading / writing to the
 *                hash table(s) heavily, you may need additional worker threads to
 *                handle the load. Regardless of how many workers you create, you
 *                will never have more than one worker for any given hash table.
 *                Thus, it is pointless to create more worker threads than you have
 *                hash tables in use.
 *
 * @return If successful, ftlh_start_advanced() will return 0. Otherwise, it will
 *         return a positive error code.
 */
FTLH_PUBLIC_FUNC uint_fast8_t ftlh_start_advanced(uint64_t max_tables, uint32_t threads);

/**
 * Before calling any other ftlh_* functions, you must call ftlh_start(). You
 * *should* call ftlh_start() immediately upon entering your program. If you need
 * create more than 4096 hash tables, or you need more than 2 worker threads, or
 * you need to change the thread attributes for the worker threads, or any other
 * default parameter of the implementation, you can use ftlh_start_expert().
 *
 * @param max_tables This parameter controls the maximum number of tables the
 *                   system will support. If you hit the limit, you will receive
 *                   an error the next time you try to create a new table. Each
 *                   table requires an entry in a global array. Each entry
 *                   consumes 8 bytes of memory on a 64-bit system and 4 bytes
 *                   of memory on a 32-bit system.
 * @param threads This field specifies how many worker threads to create. A worker
 *                thread fulfills requests for the hash tables. In general, you
 *                should avoid creating a lot of worker threads. Typically, you
 *                will only need one or two, and the default is two threads. If
 *                you can concurrently execute 64 or more threads on the system,
 *                and you typically have lots of threads reading / writing to the
 *                hash table(s) heavily, you may need additional worker threads to
 *                handle the load. Regardless of how many workers you create, you
 *                will never have more than one worker for any given hash table.
 *                Thus, it is pointless to create more worker threads than you have
 *                hash tables in use.
 * @param attr This field is a pthread_attr_t attribute pointer which specifies the
 *             the pthread attributes for the worker threads. You must have called
 *             pthread_attr_init() on the object first. By default, the only
 *             attribute FTLH sets is the stack size. FTLH uses a default stack
 *             size of 262,144 bytes. If you specify your own attributes and you
 *             have no need to change the stack size, you should set the stack
 *             size to 262,144 bytes on your attributes object.
 *
 * @return If successful, ftlh_start() will return 0. Otherwise, it will return a
 * positive error code.
 */
FTLH_PUBLIC_FUNC uint_fast8_t ftlh_start_expert(uint64_t max_tables, uint32_t threads, const pthread_attr_t *attr);

/**
 * Immediately prior to exiting your program, you must call ftlh_stop(). 
 */
FTLH_PUBLIC_FUNC void ftlh_stop();


struct ftlh_key_s;

/**
 * The ftlh_key_t type represents a hash key prior to performing the hash operation.
 */
typedef struct ftlh_key_s * ftlh_key_t __attribute__ ((aligned(16)));

/**
 * This utility function concatinates a set of strings into a single string and
 * makes note of its length. It allocates a structure to hold the key information,
 * including the newly created string and its resulting length. The returned item
 * must be freed by a call to ftlh_key_free().
 *
 * @return If an error occurs during the process (out of memory), the function will
 *         return NULL. Otherwise, it will return a fully constructed and valid
 *         ftlh_key_t which is guaranteed to accurately represent a concatenation of
 *         the input strings. 
 */
FTLH_PUBLIC_FUNC ftlh_key_t ftlh_build_key_concat(const char *str1, ...) __attribute__ ((warn_unused_result));

/**
 * This utility function creates a ftlh_key_t based on a call to printf. It expects
 * a format string in printf() style, along with an optional set of additional
 * parameters. See the documentation for printf() for further details.
 *
 * @return If an error occurs during the process (out of memory), the function will
 *         return NULL. Otherwise, it will return a fully constructed and valid
 *         ftlh_key_t which is guaranteed to accurately represent a concatenation of
 *         the input strings.
 */
FTLH_PUBLIC_FUNC ftlh_key_t ftlh_build_key_printf(const char *fmt, ...) __attribute__ ((format(printf, 1, 2)));

/**
 * This utility function builds a ftlh_key_t by reading bytes from a location in
 * memory. This allows you to use any random object (integer, double, struct, etc)
 * as a hash key.
 *
 * @param loc The location in memory where we should begin reading bytes.
 * @param len The number of bytes we should read from that location in memory.
 *
 * @return If successful, the function will return a ftlh_key_t object suitable for
 *         hashing. If the call fails for any reason (out of memory), it will return
 *         NULL.
 */
FTLH_PUBLIC_FUNC ftlh_key_t ftlh_build_key_binary(const void *loc, size_t len) __attribute__ ((warn_unused_result));

/**
 * This utility function builds a ftlh_key_t by reading bytes from multiple
 * locations in memory and concatenating them together. This allows you to use
 * multiple random objects (integer plus struct, double plus string, etc) as a hash
 * key. There are two rules to keep in mind:
 *
 * 1. Each time you pass a memory location to the function, you must follow it with
 *    a size_t sized number of bytes. 
 * 2. The only thing that can follow a specification for the number of bytes to read
 *    is another memory location to read from.
 *
 * If you follow these two rules, you should always end up passing an even number
 * of arguments.
 *
 * @param loc The location in memory where we should begin reading bytes.
 * @param len The number of bytes we should read from that location in memory.
 *
 * @return If successful, the function will return a ftlh_key_t object suitable for
 *         hashing. If the call fails for any reason (out of memory), it will return
 *         NULL.
 */
FTLH_PUBLIC_FUNC ftlh_key_t ftlh_build_key_binary_concat(const void *loc, size_t len, ...) __attribute__ ((warn_unused_result));

/**
 * This function clones an existing ftlh_key_t object. If you want to insert a key
 * into two different hash tables, you must clone it and insert the clone into the
 * other hash table.
 *
 * @param key_src This is the key you want to clone.
 *
 * @return If the passed source key is valid and memory exists, this function will
 *         return a newly created key which will pass an equality test with the
 *         source key and return the new key to the caller. The new key will not be
 *         owned by any existing hash table and can be freely inserted into another
 *         table. If not inserted into another table, it must be freed with a call
 *         to ftlh_key_free(). If the clone cannot be created, this function
 *         will return NULL.
 */
FTLH_PUBLIC_FUNC ftlh_key_t ftlh_key_clone(const ftlh_key_t key_src) __attribute__ ((warn_unused_result));

/**
 * This utility function compares two ftlh_key_t objects for equality.
 *
 * @param key1 A valid ftlh_key_t object.
 * @param key2 A valid ftlh_key_t object.
 *
 * @return If the two keys are equal, it returns a value guaranteed to evaluate in
 *         boolean logic expressions as TRUE. Otherwise, it returns a value
 *         guaranteed to evaluate in boolean expressions as FALSE.
 */
FTLH_PUBLIC_FUNC uint_fast8_t ftlh_key_eq(const ftlh_key_t key1, const ftlh_key_t key2) __attribute__ ((warn_unused_result));

/**
 * This utility function compares two ftlh_key_t objects for equality.
 *
 * @param key1 A valid ftlh_key_t object.
 * @param key2 A valid ftlh_key_t object.
 *
 * @return If the two keys are equal, it returns a value guaranteed to evaluate in
 *         boolean logic expressions as FALSE. Otherwise, it returns a value
 *         guaranteed to evaluate in boolean expressions as TRUE.
 */
#define ftlh_key_neq(k1, k2) (!ftlh_key_eq(k1, k2))

/**
 * You must always free every ftlh_key_t object returned by any call to an ftlh_*
 * function, unless you insert the key into a hash table. Once a key is inserted
 * into a hash table, you can safely ignore it. The hash table will take ownership
 * of the key and will free it when the record is deleted or the hash table is
 * cleared or destroyed. If you pass a key to ftlh_key_free() which is already owned
 * by a hash table the function will ignore the request to free the key.
 *
 * @param key This is a POINTER to an ftlh_key_t.
 *
 * @post After a call to ftlh_key_free(), the passed in key will be NULL. The caller
 *       should avoid using it again without first setting it to a different, valid
 *       key. 
 */
FTLH_PUBLIC_FUNC void ftlh_key_free(ftlh_key_t *key);


struct ftlh_hash_table_s;

/**
 * The ftlh_hash_table_t type represents a hash table object. It's really a pointer
 * to the object, so don't bother making ftlh_hash_table_t * variables.
 */
typedef struct ftlh_hash_table_s * ftlh_hash_table_t __attribute__ ((aligned(16)));


/**
 * This function pointer represents a callback status function called by
 * asynchronous operations performed on a hash table, such as ftlh_insert_async().
 */
typedef void (*ftlh_status_func_t) (ftlh_hash_table_t table, ftlh_key_t key, void *value, uint_fast8_t status);

/**
 * To create a new hash table, call ftlh_hash_table_create(). This will create a new
 * hash table for use of an appropriate size for storing your estimated number of
 * items. The table may be automatically (or forcibly) resized to a larger size
 * later, if needed. 
 *
 * @param estimated_items This value represents your best estimate for how many items
 * you will store in the hash table. The hash table created will be sized
 * appropriately for storing that many items. It is guaranteed to be larger than the
 * estimated items.
 *
 * @return Assuming all goes well, the function will return the created hash table
 *         object. If the creation fails, it will return NULL.
 */
FTLH_PUBLIC_FUNC ftlh_hash_table_t ftlh_hash_table_create(size_t estimated_items) __attribute__ ((warn_unused_result));


/**
 * You can call ftlh_insert() to insert a value into the hash table. The function
 * will return only after the KVP has been successfully inserted into the table or
 * failed to insert. 
 *
 * @param table The hash table to insert the key into.
 * @param key The key to insert into the hash table.
 * @param value The optional value to associate with the key. It can be NULL.
 *
 * @return If the key is successfully inserted, it will return a value guaranteed to
 *         evaluate to TRUE in a boolean expression. Otherwise, it will return a
 *         value guaranteed to evaluate to FALSE.
 */
FTLH_PUBLIC_FUNC uint_fast8_t ftlh_insert(ftlh_hash_table_t table, ftlh_key_t key, void *value);

/**
 * You can call ftlh_insert_async() to insert a value into the hash table. The
 * function will return immediately, regardless of whether the key can be
 * successfully inserted. You may optionally pass a callback function which will be
 * called upon finishing the insert operation. 
 *
 * @param table The hash table to insert the key into.
 * @param key The key to insert into the hash table.
 * @param value The optional value to associate with the key. It can be NULL.
 * @param cb An optional callback function pointer. Pass NULL if you don't want a
 *           callback.
 */
FTLH_PUBLIC_FUNC void ftlh_insert_async(ftlh_hash_table_t table, ftlh_key_t key, void *value, ftlh_status_func_t cb);


/**
 * You can call ftlh_replace() to replace a value associated with a key. The
 * function will return only after the KVP has been successfully replaced in the
 * table or failed to be replaced. If the key does not exist, it will attempt to
 * insert the key with the provided value.
 *
 * @param table The hash table containing the KVP to replace.
 * @param key The key whose value you wish to replace.
 * @param value The value to associate with the key. It can be NULL.
 *
 * @return If the key is found and is successfully replaced, it will return the
 *         previous value. If the the key was not present, it will insert it and
 *         return the new value if it succeeds. Otherwise, it will return NULL.
 */
FTLH_PUBLIC_FUNC void * ftlh_replace(ftlh_hash_table_t table, ftlh_key_t key, void *value);


/**
 * You can call ftlh_remove() to remove a key from the hash table. The function will
 * return only after succeeding or failing to remove the key. The only time a
 * removal would fail is if the key does not exist in the table. Because the intent
 * of the remove operation is to make the key not exist in the table, this failure
 * case is silently ignored.
 *
 * @param table The hash table containing the key to remove.
 * @param key The key to remove from the table.
 *
 * @return If the key is found and is successfully removed, it will return the
 *         value. Otherwise, it will return NULL.
 */
FTLH_PUBLIC_FUNC void * ftlh_remove(ftlh_hash_table_t table, ftlh_key_t key);

/**
 * You can call ftlh_remove_async() to remove a key from the hash table. The
 * function will return immediately, regardless of whether the KVP has been
 * successfully removed. The only time a removal would fail is if the key does not
 * exist in the table. Because the intent of the remove operation is to make the
 * key not exist in the table, this failure case is silently ignored.
 *
 * @param table The hash table containing the KVP to replace.
 * @param key The key whose value you wish to replace.
 */
FTLH_PUBLIC_FUNC void ftlh_remove_async(ftlh_hash_table_t table, ftlh_key_t key);


/**
 * @}
 */


/**
 * @defgroup misc Miscellaneous Utility Functions
 * @ingroup misc
 * @{
 */

/**
 * This function causes the executing thread to sleep for the specified number of
 * of microseconds. 
 *
 * @param usec Number of microseconds to sleep.
 */
FTLH_PUBLIC_FUNC void ftlh_yield(uint_fast64_t usec);

FTLH_PUBLIC_FUNC void ftlh_current_time(struct timeval *dest);

FTLH_PUBLIC_FUNC uint_fast64_t ftlh_time_diff_usec(struct timeval *present, struct timeval *past);

FTLH_PUBLIC_FUNC double ftlh_time_diff_sec(struct timeval *present, struct timeval *past);

/**
 * @}
 */
  
/**
 * @defgroup queue Multi-producer, Multi-consumer, Lock-free, Wait-free Queue
 * @ingroup queue
 * @{
 */

/**
 * FTLH uses a lock free multi-producer, single-consumer queue to pass messages to
 * the hash table operation worker threads. Because the queue was needed internally,
 * we also provide it publicly for general purpose use.
 */
struct ftlh_queue_s;
  
/**
 * This type represents a queue object.
 */
typedef struct ftlh_queue_s * ftlh_queue_t __attribute__ ((aligned(16)));

/**
 * This function creates a new queue of a specified size and returns it. It cannot
 * be resized. The queue can hold up to as many elements as you can get from a 32
 * bit unsigned integer, minus 1.
 *
 * @param size The number of items the queue can hold.
 *
 * @return Upon success, returns a newly allocated queue. Upon failure, returns NULL.
 */
FTLH_PUBLIC_FUNC ftlh_queue_t ftlh_queue_create(uint_fast32_t size);


/**
 * This function destroys a queue. You must ensure all threads are done using the
 * queue before attempting to destroy it.
 *
 * @param queue A pointer to the queue object to destroy.
 */
FTLH_PUBLIC_FUNC void ftlh_queue_destroy(ftlh_queue_t *queue);

/**
 * This function adds an element onto the head of the queue. If the queue is full,
 * it will block until the item can be added.
 *
 * @param value The item to add to the queue.
 *
 * @return The function call cannot fail to add the item to the queue. When it
 *         succeeds, it will return an approximate count of how many items are ahead
 *         of it in the queue. Because of race conditions, the count may not be
 *         exact. However, it will typically be within a few items of the actual
 *         count and is more likely to be >= to the count than less than the count.
 *         This can be used to detect how full the queue is and take appropriate
 *         measures to ensure you don't end up getting stuck waiting to add an item
 *         to the queue.
 */
FTLH_PUBLIC_FUNC uint_fast32_t ftlh_queue_enqueue(ftlh_queue_t queue, void *value);


/**
 * This function attempts to dequeue an item from the tail of the queue.
 *
 * @param queue The queue to dequeue the item from.
 *
 * @return Upon completion it returns the item on the tail of the queue. If no item
 *         was available, or if another consumer dequeued the item first, it will
 *         return NULL.
 */
FTLH_PUBLIC_FUNC void *ftlh_queue_dequeue(ftlh_queue_t queue);


/**
 * This function queries the queue for the original specified size and returns it.
 *
 * @param queue The queue whose size you want to know.
 *
 * @return Given a valid queue, the function cannot fail. It will return the size
 *         of the queue, which tells you how many items it can hold. If you pass it
 *         a null pointer, it will return 0.
 */
FTLH_PUBLIC_FUNC uint_fast32_t ftlh_queue_size(ftlh_queue_t queue) __attribute__ ((warn_unused_result));

/**
 * This function queries the queue for the number of items it currently holds. Keep
 * in mind that in a multithreaded environment, "current" is fleeting. By the time
 * the function returns the value it read, it may have changed. Thus, it can only
 * ever be approximately accurate.
 *
 * @param queue The queue whose item count you want to know.
 *
 * @return Given a valid queue, the function cannot fail. It will return the
 *         approximate number of items in the queue. If you pass it a NULL pointer,
 *         it will return 0. All other behavior is undefined with respect to passing
 *         it something which is not a valid queue.
 */
FTLH_PUBLIC_FUNC uint_fast32_t ftlh_queue_approx_items(ftlh_queue_t queue) __attribute__ ((warn_unused_result));

/**
 * @}
 */


FTLH_END_EXTERN_C

#endif /* FTLH_H */

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
