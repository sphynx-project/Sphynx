// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#pragma once

#include <lib/std/types.h>
#include <core/interrupts/idt.h>
#include <mm/vmm.h>

typedef struct {
	u64 id;
	PageMap *pm;
	Context_t ctx;
} __attribute((packed)) Task_t;

typedef void (*TaskFunction_t)(void);

void SchedulerInitialize();
void SchedulerSpawn(TaskFunction_t function);
void SchedulerRegister();