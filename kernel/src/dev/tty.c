// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#include <dev/tty.h>

#define NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_PRECISION_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_FLOAT_FORMAT_SPECIFIERS 0
#define NANOPRINTF_USE_LARGE_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_BINARY_FORMAT_SPECIFIERS 0
#define NANOPRINTF_USE_WRITEBACK_FORMAT_SPECIFIERS 0
#define NANOPRINTF_SNPRINTF_SAFE_TRIM_STRING_ON_OVERFLOW 1
typedef long ssize_t;

#define NANOPRINTF_IMPLEMENTATION
#include <nanoprintf.h>

#include <sys/boot.h>

void _putc(char ch)
{
	flanterm_write(ftCtx, &ch, sizeof(ch));
}

int printf(const char *fmt, ...)
{
	char buffer[1024];
	va_list args;

	va_start(args, fmt);
	int length = npf_vsnprintf(buffer, sizeof(buffer), fmt, args);
	va_end(args);

	if (length < 0 || length >= (int)sizeof(buffer)) {
		return -1;
	}

	for (int i = 0; i < length; ++i) {
		_putc(buffer[i]);
	}

	return length;
}

void vprintf(const char *fmt, va_list args)
{
	char buffer[1024];
	int length = npf_vsnprintf(buffer, sizeof(buffer), fmt, args);

	if (length < 0 || length >= (int)sizeof(buffer)) {
		return;
	}

	for (int i = 0; i < length; ++i) {
		_putc(buffer[i]);
	}
}