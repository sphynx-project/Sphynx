// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#pragma once

void printf(const char *fmt, ...);
void dprintf(const char *fmt, ...);

#define mprintf(fmt, ...)       \
	printf(fmt, ##__VA_ARGS__); \
	dprintf(fmt, ##__VA_ARGS__);
