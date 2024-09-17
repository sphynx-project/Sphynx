// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#pragma once

#include <stdatomic.h>

typedef struct {
	_Atomic int locked;
} Spinlock;

void LockInit(Spinlock *lock);
void LockAcquire(Spinlock *lock);
void LockRelease(Spinlock *lock);
