// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#pragma once

#include <lib/std/types.h>
#include <core/interrupts/idt.h>
#include <mm/vmm.h>

typedef void (*TaskFunction_t)(void *);

typedef struct {
	u64 id;
	PageMap *pm;
	Context_t ctx;
	bool hasExited;
	void *data;
	TaskFunction_t taskFunction;
} __attribute((packed)) Task_t;

void SchedulerInitialize();
void SchedulerTick(Context_t *ctx);
void SchedulerSpawn(TaskFunction_t function, void *data);