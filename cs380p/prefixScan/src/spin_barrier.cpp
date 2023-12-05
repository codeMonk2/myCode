#include <spin_barrier.h>
#include <chrono>
#include <thread>


int spin_barrier_init(spin_barrier_t *barrier, bool spin, unsigned int n_threads)
{
    int ret = 0;
    barrier->spin = spin;
    barrier->n_threads = n_threads;
    if (spin)
    {
	ret |= pthread_spin_init(&barrier->lock, PTHREAD_PROCESS_SHARED);
        if (!ret)
        {
            barrier->waitingThreads = 0;
            barrier->numWaiters = 0;
        }
    }
    else
    {
        ret = pthread_barrier_init(&barrier->threadBarrier, NULL, n_threads);
    }
    return ret;
}

int spin_barrier_destroy(spin_barrier_t *barrier)
{
    int ret = 0;
    if (barrier->spin)
    {
        if (barrier->waitingThreads.load() == 0) // No one waiting. It can be destroyed
        {
	    pthread_spin_destroy(&barrier->lock);
            barrier->n_threads = 0;
            ret = 0;
        }
        else
        {
            ret = -1;
        }
    }
    else
    {
        ret = pthread_barrier_destroy(&barrier->threadBarrier);
    }
    return ret;
}

int spin_barrier_wait(spin_barrier_t *barrier)
{
    int ret = 0;

    if (barrier->spin)
    {
        int expected = barrier->n_threads;
        volatile int waiters = 0;

	(void)pthread_spin_lock(&barrier->lock);

        barrier->numWaiters.fetch_add(1);
        waiters = barrier->numWaiters.load();

        barrier->waitingThreads.fetch_add(1);

        if (waiters == expected)
        {
            barrier->numWaiters.fetch_sub(1);

            do {
                // sleep so that other threads are scheduled
                std::this_thread::sleep_for(std::chrono::nanoseconds(1));
            } while(barrier->numWaiters.load() != 0);

            barrier->waitingThreads.compare_exchange_strong(expected, 0);

	    (void)pthread_spin_unlock(&barrier->lock);
        }
        else
        {
	    (void)pthread_spin_unlock(&barrier->lock);
            do {
                // sleep so that other threads are scheduled
                std::this_thread::sleep_for(std::chrono::nanoseconds(1));
            } while(barrier->waitingThreads.load() != expected);

            barrier->numWaiters.fetch_sub(1);
        }
    }
    else
    {
        ret = pthread_barrier_wait(&barrier->threadBarrier);
    }

    return ret;
}

//int spin_barrier_wait(spin_barrier_t *barrier, int tid, int loc)
//{
//    int ret = 0;
//    if (barrier->spin)
//    {
//        int expected = barrier->n_threads;
//        volatile int waiters = 0;
//
//        (void)pthread_spin_lock(&barrier->lock);
//
//        barrier->numWaiters.fetch_add(1);
//        waiters = barrier->numWaiters.load();
//
//        barrier->waitingThreads.fetch_add(1);
//
//        if (waiters == expected)
//        {
//            barrier->numWaiters.fetch_sub(1);
//
//            while(barrier->numWaiters.load() != 0);
//            barrier->waitingThreads.compare_exchange_strong(expected, 0);
//            (void)pthread_spin_unlock(&barrier->lock);
//        }
//        else
//        {
//            (void)pthread_spin_unlock(&barrier->lock);
//            while(barrier->waitingThreads.load() != expected);
//
//            barrier->numWaiters.fetch_sub(1);
//        }
//    }
//    else
//    {
//        ret = pthread_barrier_wait(&barrier->threadBarrier);
//    }
//
//    return ret;
//}
