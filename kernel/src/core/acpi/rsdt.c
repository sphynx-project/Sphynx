// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#include <core/acpi/rsdt.h>
#include <core/acpi/madt.h>

#include <lib/posix/string.h>
#include <lib/posix/assert.h>
#include <sys/boot.h>

xsdtTable *g_xsdt;
rsdtTable *g_rsdt;

void RsdtInitialize()
{
	rsdp_t *rsdp = (rsdp_t *)rsdpResponse->address;
	g_rsdt = (rsdtTable *)(uptr)PHYS_TO_VIRT(rsdp->rsdtAddress);

	if (AcpiUseXsdt()) {
		xsdp_t *xsdp = (xsdp_t *)rsdpResponse->address;
		g_xsdt = (xsdtTable *)(uptr)PHYS_TO_VIRT(xsdp->xsdtAddress);
	}

	madtTable *madt = AcpiFindSdt("APIC");
	assert(madt != NULL);

	MadtInitialize(madt);
}
