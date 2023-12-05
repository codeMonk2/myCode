#ifndef _SPIN_BARRIER_H
#define _SPIN_BARRIER_H

#include <pthread.h>
#include <iostream>
#include <atomic>

typedef struct spin_barrier_s {
    bool spin;
    unsigned int n_threads;
    pthread_spinlock_t lock;
    std::atomic_int waitingThreads;
    std::atomic_int numWaiters;

    pthread_barrier_t threadBarrier;
} spin_barrier_t;

int spin_barrier_init(spin_barrier_t *, bool, unsigned int);
int spin_barrier_destroy(spin_barrier_t *);
int spin_barrier_wait(spin_barrier_t *);

#endif
