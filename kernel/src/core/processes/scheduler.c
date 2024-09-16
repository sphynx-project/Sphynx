// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#include <core/processes/scheduler.h>
#include <lib/posix/stdio.h>
#include <sphynx.h>

#define INITIAL_PID 1

Process_t processes[MAX_PROCESSES];
u32 processCount = 0;
u32 currentProcessIndex = 0;
u32 nextPid = INITIAL_PID;

static void ContextSwitch(Process_t *prevProcess, Process_t *nextProcess)
{
	__asm__ volatile("mov %%rsp, %0\n" : "=r"(prevProcess->stackPointer));
	__asm__ volatile("mov %0, %%rsp\n" : : "r"(nextProcess->stackPointer));
}

void SchedulerInitialize()
{
	processCount = 0;
	currentProcessIndex = 0;
	nextPid = INITIAL_PID;
}

void SchedulerCreateProcess(void (*processFunction)(void))
{
	if (processCount >= MAX_PROCESSES) {
		KernelLog("Process limit reached");
		return;
	}

	Process_t *process = &processes[processCount];
	process->pid = nextPid++;
	process->processFunction = processFunction;
	process->stackPointer =
		(u64 *)(process->stack + TASK_STACK_SIZE - sizeof(u64));

	processCount++;
}

static void ExecuteTasks()
{
	if (processCount == 0)
		return;

	Process_t *currentProcess = &processes[currentProcessIndex];
	Process_t *nextProcess = &processes[currentProcessIndex];

	ContextSwitch(currentProcess, nextProcess);

	nextProcess->processFunction();

	for (u32 i = currentProcessIndex; i < processCount - 1; i++) {
		processes[i] = processes[i + 1];
	}
	processCount--;

	currentProcessIndex = currentProcessIndex % (processCount + 1);
}

void SchedulerSwitchTask()
{
	if (processCount <= 0)
		return;

	ExecuteTasks();
}

void SchedulerRun()
{
	while (processCount > 0) {
		SchedulerSwitchTask();
	}
}
