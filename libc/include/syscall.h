// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#pragma once

#include <types.h>

u64 syscall(u64 number, u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5);
void sys_write(u64 device, const char *data, u64 len);
void sys_exit(u64 exitCode);
void sys_sleep(u64 ms);
void sys_put_pixel(u64 x, u64 y, u64 r, u64 g, u64 b);