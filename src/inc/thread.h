#ifndef __THREAD_H__
#define __THREAD_H__
#include "locks.h"
#include "alloc.h"

#define THREAD_MAX_COUNT 4
#define THREAD_STACK_SIZE 1000

typedef enum 
{
    RUNNING, 
    FINISHED
} thread_state;

struct thread {
    void* stack_start;
	void* (*func)(void*);
	//void* arg;
	void* res;
	thread_state state;
};

struct thread* create_thread(void* (*func)(void*));
void start_thread(struct thread* t);
void join_thread(struct thread* t);
void destroy_thread(struct thread* t);

//void thread_switcher(struct thread* thread1, struct thread* thread2);
void switch_threads(void **prev, void *next);

#endif /* __THREAD_H__ */