// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

unsigned long long syscall(unsigned long long number, unsigned long long arg1,
						   unsigned long long arg2, unsigned long long arg3,
						   unsigned long long arg4, unsigned long long arg5)
{
	unsigned long long result;

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

void sys_write(unsigned long long stream, const char *data,
			   unsigned long long len)
{
	syscall(1, stream, (unsigned long long)data, len, 0, 0);
}

void sys_exit(unsigned long long exitCode)
{
	syscall(2, exitCode, 0, 0, 0, 0);
}