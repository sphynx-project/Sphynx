// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#include <lib/std/lock.h>

void LockInit(Spinlock *lock)
{
	atomic_store(&lock->locked, 0);
}

void LockAcquire(Spinlock *lock)
{
	while (atomic_exchange(&lock->locked, 1)) {
		while (atomic_load(&lock->locked)) {
		}
	}
}

void LockRelease(Spinlock *lock)
{
	atomic_store(&lock->locked, 0);
}
