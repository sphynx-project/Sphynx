// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#include <core/interrupts/syscall.h>
#include <core/interrupts/timers/pit.h>
#include <lib/posix/stdio.h>
#include <lib/posix/string.h>
#include <core/scheduler.h>
#include <core/bus.h>
#include <sys/boot.h>
#include <dev/tty.h>
#include <sys/cpu.h>

#define MAX_SYSCALLS 256
#define SYSCALL_WRITE 1
#define SYSCALL_EXIT 2
#define SYSCALL_PUT_PIXEL 3

static SyscallHandler_t syscallHandlers[MAX_SYSCALLS] = { 0 };

// TODO: Move all handlers elsewhere
u64 SyscallWriteHandler(u64 device, u64 data, u64 size, u64 unused1,
						u64 unused2)
{
	DeviceHandle_t *deviceHandle = DeviceGet(device);
	if (deviceHandle == NULL) {
		KernelLog("Failed to open handle to device %d", device);
		return 0;
	}

	while (deviceHandle->poll() != DEVICE_NOT_READY) {
	}

	deviceHandle->write((void *)data, size);

	return size;
}

u64 SyscallExitHandler(u64 exitCode, u64 unused2, u64 unused3, u64 unused4,
					   u64 unused5)
{
	Task_t *currentTask = SchedulerGetCurrentTask();
	TaskExit(currentTask, exitCode);
	return 0;
}

u64 SyscallPutPixelHandler(u64 x, u64 y, u64 color, u64 unused4, u64 unused5)
{
	u32 *pixel_addr = (u32 *)(framebuffer->address + y * framebuffer->pitch +
							  x * (framebuffer->bpp / 8));
	*pixel_addr = (u32)color;

	return 0;
}
// TODO end

void SyscallInitialize()
{
	SyscallRegisterHandler(SYSCALL_WRITE, SyscallWriteHandler);
	SyscallRegisterHandler(SYSCALL_EXIT, SyscallExitHandler);
	SyscallRegisterHandler(SYSCALL_PUT_PIXEL, SyscallPutPixelHandler);
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
			printf("(No valid handler specified).\n");
		} else {
			printf("(Unknown).\n");
		}
		return (u64)-1;
	}

	return syscallHandlers[syscallNumber](arg1, arg2, arg3, arg4, arg5);
}
