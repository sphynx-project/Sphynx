// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#include <core/interrupts/syscall.h>
#include <core/interrupts/timers/pit.h>
#include <lib/posix/stdio.h>
#include <lib/posix/string.h>
#include <core/scheduler.h>
#include <dev/tty.h>

#define MAX_SYSCALLS 256
#define SYSCALL_WRITE 1
#define SYSCALL_EXIT 2

static SyscallHandler_t syscallHandlers[MAX_SYSCALLS] = { 0 };

// TODO: Move all handlers elsewhere
u64 SyscallWriteHandler(u64 stream, u64 data, u64 size, u64 unused1,
						u64 unused2)
{
	char *buffer = (char *)data;

	if (stream == 1) {
		for (u64 i = 0; i < size; ++i) {
			putchar(buffer[i], 1);
		}
	} else if (stream == 2) {
		for (u64 i = 0; i < size; ++i) {
			putchar(buffer[i], 2);
		}
	} else {
		printf("Invalid stream: %llu\n", stream);
		return (u64)-1;
	}

	return size;
}

u64 SyscallExitHandler(u64 exitCode, u64 unused2, u64 unused3, u64 unused4,
					   u64 unused5)
{
	Task_t *currentTask = SchedulerGetCurrentTask();
	TaskExit(currentTask, exitCode);
	return 0;
}

// todo end

void SyscallInitialize()
{
	SyscallRegisterHandler(SYSCALL_WRITE, SyscallWriteHandler);
	SyscallRegisterHandler(SYSCALL_EXIT, SyscallExitHandler);
}

void SyscallRegisterHandler(u64 syscallNumber, SyscallHandler_t handler)
{
	if (syscallNumber >= MAX_SYSCALLS) {
		printf("Invalid syscall number: %llu\n", syscallNumber);
		return;
	}

	syscallHandlers[syscallNumber] = handler;
}

u64 SyscallHandle(u64 syscallNumber, u64 arg1, u64 arg2, u64 arg3, u64 arg4,
				  u64 arg5)
{
	if (syscallNumber >= MAX_SYSCALLS ||
		syscallHandlers[syscallNumber] == NULL) {
		printf("Unknown syscall: %llu, Error", syscallNumber);
		if (syscallHandlers[syscallNumber] == NULL) {
			printf("(No vaid handler specified).\n");
		} else {
			printf("(Unknown).\n");
		}
		return (u64)-1;
	}

	return syscallHandlers[syscallNumber](arg1, arg2, arg3, arg4, arg5);
}
