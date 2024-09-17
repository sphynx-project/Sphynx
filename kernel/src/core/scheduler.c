// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#include <core/scheduler.h>
#include <lib/posix/string.h>
#include <lib/posix/stdio.h>
#include <sys/boot.h>
#include <mm/pmm.h>

#define MAX_TASKS 1024

u64 taskId = 0;
Task_t *currentTask;
Task_t *taskList[MAX_TASKS];
u32 taskCount = 0;
u32 currentTaskIndex = 0;

void removeTask()
{
	currentTask->hasExited = true;
}

void taskMain()
{
	Task_t *task = currentTask;
	task->taskFunction();
	removeTask();
	while (1) {
	}
}

void TerminatorTask()
{
	while (1) {
		for (u32 i = 0; i < taskCount; i++) {
			Task_t *task = taskList[i];
			if (task->hasExited) {
				// printf("Task %lu exited with status %d\n", task->id,
				// 	   task->ctx.rax);
				VmmFree(task->pm, (void *)task->ctx.rsp);

				for (u32 j = i; j < taskCount - 1; j++) {
					taskList[j] = taskList[j + 1];
				}
				taskCount--;
				i--;
			}
		}
		asm volatile("pause");
	}
}

void SchedulerInitialize()
{
	taskCount = 0;
	currentTaskIndex = 0;
	SchedulerSpawn(TerminatorTask);
}

void SchedulerSpawn(TaskFunction_t function)
{
	if (taskCount >= MAX_TASKS) {
		return;
	}

	Task_t *task = (Task_t *)PHYS_TO_VIRT(PmmRequestPages(1));
	task->id = taskId++;
	task->pm = VmmNewPageMap();
	task->ctx.rip = (u64)taskMain;
	task->ctx.rsp = (u64)PHYS_TO_VIRT(PmmRequestPages(1)) + 4095;
	task->ctx.cs = 0x08;
	task->ctx.ss = 0x10;
	task->ctx.rflags = 0x202;
	task->taskFunction = function;

	taskList[taskCount++] = task;
}

void SchedulerTick(Context_t *ctx)
{
	if (taskCount == 0) {
		return;
	}

	Task_t *task = taskList[currentTaskIndex];
	if (task == NULL) {
		return;
	}

	if (currentTask != NULL) {
		memcpy(&currentTask->ctx, ctx, sizeof(Context_t));
	}

	currentTask = task;

	memcpy(ctx, &currentTask->ctx, sizeof(Context_t));
	VmmSwitchPageMap(currentTask->pm);

	currentTaskIndex = (currentTaskIndex + 1) % taskCount;
}
