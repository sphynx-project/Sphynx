// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#include <syscall.h>

u64 syscall(u64 number, u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
	u64 result;

	__asm__ volatile("movq %1, %%rax;"
					 "movq %2, %%rdi;"
					 "movq %3, %%rsi;"
					 "movq %4, %%rdx;"
					 "movq %5, %%r10;"
					 "movq %6, %%r8;"
					 "int $0x80;"
					 "movq %%rax, %0;"
					 : "=r"(result)
					 : "r"(number), "r"(arg1), "r"(arg2), "r"(arg3), "r"(arg4),
					   "r"(arg5)
					 : "rax", "rdi", "rsi", "rdx", "r10", "r8");

	return result;
}

void sys_write(u64 device, const char *data, u64 len)
{
	syscall(1, device, (u64)data, len, 0, 0);
}

void sys_exit(u64 exitCode)
{
	syscall(2, exitCode, 0, 0, 0, 0);
}

void sys_put_pixel(u64 x, u64 y, u64 r, u64 g, u64 b)
{
	u64 color = (r << 16) | (g << 8) | b;
	syscall(3, x, y, color, 0, 0);
}
