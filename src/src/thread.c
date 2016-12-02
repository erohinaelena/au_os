#include "thread.h"

struct thread treads[THREAD_MAX_COUNT];
static struct spinlock thread_lock;

struct thread * create_thread(void* (*func)(void*)) {
	lock(&thread_lock);
	struct thread* t = mem_alloc(sizeof(struct thread));

	t->func = func; 
	//t->arg = arg;
	t->stack_start = mem_alloc(THREAD_STACK_SIZE);
	t->state = RUNNING;

	unlock(&thread_lock);
	return t;
}


/*void thread_switcher(struct thread* thread1, struct thread* thread2) {
	lock(&thread_lock);
    //void switch_threads(thread1, &thread2); 
	unlock(&thread_lock);
}*/


