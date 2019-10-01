#include <assert.h>
#include <stdlib.h>
#include <ucontext.h>
#include "thread.h"
#include "interrupt.h"

typedef enum TStatus{
    RUNNING,
    READY,
    KILLED,
}TStatus;

/* This is the wait queue structure */
struct wait_queue {
	/* ... Fill this in Lab 3 ... */
};

/* This is the thread control block */
struct thread {
    Tid tid;
    ucontext_t context;
    stack_t stack;
    TStatus status;
    
	/* ... Fill this in ... */
};


thread* threads[THREAD_MAX_THREADS] = { NULL };
int currentlyRunningThread = -1;

Tid
search_threads(TStatus status, int searchForNull)
{
    for(int i = 0; i < THREAD_MAX_THREADS; i++){
            if(threads[i] == status){
                return i;
            }
            if(searchForNull == 1 && threads[i] == NULL){
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

void thread_stub(void (*fn)(void *), void *arg){
    Tid ret;
    fn(arg);
    ret = thread_exit();
}

Tid
thread_create(void (*fn) (void *), void *parg)
{
	int new_tid = search_threads(KILLED, 1);
	if(new_tid == -1){
            return THREAD_NOMORE;
	}
	thread newThread = (thread *)malloc(sizeof(thread));
        getcontext(newThread->context);
        stack_t newStack = (stack_t)malloc(THREAD_MIN_STACK);
	newThread->stack = newStack;
        newThread->status = READY;
        threads[new_tid] = newThread;
        threads[new_tid]->context.uc_mcontext.gregs[REG_RIP] = (long long int)(thread_stub);
        threads[new_tid]->context.uc_mcontext.gregs[REG_RSP] = (long long int)newStack;
        threads[new_tid]->context.uc_mcontext.gregs[REG_RDI] = (long long int)(fn);
        threads[new_tid]->context.uc_mcontext.gregs[REG_RSI] = (long long int)(parg);
	return new_tid;
}

Tid
thread_yield(Tid want_tid)
{
    if(want_tid == THREAD_ANY){
        
    }
}

void
thread_exit()
{
    int currentlyRunningThreadTid = search_threads(RUNNING);
    if (currentlyRunningThreadTid < 0){
        return THREAD_FAILED;
    }
    threads[currentlyRunningThreadTid]->status = KILLED;
    int readyThreadTid = search_threads(READY, -1);
    if(readyThreadTid < 0){
//        exit
        TBD();
    }
    setcontext(threads[readyThreadTid]->context);
    threads[readyThreadTid]->status = RUNNING;
}

Tid
thread_kill(Tid tid)
{
        int currentlyRunningThread = search_threads(RUNNING);
        if(tid < 0 || tid == currentlyRunningThread){
            return THREAD_INVALID;
        }
        threads[tid]->status = KILLED;
	free(threads[tid]->stack);
        free(threads[tid]);
        threads[tid] = NULL;
        return tid;
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
