// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#pragma once

unsigned long long syscall(unsigned long long number, unsigned long long arg1,
						   unsigned long long arg2, unsigned long long arg3,
						   unsigned long long arg4, unsigned long long arg5);

void sys_write(unsigned long long stream, const char *data,
			   unsigned long long len);

void sys_exit(unsigned long long exitCode);
void sys_sleep(unsigned long long ms);