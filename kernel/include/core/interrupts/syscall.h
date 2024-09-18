// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#pragma once

#include <core/interrupts/idt.h>

#define SYSCALL_VECTOR 0x80

typedef u64 (*SyscallHandler_t)(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5);

void SyscallInitialize();
void SyscallRegisterHandler(u64 syscallNumber, SyscallHandler_t handler);
u64 SyscallHandle(u64 syscallNumber, u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5);
