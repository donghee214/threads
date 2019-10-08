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
    void *stack_address;
    int setcontext_called;
    TStatus status;
	/* ... Fill this in ... */
};

Tid readyQueue[THREAD_MAX_THREADS] = { [ 0 ... THREAD_MAX_THREADS-1 ] = -1 };
int readyQueueSize = THREAD_MAX_THREADS;
int size = 0;
int last = 0;
Tid currRunningThread;
// volatile int setcontextCalledThreads[THREAD_MAX_THREADS];
struct thread *threads[THREAD_MAX_THREADS] = { NULL };


void queueReadyThread(Tid tid)
{ 
    for(int i = 0; i < THREAD_MAX_THREADS; i++){
        if(readyQueue[i] == -1){
            readyQueue[i] = tid;
            break;
        }
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
    for(int i = 0; i < THREAD_MAX_THREADS - 1; i++){
        readyQueue[i] = readyQueue[i + 1];
        if(i == 1022){
            readyQueue[1023] = -1;
        }
    }
    return ret;
}

Tid dequeueIdReadyThread(Tid id)
{
//	printf("dequque id: %d\n", id);
	int index = -1;
	for(int i = 0; i < THREAD_MAX_THREADS; i++){
            if(readyQueue[i] == id){
                index = i;
            }
	}
	for(int j = index; j < THREAD_MAX_THREADS - 1; j++){
            readyQueue[j] = readyQueue[j + 1];
            if(j == 1022){
                readyQueue[1023] = -1;
            }
	}
	return id;
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
    return -1;
}

void
thread_init(void)
{
    struct thread *initialThread = (struct thread *)malloc(sizeof(struct thread));
    initialThread->tid = 0;
    initialThread->status = RUNNING;
    initialThread->setcontext_called = 0;
    threads[0] = initialThread;
    currRunningThread = 0;
}

Tid
thread_id()
{
//    int currentlyRunningThread = search_threads(RUNNING, -1);
//    if(currentlyRunningThread == -1){
//        return 0;
//    }
    return currRunningThread;
}

void thread_stub(void (*fn)(void *), void *arg){
    for(int i = 0; i < THREAD_MAX_THREADS; i++){
        if(threads[i] != NULL){
             if(threads[i]->status == KILLED){
//                 if(threads[i]->tid == 0){
//                     printf("uh oh");
//                     exit(0);
//                 }
//                dequeueIdReadyThread(threads[i]->tid);
                free(threads[i]->stack_address);
                free(threads[i]);
                threads[i] = NULL;
             }
        }
    }
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
        if(newStack == NULL){
            free(newThread);
            return THREAD_NOMEMORY;
        }
	newThread->tid = new_tid;
	newThread->stack_address = newStack;
	newThread->status = READY;
	newThread->setcontext_called = 0;
	threads[new_tid] = newThread;
	threads[new_tid]->context.uc_mcontext.gregs[REG_RIP] = (long long int)&thread_stub;
	threads[new_tid]->context.uc_mcontext.gregs[REG_RSP] = (long long int)newStack + THREAD_MIN_STACK - 8;
	threads[new_tid]->context.uc_stack.ss_size = THREAD_MIN_STACK - 8;
	threads[new_tid]->context.uc_mcontext.gregs[REG_RDI] = (long long int)(fn);
	threads[new_tid]->context.uc_mcontext.gregs[REG_RSI] = (long long int)(parg);
	queueReadyThread(new_tid);
	return new_tid;
}

Tid
thread_yield(Tid want_tid)
{   
//           displayReadyQueue();
//	int currentlyRunningThread = search_threads(RUNNING, -1);
	if (want_tid == THREAD_SELF){
		return currRunningThread;
	}
	if (want_tid == currRunningThread){
		return currRunningThread;
	}
	if (want_tid < -2 || want_tid > THREAD_MAX_THREADS){
		return THREAD_INVALID;
	}
	if (want_tid > 0 && threads[want_tid] == NULL){
		return THREAD_INVALID;
	}
	if(want_tid == THREAD_ANY){
            int threadID = dequeueReadyThread();
//            printf("current thread, %d", currRunningThread);
//            printf("threadId, %d\n", threadID);
            while(threadID != -1 && threads[threadID]->status == KILLED){
//                if(threadID == 0){
//                    printf("uh oh2");
//                    exit(0);
//                }
//                dequeueIdReadyThread(threadID);
                free(threads[threadID]->stack_address);
                free(threads[threadID]);
                threads[threadID] = NULL;
                threadID = dequeueReadyThread();
            }
            if(threadID == -1){
                    return THREAD_NONE;
            }
            queueReadyThread(currRunningThread);
            getcontext(&(threads[currRunningThread]->context));
            if(threads[currRunningThread]->setcontext_called == 0){
                threads[currRunningThread]->setcontext_called = 1;
                currRunningThread = threadID;
//                printf("SWITCHING TO THREAD: %d\n", threadID);
                setcontext(&(threads[threadID]->context));
            }
            else{
                threads[currRunningThread]->setcontext_called = 0;
                return threadID;
            }
	}
	else{
            if(threads[want_tid] == NULL){
                return THREAD_NONE;
            }
            if(threads[want_tid]->status == KILLED){
                dequeueIdReadyThread(want_tid);
                free(threads[want_tid]->stack_address);
                free(threads[want_tid]);
                threads[want_tid] = NULL;
                return THREAD_INVALID;
            }
            queueReadyThread(currRunningThread);
            want_tid = dequeueIdReadyThread(want_tid);
            getcontext(&(threads[currRunningThread]->context));
//            printf("current thread, %d\n", currRunningThread);
//            printf("next threadId, %d\n", want_tid);
            if(threads[currRunningThread]->setcontext_called == 0){
                threads[currRunningThread]->setcontext_called = 1;
                currRunningThread = want_tid;
//                printf("about to switch to, %d", want_tid);
                setcontext(&(threads[want_tid]->context));
            }
            else{
                threads[currRunningThread]->setcontext_called = 0;

                return want_tid;
            }
	}
	return THREAD_NONE;
}

void
thread_exit()
{
    if (currRunningThread < 0){
        exit(0);
    }
    int readyThreadTid = dequeueReadyThread();
//    printf("EXITING: %d\n", currRunningThread);
//    printf("NEXT UP: %d\n",  readyThreadTid);    
    while(threads[readyThreadTid] == NULL || threads[readyThreadTid]->status == KILLED){
        readyThreadTid = dequeueReadyThread();
        if(readyThreadTid == -1){
            break;
        }
    }

//    displayThreads();
    if(readyThreadTid == -1){
       exit(0);
    }
    else{
        threads[currRunningThread]->status = KILLED;
        currRunningThread = readyThreadTid;
        setcontext(&(threads[readyThreadTid]->context));
    }

}

Tid
thread_kill(Tid tid)
{
        if(tid < 0 || tid == currRunningThread || threads[tid] == NULL || tid > THREAD_MAX_THREADS){
            return THREAD_INVALID;
        }
        threads[tid]->status = KILLED;
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
