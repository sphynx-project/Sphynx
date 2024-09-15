// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#pragma once

#include <lib/posix/stdio.h>
#include <lib/std/types.h>
#include <stdbool.h>
#include <sys/cpu.h>

#define assert(x)                                                            \
	do {                                                                     \
		if (!(x)) {                                                          \
			printf("%s:%d: %s: Assertion '%s' failed\n", __FILE__, __LINE__, \
				   __func__, #x);                                            \
			HaltAndCatchFire();                                              \
		}                                                                    \
	} while (0)
