#include <assert.h>
#include <stdlib.h>
#include <ucontext.h>
#include "thread.h"
#include "interrupt.h"

/* This is the wait queue structure */
struct wait_queue {
	/* ... Fill this in Lab 3 ... */
};

/* This is the thread control block */
struct thread {
	Tid tid;
	ucontext context;
	/* ... Fill this in ... */
};


thread* threads[THREAD_MAX_THREADS] = { NULL }
int currentlyRunningThread = -1;

Tid
create_new_tid(void)
{
	for(int i = 0; i < THREAD_MAX_THREADS; i++){
		if(threads[i] == NULL){
			return i;
		}
	}
	printf("No available threads");
	return -1;
}

void
thread_init(void)
{
	/* your optional code here */
}

Tid
thread_id()
{
	if(currentlyRunningThread > -1){
		return currentlyRunningThread;
	}
	printf("No running thread");
	return -1;
}

Tid
thread_create(void (*fn) (void *), void *parg)
{
	int new_tid = create_new_tid();
	if(new_tid == -1){
		return THREAD_NOMORE
	}
	threads[new_tid] = (thread *)malloc(sizeof(thread));
	currentlyRunningThread = new_tid
	return THREAD_FAILED;
}

Tid
thread_yield(Tid want_tid)
{
	TBD();
	return THREAD_FAILED;
}

void
thread_exit()
{
	
}

Tid
thread_kill(Tid tid)
{
	TBD();
	return THREAD_FAILED;
}

/*******************************************************************
 * Important: The rest of the code should be implemented in Lab 3. *
 *******************************************************************/

/* make sure to fill the wait_queue structure defined above */
struct wait_queue *
wait_queue_create()
{
	struct wait_queue *wq;

	wq = malloc(sizeof(struct wait_queue));
	assert(wq);

	TBD();

	return wq;
}

void
wait_queue_destroy(struct wait_queue *wq)
{
	TBD();
	free(wq);
}

Tid
thread_sleep(struct wait_queue *queue)
{
	TBD();
	return THREAD_FAILED;
}

/* when the 'all' parameter is 1, wakeup all threads waiting in the queue.
 * returns whether a thread was woken up on not. */
int
thread_wakeup(struct wait_queue *queue, int all)
{
	TBD();
	return 0;
}

/* suspend current thread until Thread tid exits */
Tid
thread_wait(Tid tid)
{
	TBD();
	return 0;
}

struct lock {
	/* ... Fill this in ... */
};

struct lock *
lock_create()
{
	struct lock *lock;

	lock = malloc(sizeof(struct lock));
	assert(lock);

	TBD();

	return lock;
}

void
lock_destroy(struct lock *lock)
{
	assert(lock != NULL);

	TBD();

	free(lock);
}

void
lock_acquire(struct lock *lock)
{
	assert(lock != NULL);

	TBD();
}

void
lock_release(struct lock *lock)
{
	assert(lock != NULL);

	TBD();
}

struct cv {
	/* ... Fill this in ... */
};

struct cv *
cv_create()
{
	struct cv *cv;

	cv = malloc(sizeof(struct cv));
	assert(cv);

	TBD();

	return cv;
}

void
cv_destroy(struct cv *cv)
{
	assert(cv != NULL);

	TBD();

	free(cv);
}

void
cv_wait(struct cv *cv, struct lock *lock)
{
	assert(cv != NULL);
	assert(lock != NULL);

	TBD();
}

void
cv_signal(struct cv *cv, struct lock *lock)
{
	assert(cv != NULL);
	assert(lock != NULL);

	TBD();
}

void
cv_broadcast(struct cv *cv, struct lock *lock)
{
	assert(cv != NULL);
	assert(lock != NULL);

	TBD();
}
