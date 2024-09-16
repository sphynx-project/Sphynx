// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#pragma once

#include <lib/std/types.h>
#include <core/acpi/acpi.h>

typedef struct {
	sdtTable header;
	u32 sdt[];
} __attribute__((packed)) rsdtTable;

typedef struct {
	sdtTable header;
	u64 sdt[];
} __attribute__((packed)) xsdtTable;

extern xsdtTable *g_xsdt;
extern rsdtTable *g_rsdt;

void RsdtInitialize();
