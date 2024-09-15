// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#pragma once

#include <lib/std/types.h>
#include <stdbool.h>
#include <dev/tty.h>

#ifndef _ASSERT_CALLBACK
#ifdef _HCF_ON_ASSERT
#if _HCF_ON_ASSERT == true
#define _ASSERT_CALLBACK() HaltAndCatchFire()
#else
#define _ASSERT_CALLBACK() \
	do {                   \
	} while (0)
#endif // _HCF_ON_ASSERT == true
#else
#define _ASSERT_CALLBACK() \
	do {                   \
	} while (0)
#endif // _HCF_ON_ASSERT
#endif // _ASSERT_CALLBACK

#define assert(x)                                                            \
	do {                                                                     \
		if (!(x)) {                                                          \
			printf("%s:%d: %s: Assertion '%s' failed\n", __FILE__, __LINE__, \
				   __func__, #x);                                            \
			_ASSERT_CALLBACK();                                              \
		}                                                                    \
	} while (0)
