#include <stdlib.h>
#include <stdio.h>
#include <ucontext.h>
#include <assert.h>
#include "green.h"

#define FALSE 0
#define TRUE 1

#define STACK_SIZE 4096

void green_thread();

static ucontext_t main_cntx = {0};
static green_t main_green = {&main_cntx, NULL, NULL, NULL, NULL, FALSE};

static green_t *running = &main_green;

static green_t *ready_queue;

static void init() __attribute__((constructor));

void init() {
    getcontext(&main_cntx);
    printf("main context: %p\n", &main_cntx);
}

/* Returns the next in the ready queue. Returns NULL if there the queue is empty */
green_t *dequeue_ready() {
    green_t *next = NULL;
    if (ready_queue != NULL) {
        next = ready_queue;
        ready_queue = ready_queue->next;
    }
    next->next = NULL;
    return next;
}

/* adds a thread to the ready queue. */
void queue(green_t *new) {
    // add new to the ready queue
    green_t *curr = ready_queue;
    if (ready_queue != NULL) {
        while(curr->next != NULL) {
            // printf("Snurrr.\n");
            curr = curr->next;
        }
        curr->next = new;
    } else {
        ready_queue = new;
    }
}

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
    queue(new);

    return 0;
}

/* Starts the execution of a function (of a thread) and then terminate the thread */
void green_thread() {
    green_t *this = running;
    (*this->fun)(this->arg);

    // Place waiting (joining) thread in ready queue
    if (this->join != NULL) {
        queue(this->join);
    }

    // Free allocated memory structures
    free(this->context->uc_stack.ss_sp);
    free(this->context);

    // we're a zombie
    this->zombie = TRUE; // WHAT TO DO

    // find the next thread to run
    green_t *next = dequeue_ready();
    if (next == NULL) {
        next = &main_green;
    }
    running = next;
    setcontext(next->context);
}

int green_yield() {
    green_t *susp = running;
    queue(susp);
    
    green_t *next = dequeue_ready();
    if (next == NULL) {
        next = &main_green;
    }
    running = next;
    swapcontext(susp->context, next->context);
    // IMPORTANT POINT IN CODE, EXECUTION WILL RESUME HERE FOR SUSP!!!!
    
    return 0;
}

int green_join(green_t *thread) {
    if (thread->zombie)
        return 0;

    // add susp to the ready queue
    green_t *susp = running;
    thread->join = susp;

    // select the next thread for execution
    green_t *next = dequeue_ready();
    if (next == NULL)
        next = &main_green;
    running = next;
    swapcontext(susp->context, next->context);

    return 0;
}