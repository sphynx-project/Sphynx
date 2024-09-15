// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#pragma once

#include <lib/std/types.h>
#include <core/acpi/acpi.h>
#include <sys/boot.h>

#define APIC_LAPIC 0
#define APIC_IOAPIC 1
#define APIC_ISO 2
#define APIC_IOAPIC_NMI 3
#define APIC_LAPIC_NMI 4
#define APIC_LAPIC_OVERRIDE 5
#define APIC_X2APIC 9

typedef struct {
	sdtTable header;
	u32 lapicAddress;
	u32 flags;
} __attribute__((packed)) madtTable;

typedef struct {
	u8 type;
	u8 length;
} __attribute__((packed)) apicHeader;

typedef struct {
	apicHeader header;
	u8 acpiProcId;
	u8 apicId;
	u32 flags;
} __attribute__((packed)) apicLapic;

typedef struct {
	apicHeader header;
	u8 ioapicId;
	u8 reserved;
	u32 ioapicAddress;
	u32 gsiBase;
} __attribute__((packed)) apicIOApic;

typedef struct {
	apicHeader header;
	u8 bus;
	u8 irq;
	u32 gsi;
	uint16_t flags;
} __attribute__((packed)) apicIso;

extern madtTable *g_madtTable;
extern u32 g_acpiCpuCount;
void MadtInitialize(madtTable *table);
