// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#pragma once

#include <stdarg.h>
#include <lib/std/lock.h>

extern Spinlock vprintf_lock;

void vprintf(const char *fmt, va_list args);
void vdprintf(const char *fmt, va_list args);
