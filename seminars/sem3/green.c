#include <stdlib.h>
#include <stdio.h>
#include <ucontext.h>
#include <assert.h>
#include <signal.h>
#include <sys/time.h>
#include "green.h"

#define FALSE 0
#define TRUE 1

#define PERIOD 1
#define STACK_SIZE 4096

void green_thread(void);
void timer_handler(int);
green_t *dequeue_ready(void);
void queue_ready(green_t*);
int green_mutex_lock(green_mutex_t*);
int green_mutex_unlock(green_mutex_t*);
int green_mutex_init(green_mutex_t*);

static sigset_t block;
static green_mutex_t timer_lock;
static ucontext_t main_cntx = {0};
static green_t main_green = {&main_cntx, NULL, NULL, NULL, NULL, FALSE};

static green_t *running = &main_green;

static green_t *ready_first = NULL;
static green_t *ready_last = NULL;

static void init() __attribute__((constructor));

void init() {
    getcontext(&main_cntx);
    green_mutex_init(&timer_lock);

    // Timer stuff
    sigemptyset(&block);
    sigaddset(&block, SIGVTALRM);

    struct sigaction act = {0};
    struct timeval interval;
    struct itimerval period;

    act.sa_handler = timer_handler;
    assert(sigaction(SIGVTALRM, &act, NULL) == 0);

    interval.tv_sec = 0;
    interval.tv_usec = PERIOD;
    period.it_interval = interval;
    period.it_value = interval;
    setitimer(ITIMER_VIRTUAL, &period, NULL);
}

/* Handles the interrupt each period */
void timer_handler(int sig) {
    sigprocmask(SIG_BLOCK, &block, NULL);
    green_t *susp = running;

    // RING RING
    printf("!");
    // put in ready queue 
    queue_ready(susp);

    // find next to execute
    green_t *next = dequeue_ready();
    running = next;
    sigprocmask(SIG_UNBLOCK, &block, NULL);
    swapcontext(susp->context, next->context);
}

/* Returns the next in the ready queue. Returns the main_green thread if there the queue is empty */
green_t *dequeue_ready() {
    green_t *next = &main_green;
    if (ready_first != NULL) {
        next = ready_first;
        // If they both point at the same node, last should be set to NULL as well
        if (ready_first == ready_last) {
            ready_last = NULL;
        }
        ready_first = ready_first->next;
        next->next = NULL;
    } else { // DEBUG TODO: Remove me
        printf("returning the main thread\n");
    }
    return next;
}

/* adds a thread to the ready queue. */
void queue_ready(green_t *new) {
    // add new to the ready queue
    // new->next = NULL;
    if (ready_last != NULL) {
        ready_last->next = new;
        ready_last = new;
    } else {
        ready_first = new;
        ready_last = new;
    }
}

/* creates a thread with a function and parameters */
int green_create(green_t *new, void *(*fun)(void*), void *arg) {
    ucontext_t *cntx = (ucontext_t *) malloc(sizeof(ucontext_t));
    getcontext(cntx);

    void *stack = malloc(STACK_SIZE);
    cntx->uc_stack.ss_sp = stack;
    cntx->uc_stack.ss_size = STACK_SIZE;

    makecontext(cntx, green_thread, 0);
    new->context = cntx;
    new->fun = fun;
    new->arg = arg;
    new->next = NULL;
    new->join = NULL;
    new->zombie = FALSE;

    // add new to the ready queue
    queue_ready(new);

    return 0;
}

/* Starts the execution of a function (of a thread) and then terminate the thread */
void green_thread() {
    green_t *this = running;
    (*this->fun)(this->arg);

    // Place waiting (joining) thread in ready queue
    if (this->join != NULL) {
        // TODO: Changed here....
        green_t *joiner = this->join;
        this->join = this->join->next;
        queue_ready(joiner);
    }

    // Free allocated memory structures
    free(this->context->uc_stack.ss_sp);
    free(this->context);

    // we're a zombie
    this->zombie = TRUE;

    // find the next thread to run
    green_t *next = dequeue_ready();
    running = next;
    setcontext(next->context);
}

/* yields a thread from execution */
int green_yield() {
    green_t *susp = running;
    queue_ready(susp);
    
    green_t *next = dequeue_ready();
    running = next;
    swapcontext(susp->context, next->context);
    // IMPORTANT POINT IN CODE, EXECUTION WILL RESUME HERE FOR SUSP!!!!
    
    return 0;
}

/* waits a the specified thread to terminate before executing */
int green_join(green_t *thread) {
    if (thread->zombie)
        return 0;

    // add susp to the ready queue
    green_t *susp = running;
    thread->join = susp;

    // select the next thread for execution
    green_t *next = dequeue_ready();
    running = next;
    swapcontext(susp->context, next->context);

    return 0;
}

/************************************************
 *                CONDITIONALS                  *
 * **********************************************/

void green_cond_init(green_cond_t *cond) {
    cond->num_susp = 0;
    cond->waiting = NULL;
}

/* Suspends the process on the condition variable */
void green_cond_wait(green_cond_t *cond) {
    // Another one is waiting
    
    // The currently running one will be suspended
    green_t *susp = running;
    
    // If no one's waiting
    if (cond->num_susp == 0) {
        cond->waiting = susp;
    } else {
        green_t *curr = cond->waiting;
        while (curr != NULL) {
            curr = curr->next;
        } // Add the suspended one to the list
        curr = susp;
    }

    // Find next to run and run it.
    green_t *next = dequeue_ready();
    // printf("Sleeping #%d\n", *((int *)susp->arg));
    running = next;
    cond->num_susp++;
    swapcontext(susp->context, next->context);
}

/* Signals the next waiting on the variable */
void green_cond_signal(green_cond_t *cond) {
    if (cond->num_susp == 0) {
        assert(cond->waiting == NULL);
        return;
    }
    assert(cond->waiting != NULL && cond->num_susp > 0);
    green_t *old = cond->waiting;
    cond->waiting = old->next;

    old->next = NULL;
    queue_ready(old);
    cond->num_susp--;
}

/************************************************
 *                  LOCKS                       *
 * **********************************************/

/* Initializes the mutex */
int green_mutex_init(green_mutex_t *mutex) {
    mutex->taken = FALSE;
    mutex->susp = NULL;
}

/* Grabs the lock */
int green_mutex_lock(green_mutex_t *mutex) {
    // Block timer interrupt
    sigprocmask(SIG_BLOCK, &block, NULL);

    green_t *susp = running;
    while(mutex->taken == TRUE) {
        printf("????????????\n");
        // Suspend the running thread
        green_t *curr = mutex->susp;
        while (curr != NULL) {
            curr = curr->next;
        }
        curr = susp;

        // printf("=== Mutex wait list ===\n");
        // green_t *debug = mutex->susp;
        // while (debug != NULL) {
        //     printf("I'm: %p -> %p\n", debug, debug->next);
        // }
        // printf("=== Mutex wait END ===\n");

        // find the next thread
        green_t *next = dequeue_ready();
        running = next;
        swapcontext(susp->context, next->context);
    }
    // Take the lock
    mutex->taken = TRUE;
    
    // Unblock
    sigprocmask(SIG_UNBLOCK, &block, NULL);

    return 0;
}

/* Releases the lcok */
int green_mutex_unlock(green_mutex_t *mutex) {
    // Block timer interrupt
    sigprocmask(SIG_BLOCK, &block, NULL);

    // Move suspended threads to ready queue
    queue_ready(mutex->susp);
    mutex->susp = NULL;

    // release lock
    mutex->taken = FALSE;

    // Unblock
    sigprocmask(SIG_UNBLOCK, &block, NULL);

    return 0;
}