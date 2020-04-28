#ifdef __APPLE__

#ifndef PTHREAD_SPINLOCK_H_
#define PTHREAD_SPINLOCK_H_

#include <pthread.h>
#include "node.h"

typedef pthread_mutex_t pthread_spinlock_t;

int pthread_spin_init(pthread_spinlock_t *spin, int pshared)
{
    UNUSED(pshared);
    return pthread_mutex_init(spin, NULL);
}

int pthread_spin_destroy(pthread_spinlock_t *spin)
{
    return pthread_mutex_destroy(spin);
}

int pthread_spin_lock(pthread_spinlock_t *spin)
{
    return pthread_mutex_lock(spin);
}

int pthread_spin_unlock(pthread_spinlock_t *spin)
{
    return pthread_mutex_unlock(spin);
}

#endif /* PTHREAD_SPINLOCK_H_ */

#endif /* __APPLE__ */
