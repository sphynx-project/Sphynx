// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#include <core/scheduler.h>
#include <lib/posix/string.h>
#include <sys/boot.h>
#include <mm/pmm.h>

#define MAX_TASKS 1024

u64 taskId = 0;
Task_t *currentTask;
Task_t *taskList[MAX_TASKS];
u32 taskCount = 0;

void SchedulerInitialize()
{
	taskCount = 0;
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

	taskList[taskCount++] = task;
}

void SchedulerRun(Context_t *ctx)
{
	if (currentTask != NULL) {
		memcpy(&currentTask->ctx, ctx, sizeof(Context_t));
	}

	if (taskCount > 0) {
		currentTask = taskList[0];
		memmove(&taskList[0], &taskList[1], sizeof(Task_t *) * (taskCount - 1));
		taskCount--;

		printf("Scheduling task with id %d\n", currentTask->id);

		memcpy(ctx, &currentTask->ctx, sizeof(Context_t));
		VmmSwitchPageMap(currentTask->pm);
	}
}

void SchedulerRegister()
{
	IdtIrqRegister(0, SchedulerRun);
}
