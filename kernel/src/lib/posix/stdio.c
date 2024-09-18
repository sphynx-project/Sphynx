// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#include <lib/posix/stdio.h>
#include <dev/tty.h>
#include <stdarg.h>

void printf(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
}

void dprintf(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vdprintf(fmt, args);
	va_end(args);
}
