// priority queue for threads
#ifndef THREAD_PQUEUE_
#define THREAD_PQUEUE_

#include "threads/thread.h"

struct thread_pqueue;

int compare_sleeptime(const struct thread* thread1, const struct thread* thread2);

struct thread_pqueue* thread_pqueue_init(int(*compare)(const struct thread* thread1, const struct thread* thread2));
void thread_pqueue_free(struct thread_pqueue* tq);
int thread_pqueue_empty(struct thread_pqueue* tq);
void thread_pqueue_insert(struct thread_pqueue* tq, struct thread* th);
struct thread* thread_pqueue_top(struct thread_pqueue* tq);
struct thread* thread_pqueue_peek(struct thread_pqueue* tq);

#endif // THREAD_PQUEUE_
