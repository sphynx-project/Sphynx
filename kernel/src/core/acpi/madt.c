// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#include <core/acpi/madt.h>

madtTable *g_madtTable;
u32 g_acpiCpuCount;

void MadtInitialize(madtTable *table)
{
	g_madtTable = table;
	g_acpiCpuCount = 0;
	u8 *ptr = (u8 *)(table + 1);
	u8 *end = (u8 *)table + table->header.length;

	while (ptr < end) {
		apicHeader *header = (apicHeader *)ptr;

		switch (header->type) {
		case APIC_LAPIC: {
			if (g_acpiCpuCount < CONFIG_CPU_MAX) {
				++g_acpiCpuCount;
			}
			break;
		}
		case APIC_IOAPIC:
		case APIC_ISO:
		case APIC_IOAPIC_NMI:
		case APIC_LAPIC_NMI:
		case APIC_LAPIC_OVERRIDE:
		case APIC_X2APIC: {
			break;
		}
		default: {
			KernelLog("Found invalid MADT entry %i", header->type);
			break;
		}
		}
		ptr += header->length;
	}
}
