#include <locks.h>


#define LOCKED 1
#define UNLOCKED 0


void lock(struct spinlock *lock)
{
	do { 
		while (atomic_load_explicit(&lock->locked, memory_order_relaxed) == LOCKED);
	} while (atomic_exchange_explicit(&lock->locked, LOCKED, memory_order_acquire) == LOCKED);
}


void unlock(struct spinlock *lock) {
	atomic_store_explicit(&lock->locked, UNLOCKED, memory_order_release);
}
