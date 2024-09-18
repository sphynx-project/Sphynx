// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#include <string.h>
#include <syscall.h>

void puts(const char *str)
{
	sys_write(1, str, strlen(str));
}

void _start(void)
{
	puts("SphynxOS booted yippie!\n");
	puts("now we die >:D\n");
	sys_exit(69);
}
