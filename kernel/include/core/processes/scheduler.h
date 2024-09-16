// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#pragma once

#include <lib/std/types.h>

#define MAX_PROCESSES 256
#define TASK_STACK_SIZE 8192

typedef struct {
	void (*processFunction)(void);
	u64 stack[TASK_STACK_SIZE / sizeof(u64)];
	u64 *stackPointer;
	u32 pid;
} Process_t;

extern Process_t processes[MAX_PROCESSES];
extern u32 processCount;
extern u32 currentProcessIndex;
extern u32 nextPid;

void SchedulerInitialize();
void SchedulerCreateProcess(void (*processFunction)(void));
void SchedulerRun();
void SchedulerSwitchTask();
