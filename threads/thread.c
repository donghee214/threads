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
    void *stack;
    TStatus status;
	/* ... Fill this in ... */
};

Tid readyQueue[THREAD_MAX_THREADS] = { [ 0 ... THREAD_MAX_THREADS-1 ] = -1 };
int readyQueueSize = THREAD_MAX_THREADS;
int size = 0;
int last = -1;
int setcontextCalledThreads[THREAD_MAX_THREADS];
struct thread *threads[THREAD_MAX_THREADS] = { NULL };

void queueReadyThread(Tid tid)
{
    if(size < THREAD_MAX_THREADS)
    {
        readyQueue[last+1] = tid;
        last++;
        size++;
    }
    else
    {
        printf("Queue is full\n");
    }
}

Tid showNextReadyThread(){
    return readyQueue[0];
}

Tid dequeueReadyThread()
{
	int ret = readyQueue[0];
	if(ret == -1){
            return -1;
        }
        for(int i = 0; i + 1 < last; i++){
            readyQueue[i] = readyQueue[i + 1];
	}
	last -= 1;
        size--;
	return ret;
}

Tid
search_threads(TStatus status, int searchForNull)
{
    for(int i = 0; i < THREAD_MAX_THREADS; i++){
        if(searchForNull == 1 && threads[i] == NULL){
            return i;
        }
        if(threads[i] == NULL){
            continue;
        }
        if(threads[i]->status == status){
            return i;
        }
    }
    printf("No available threads");
    return -1;
}

void
thread_init(void)
{
    struct thread *initialThread = (struct thread *)malloc(sizeof(struct thread));
    initialThread->tid = 0;
    initialThread->status = RUNNING;
    threads[0] = initialThread;
}

Tid
thread_id()
{
    int currentlyRunningThread = search_threads(RUNNING, -1);
    if(currentlyRunningThread == -1){
        return 0;
    }
    return currentlyRunningThread;
}

void thread_stub(void (*fn)(void *), void *arg){
    fn(arg);
    thread_exit();
}

Tid
thread_create(void (*fn) (void *), void *parg)
{
	int new_tid = search_threads(KILLED, 1);
	if(new_tid == -1){
            return THREAD_NOMORE;
	}
	struct thread *newThread = (struct thread *)malloc(sizeof(struct thread));
	getcontext(&(newThread->context));
	void *newStack = malloc(THREAD_MIN_STACK);
	newThread->tid = new_tid;
	newThread->stack = newStack;
	newThread->status = READY;
	threads[new_tid] = newThread;
	threads[new_tid]->context.uc_mcontext.gregs[REG_RIP] = (long long int)(thread_stub);
	threads[new_tid]->context.uc_mcontext.gregs[REG_RSP] = (long long int)newStack + THREAD_MIN_STACK - 8;
	threads[new_tid]->context.uc_stack.ss_sp = newStack;
	threads[new_tid]->context.uc_stack.ss_size = THREAD_MIN_STACK - 8;
	threads[new_tid]->context.uc_mcontext.gregs[REG_RDI] = (long long int)(fn);
	threads[new_tid]->context.uc_mcontext.gregs[REG_RSI] = (long long int)(parg);
	queueReadyThread(new_tid);
	return new_tid;
}

void switch_thread(Tid currThreadID, Tid newThreadId)
{
    queueReadyThread(currThreadID);
    getcontext(&(threads[currThreadID]->context));
	setcontextCalledThreads[currThreadID] = 1;
    threads[currThreadID]->status = READY;
    threads[newThreadId]->status = RUNNING;
    setcontext(&(threads[newThreadId]->context));
}

Tid
thread_yield(Tid want_tid)
{       
		int currentlyRunningThread = search_threads(RUNNING, -1);
        if (want_tid == THREAD_SELF){
            return currentlyRunningThread;
        }
        if(want_tid == THREAD_ANY){
            int threadID = dequeueReadyThread();
            if(threadID == -1){
                return THREAD_NONE;
            }
			getcontext(&(threads[currentlyRunningThread]->context));
			if(setcontextCalledThreads[currentlyRunningThread] == 0){
				setcontextCalledThreads[currentlyRunningThread] = 1;
				threads[currentlyRunningThread]->status = READY;
				threads[currentlyRunningThread]->status = RUNNING;
				setcontext(&(threads[want_tid]->context));
			}
			else{
				setcontextCalledThreads[currentlyRunningThread] = 0;
				return threadID;
			}
        }
        if (want_tid == currentlyRunningThread){
            return currentlyRunningThread;
        }
        if (want_tid < 0 || want_tid > THREAD_MAX_THREADS){
            return THREAD_INVALID;
        }
        if (threads[want_tid] == NULL){
            return THREAD_INVALID;
        }
		else{
			int threadID = dequeueReadyThread();
			if(setcontextCalledThreads[currentlyRunningThread] == 0){
				setcontextCalledThreads[currentlyRunningThread] = 1;
				threads[currentlyRunningThread]->status = READY;
				threads[currentlyRunningThread]->status = RUNNING;
				setcontext(&(threads[threadID]->context));
			}
			else{
				setcontextCalledThreads[currentlyRunningThread] = 0;
				return threadID;
			}
		}
}

void
thread_exit()
{
    int currentlyRunningThreadTid = search_threads(RUNNING, -1);
    if (currentlyRunningThreadTid < 0){
        exit(0);
    }
    threads[currentlyRunningThreadTid]->status = KILLED;
    int readyThreadTid = search_threads(READY, -1);
    if(readyThreadTid == -1){
       exit(0);
    }
    setcontext(&(threads[readyThreadTid]->context));
    threads[readyThreadTid]->status = RUNNING;
}

Tid
thread_kill(Tid tid)
{
        int currentlyRunningThread = search_threads(RUNNING ,-1);
        if(tid < 0 || tid == currentlyRunningThread || threads[tid] == NULL){
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
