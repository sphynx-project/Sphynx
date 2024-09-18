// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#include <dev/tty.h>
#include <lib/std/io.h>

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

Spinlock vprintf_lock;
Spinlock vdprintf_lock;

#include <core/bus.h>

void _putc(char ch)
{
	flanterm_write(ftCtx, &ch, sizeof(ch));
}

void _dputc(char ch)
{
	SerialIOWriteByte(0xE9, ch);
}

u8 TTYPoll()
{
	return DEVICE_READY;
}

u64 TTYRead(void *unused)
{
	(void)unused;
	return 0;
}

void TTYWrite(void *in, usize len)
{
	char *data = (char *)in;
	for (usize i = 0; i < len; i++)
		_putc(data[i]);
}

void TTYInitialize()
{
	DeviceRegister(0, TTYPoll, TTYRead, TTYWrite);
}

void vprintf(const char *fmt, va_list args)
{
	char buffer[1024];
	int length = npf_vsnprintf(buffer, sizeof(buffer), fmt, args);

	if (length >= 0 && length < (int)sizeof(buffer)) {
		LockAcquire(&vprintf_lock);
		DeviceHandle_t *ttyHandle = DeviceGet(0);
		if (ttyHandle == NULL) {
			KernelLog("Failed to open handle to the TTY!\n");
			return;
		}

		while (ttyHandle->poll() != DEVICE_NOT_READY) {
		}

		ttyHandle->write(buffer, length);
		LockRelease(&vprintf_lock);
	}
}