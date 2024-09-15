// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#pragma once

#include <dev/tty.h>

#define KernelLog(fmt, ...) \
	printf("\033[1m%s (%s:%d)\033[0m: ", __func__, __FILE__, __LINE__); \
	printf(fmt, ##__VA_ARGS__); \
    printf("\n");
