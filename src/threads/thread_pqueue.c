#include "threads/thread_pqueue.h"


#include <stdio.h>

typedef int(*compare)(const struct thread* thread1, const struct thread* thread2);

int compare_sleeptime(const struct thread* thread1, const struct thread* thread2){
    return thread1->sleep_due - thread2->sleep_due;
}

struct thread_pqueue {
    int capacity;
    int n;
    struct thread** array;
    compare cmp;
};

static const int initial_size = 16;

static void swap(struct thread_pqueue* tq, int index1, int index2) {
    // shallow copy of pointers only
    struct thread* tmp = tq->array[index1];
    tq->array[index1] = tq->array[index2];
    tq->array[index2] = tmp;
}

static void rise(struct thread_pqueue* tq, int k) {
    while (k > 1 && tq->cmp(tq->array[k / 2], tq->array[k]) < 0) {
        swap(tq, k, k / 2);
        k = k / 2;
    }
}

static void fall(struct thread_pqueue* tq, int k) {
    while (2 * k <= tq->n) {
        int j = 2 * k;
        if (j < tq->n && tq->cmp(tq->array[j], tq->array[j + 1]) < 0) {
            j++;
        }

        if (tq->cmp(tq->array[k], tq->array[j]) < 0) {
            swap(tq, k, j);
        }
        k = j;
    }
}

static struct thread** array_resize(struct thread** array, int newlength) {
    // reallocate array to new size
    return (struct thread**)realloc(array, newlength * sizeof(struct thread*));
}

void thread_pqueue_init(struct thread_pqueue* tq, int(*compare)(const struct thread* thread1, const struct thread* thread2)) {
    tq->array = NULL;
    tq->capacity = initial_size;
    tq->n = 0;
    tq->cmp = compare;
}

void thread_pqueue_free(struct thread_pqueue* tq) {
    free(tq);
}

int thread_pqueue_empty(struct thread_pqueue* tq) {
    return tq->n == 0;
}

void thread_pqueue_insert(struct thread_pqueue* tq, struct thread* th) {
    if (tq->array == NULL){
        tq->array = (struct thread**)malloc(sizeof(struct thread*) * (initial_size + 1));
        tq->capacity = initial_size;
        tq->n = 0;
    }

    if (tq->n == tq->capacity) {
        tq->capacity *= 2;
        // we need to resize the array
        tq->array = array_resize(tq->array, tq->capacity);
    }

    // we always insert at end of array
    tq->array[++tq->n] = th;
    rise(tq, tq->n);
}

struct thread* thread_pqueue_top(struct thread_pqueue* tq) {

    // reduce array memory use if appropriate
    if (tq->capacity > initial_size && tq->n < tq->capacity / 4) {
        tq->array = array_resize(tq->array, tq->capacity / 2);
        tq->capacity /= 2;
    }

    struct thread* el = tq->array[1];
    swap(tq, 1, tq->n--);
    tq->array[tq->n + 1] = NULL;  // looks tidier when stepping through code - not really necessary
    fall(tq, 1);
    return el;
}

struct thread* thread_pqueue_peek(struct thread_pqueue* tq) {
    return tq->array[1];
}
