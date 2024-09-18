// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#pragma once

#include <stdarg.h>
#include <types.h>

void write(u64 device, u8 *data, u64 size);

void vprintf(const char *fmt, va_list args);
int printf(const char *fmt, ...);