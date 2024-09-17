// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#pragma once

#include <core/interrupts/pic.h>
#include <lib/std/types.h>
#include <lib/std/io.h>

#define PIT_COMMAND 0x43
#define PIT_DATA 0x40

void PitInitialize();
void PitSleep(u64 milliseconds);
u64 PitGetUptimeSeconds();
u64 PitGetUptimeMilliseconds();