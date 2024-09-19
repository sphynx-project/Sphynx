// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#pragma once

#include <lib/std/types.h>
#include <core/interrupts/idt.h>
#include <mm/vmm.h>

typedef void (*TaskFunction_t)(void);

typedef enum {
	STATUS_WAITING = 0,
	STATUS_RUNNING = 1,
	STATUS_TERMINATED = 2
} TaskStatus_t;

typedef struct {
	u64 id;
	PageMap *pm;
	Context_t ctx;
	TaskStatus_t status;
	u64 exitCode;
} __attribute((packed)) Task_t;

void SchedulerInitialize();
void SchedulerTick(Context_t *ctx);
void SchedulerSpawn(TaskFunction_t function);
Task_t *SchedulerGetCurrentTask();
void SchedulerSpawnElf(const char *path);
void TaskExit(Task_t *task, u64 exitCode);
