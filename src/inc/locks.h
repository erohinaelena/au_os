#ifndef __LOCKS_H__
#define __LOCKS_H__
#include <stdatomic.h>

struct spinlock {
	atomic_int locked;
};

void lock(struct spinlock* lock);
void unlock(struct spinlock* lock);

#endif /* __LOCKS_H__ */
