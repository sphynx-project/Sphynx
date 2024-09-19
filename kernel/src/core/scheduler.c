// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#include <core/scheduler.h>
#include <lib/posix/string.h>
#include <lib/posix/stdio.h>
#include <core/proc/elf.h>
#include <sys/boot.h>
#include <dev/vfs.h>
#include <mm/pmm.h>

#define MAX_TASKS 4096

u64 taskId = 0;
Task_t *currentTask;
Task_t *taskList[MAX_TASKS];
u32 taskCount = 0;
u32 currentTaskIndex = 0;

void RemoveTask(u32 index)
{
	if (index >= taskCount) {
		return;
	}

	Task_t *task = taskList[index];
	VmmFree(task->pm, (void *)task->ctx.rsp);
	VmmFree(task->pm, (void *)task);

	for (u32 i = index; i < taskCount - 1; ++i) {
		taskList[i] = taskList[i + 1];
	}
	taskCount--;
}

void TaskExit(Task_t *task, u64 exitCode)
{
	task->status = STATUS_TERMINATED;
	task->exitCode = exitCode;
	SchedulerTick(NULL);
}

Task_t *SchedulerGetCurrentTask()
{
	return currentTask;
}

void SchedulerInitialize()
{
	taskCount = 0;
	currentTaskIndex = 0;
}

void SchedulerSpawn(TaskFunction_t function)
{
	if (taskCount >= MAX_TASKS) {
		return;
	}

	Task_t *task = (Task_t *)PHYS_TO_VIRT(PmmRequestPages(1));
	task->id = taskId++;
	task->pm = VmmNewPageMap();
	task->ctx.rip = (u64)function;
	task->ctx.rsp = (u64)PHYS_TO_VIRT(PmmRequestPages(1)) + 4095;
	task->ctx.cs = 0x08;
	task->ctx.ss = 0x10;
	task->ctx.rflags = 0x202;
	task->status = STATUS_WAITING;

	taskList[taskCount++] = task;
}

void SchedulerTick(Context_t *ctx)
{
	if (taskCount == 0) {
		return;
	}

	for (u32 i = 0; i < taskCount; ++i) {
		Task_t *task = taskList[i];
		if (task->status == STATUS_TERMINATED) {
			printf("Task %llu terminated with code %llu\n", task->id,
				   task->exitCode);
			RemoveTask(i);
			i--;
		}
	}

	if (taskCount == 0) {
		return;
	}

	if (ctx == NULL) {
		return;
	}

	Task_t *task = taskList[currentTaskIndex];
	if (task == NULL) {
		return;
	}

	if (currentTask != NULL) {
		memcpy(&currentTask->ctx, ctx, sizeof(Context_t));
		currentTask->status = STATUS_WAITING;
	}

	currentTask = task;
	currentTask->status = STATUS_RUNNING;

	memcpy(ctx, &currentTask->ctx, sizeof(Context_t));
	VmmSwitchPageMap(currentTask->pm);

	currentTaskIndex = (currentTaskIndex + 1) % taskCount;
}

void SchedulerSpawnElf(const char *path)
{
	if (taskCount >= MAX_TASKS) {
		mprintf("ERROR: Maximum task limit reached!\n");
		return;
	}

	u8 *elfData =
		(u8 *)VmmAlloc(VmmGetKernelPageMap(), 1, VMM_PRESENT | VMM_WRITE);
	if (elfData == NULL) {
		mprintf("ERROR: Failed to allocate memory for ELF data!\n");
		return;
	}

	u64 bytesRead = VfsRead(path, elfData);
	if (bytesRead <= 0) {
		mprintf("ERROR: Failed to read ELF at %s\n", path);
		VmmFree(VmmGetKernelPageMap(), elfData);
		return;
	}

	Task_t *task = (Task_t *)PHYS_TO_VIRT(PmmRequestPages(1));
	if (task == NULL) {
		mprintf("ERROR: Failed to allocate memory for task!\n");
		VmmFree(VmmGetKernelPageMap(), elfData);
		return;
	}

	task->id = taskId++;
	task->pm = VmmNewPageMap();
	if (task->pm == NULL) {
		mprintf("ERROR: Failed to create a new page map for task!\n");
		VmmFree(VmmGetKernelPageMap(), elfData);
		VmmFree(VmmGetKernelPageMap(), (void *)task);
		return;
	}

	task->ctx.rip = (u64)SpawnElf(elfData, task->pm);
	if (task->ctx.rip == 0) {
		mprintf("ERROR: Failed to load ELF!\n");
		VmmFree(VmmGetKernelPageMap(), elfData);
		VmmFree(task->pm, NULL);
		VmmFree(VmmGetKernelPageMap(), (void *)task);
		return;
	}

	task->ctx.rsp = (u64)PHYS_TO_VIRT(PmmRequestPages(1)) + 4095;
	task->ctx.cs = 0x08;
	task->ctx.ss = 0x10;
	task->ctx.rflags = 0x202;
	task->status = STATUS_WAITING;

	taskList[taskCount++] = task;
	VmmFree(VmmGetKernelPageMap(), elfData);
}
