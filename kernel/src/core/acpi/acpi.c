// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#include <core/acpi/acpi.h>
#include <core/acpi/rsdt.h>
#include <lib/posix/string.h>
#include <sys/boot.h>

bool _xsdtIsAvailable;

void AcpiInitialize()
{
	rsdp_t *rsdp = (rsdp_t *)rsdpResponse->address;
	if (rsdp->revision == 0) {
		_xsdtIsAvailable = false;
		RsdtInitialize();
	} else if (rsdp->revision >= 2) {
		_xsdtIsAvailable = true;
		RsdtInitialize();
	}
}

void *AcpiFindSdt(char *signature)
{
	int entry_divisor = (AcpiUseXsdt() ? 8 : 4);
	int header_length = AcpiUseXsdt() ? g_xsdt->header.length :
										g_rsdt->header.length;
	int entries = (header_length - sizeof(sdtTable)) / entry_divisor;
	for (int i = 0; i < entries; i++) {
		sdtTable *header = NULL;
		if (AcpiUseXsdt()) {
			header = (sdtTable *)(uptr)PHYS_TO_VIRT(g_xsdt->sdt[i]);
		} else {
			header = (sdtTable *)(uptr)PHYS_TO_VIRT(g_rsdt->sdt[i]);
		}
		if (!strncmp(header->signature, signature, 4)) {
			return (void *)header;
		}
	}

	return NULL;
}

bool AcpiUseXsdt()
{
	return _xsdtIsAvailable;
}
