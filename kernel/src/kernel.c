// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

// Boot and CPU includes
#include <sphynx.h>
#include <sys/boot.h>
#include <sys/cpu.h>
#include <lib/std/io.h>

// Interrupt includes
#include <core/interrupts/idt.h>
#include <core/gdt.h>

// ACPI includes
#include <core/acpi/acpi.h>
#include <core/acpi/rsdt.h>
#include <core/acpi/madt.h>

// Memory includes
#include <mm/pmm.h>
#include <mm/vmm.h>

// TTY and GUI includes
#include <flanterm/backends/fb.h>
#include <flanterm/flanterm.h>
#include <lib/posix/stdio.h>
#include <dev/tty.h>

// Misc includes
#include <lib/posix/assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <seif.h>

LIMINE_START

LIMINE_REQUEST_SECTION static volatile LIMINE_BASE_REVISION(2);

LIMINE_REQUEST limine_framebuffer_request
	framebufferRequest = { .id = LIMINE_FRAMEBUFFER_REQUEST, .revision = 0 };
LIMINE_REQUEST limine_memmap_request
	memmapRequest = { .id = LIMINE_MEMMAP_REQUEST, .revision = 0 };
LIMINE_REQUEST limine_hhdm_request hhdmRequest = { .id = LIMINE_HHDM_REQUEST,
												   .revision = 0 };
LIMINE_REQUEST limine_rsdp_request rsdpRequest = { .id = LIMINE_RSDP_REQUEST,
												   .revision = 0 };
LIMINE_REQUEST limine_module_request
	moduleRequest = { .id = LIMINE_MODULE_REQUEST, .revision = 0 };
LIMINE_REQUEST limine_kernel_address_request kernelAddressRequest = {
	.id = LIMINE_KERNEL_ADDRESS_REQUEST,
	.revision = 0
};

LIMINE_END

struct flanterm_context *ftCtx;

struct limine_framebuffer *framebuffer;
struct limine_memmap_response *memoryMap;
struct limine_rsdp_response *rsdpResponse;
struct limine_module_response *moduleResponse;
struct limine_kernel_address_response *kernelAddressResponse;
u64 hhdmOffset;

void KernelEntry(void)
{
	if (LIMINE_BASE_REVISION_SUPPORTED == false) {
		HaltAndCatchFire();
	}

	if (framebufferRequest.response == NULL ||
		framebufferRequest.response->framebuffer_count < 1) {
		HaltAndCatchFire();
	}

	framebuffer = framebufferRequest.response->framebuffers[0];

	ftCtx = flanterm_fb_init(
		NULL, NULL, framebuffer->address, framebuffer->width,
		framebuffer->height, framebuffer->pitch, framebuffer->red_mask_size,
		framebuffer->red_mask_shift, framebuffer->green_mask_size,
		framebuffer->green_mask_shift, framebuffer->blue_mask_size,
		framebuffer->blue_mask_shift, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		NULL, 0, 0, 1, 0, 0, 8);
	ftCtx->cursor_enabled = false;
	ftCtx->full_refresh(ftCtx);

	printf("Booting Sphynx...\n");

	GdtInitialize();
	IdtInitialize();

	if (memmapRequest.response == NULL) {
		KernelLog("ERROR: Failed to get memory map!");
		HaltAndCatchFire();
	}

	memoryMap = memmapRequest.response;

	if (hhdmRequest.response == NULL) {
		KernelLog("ERROR: Failed to get HHDM offset!");
		HaltAndCatchFire();
	}

	hhdmOffset = hhdmRequest.response->offset;
	PmmInitialize();

	void *ptr = PmmRequestPages(1);
	if (ptr == NULL) {
		KernelLog("ERROR: Failed to allocate a single page for test!");
		HaltAndCatchFire();
	}
	PmmFreePages(ptr, 1);

	if (rsdpRequest.response == NULL) {
		KernelLog("ERROR: Failed to get RSDP!");
		HaltAndCatchFire();
	}
	rsdpResponse = rsdpRequest.response;
	AcpiInitialize();

	if (kernelAddressRequest.response == NULL) {
		KernelLog("ERROR: Failed to get kernel address!");
		HaltAndCatchFire();
	}
	kernelAddressResponse = kernelAddressRequest.response;
	VmmInitialize();

	printf("Initialized VMM\n");

	HaltAndCatchFire();
}
