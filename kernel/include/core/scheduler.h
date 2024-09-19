// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#pragma once

#include <lib/std/types.h>
#include <core/interrupts/idt.h>
#include <mm/vmm.h>

typedef void (*TaskFunction_t)(void);

typedef struct {
	u64 id;
	PageMap *pm;
	Context_t ctx;
	bool hasExited;
	TaskFunction_t taskFunction;
	u64 exitCode;
} __attribute((packed)) Task_t;

void SchedulerInitialize();
void SchedulerTick(Context_t *ctx);
void SchedulerSpawn(TaskFunction_t function);
Task_t *SchedulerGetCurrentTask();
void SchedulerSpawnElf(const char *path);
void TaskExit(Task_t *task, u64 exitCode);