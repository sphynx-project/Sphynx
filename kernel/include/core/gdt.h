// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#pragma once

#include <lib/std/types.h>

typedef struct {
	u16 limit;
	uptr base;
} __attribute((packed)) gdtPointer_t;

typedef struct {
	u16 limitLow;
	u16 baseLow;
	u8 baseMid;
	u8 accessByte;
	u8 limitHighAndFlags;
	u8 baseHigh;
} __attribute((packed)) gdtEntry_t;

extern gdtPointer_t gdtr;

int GdtInitialize();
void GdtFlush(gdtPointer_t gdtr);
