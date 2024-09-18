// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#pragma once

#include <config.h>
#include <lib/posix/stdio.h>

#define DIV_ROUND_UP(x, y) (x + (y - 1)) / y
#define ALIGN_UP(x, y) DIV_ROUND_UP(x, y) * y
#define ALIGN_DOWN(x, y) (x / y) * y

#define ALIGN_ADDRESS_UP(ADDR, ALIGN) \
	(u64)((((u64)ADDR + (ALIGN - 1)) / ALIGN) * ALIGN)

#define PHYS_TO_VIRT(x) ((void *)((u64)(x) + hhdmOffset))
#define VIRT_TO_PHYS(x) ((u64)(x) - hhdmOffset)

#define VoidRedirect(...) ((void)0)

#define KernelLog(fmt, ...)                                             \
	mprintf("\033[1m%s (%s:%d)\033[0m: ", __func__, __FILE__, __LINE__); \
	mprintf(fmt, ##__VA_ARGS__);                                         \
	mprintf("\n");
