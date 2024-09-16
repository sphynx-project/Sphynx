// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

// Boot and CPU includes
#include <sphynx.h>
#include <sys/boot.h>
#include <sys/cpu.h>
#include <lib/std/io.h>

// Interrupt includes
#include <core/interrupts/timers/pit.h>
#include <core/interrupts/idt.h>
#include <core/gdt.h>

// Misc core includes
#include <core/processes/scheduler.h>

// ACPI includes
#include <core/acpi/acpi.h>
#include <core/acpi/rsdt.h>
#include <core/acpi/madt.h>

// Memory includes
#include <mm/pmm.h>

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

LIMINE_END

struct flanterm_context *ftCtx;

struct limine_framebuffer *framebuffer;
struct limine_memmap_response *memoryMap;
struct limine_rsdp_response *rsdpResponse;
struct limine_module_response *moduleResponse;
u64 hhdmOffset;

void FramebufferPutPixel(uint32_t x, uint32_t y, uint8_t r, uint8_t g,
						 uint8_t b)
{
	if (x >= framebuffer->width || y >= framebuffer->height) {
		return;
	}

	uint32_t *pixel_addr =
		(uint32_t *)(framebuffer->address + y * framebuffer->pitch +
					 x * (framebuffer->bpp / 8));

	uint32_t color = 0xFF000000 | (r << 16) | (g << 8) | b;

	*pixel_addr = color;
}

void SeifDrawImage(uint8_t *image, u32 x, u32 y)
{
	SEIF_Header *header = (SEIF_Header *)image;
	if (header->magic[0] != 'S' || header->magic[1] != 'E' ||
		header->magic[2] != 'I' || header->magic[3] != 'F') {
		printf("ERROR: Failed to draw image: Invalid image magic!\n");
		return;
	}

	uint8_t encoding_size = 0;
	if (header->encoding == SEIF_ENCODING_RGB) {
		encoding_size = 3;
	} else if (header->encoding == SEIF_ENCODING_RGBA ||
			   header->encoding == SEIF_ENCODING_ARGB) {
		encoding_size = 4;
	}

	SEIF_ChunkHeader *chunk_header =
		(SEIF_ChunkHeader *)(image + sizeof(SEIF_Header));
	if (header->chunk_count != 1 || chunk_header->width != header->meta.width ||
		chunk_header->height != header->meta.height) {
		printf("ERROR: Unsupported image format or multiple chunks!\n");
		return;
	}

	uint8_t *data =
		(uint8_t *)(image + sizeof(SEIF_Header) + sizeof(SEIF_ChunkHeader));

	for (u32 iy = 0; iy < header->meta.height; iy++) {
		for (u32 ix = 0; ix < header->meta.width; ix++) {
			u32 index = (iy * header->meta.width + ix) * encoding_size;

			uint8_t r = 0, g = 0, b = 0, a = 255;

			if (header->encoding == SEIF_ENCODING_RGB) {
				r = data[index];
				g = data[index + 1];
				b = data[index + 2];
			} else if (header->encoding == SEIF_ENCODING_RGBA) {
				r = data[index];
				g = data[index + 1];
				b = data[index + 2];
				a = data[index + 3];
			} else if (header->encoding == SEIF_ENCODING_ARGB) {
				a = data[index];
				r = data[index + 1];
				g = data[index + 2];
				b = data[index + 3];
			}

			if (header->encoding != SEIF_ENCODING_RGB && a == 0) {
				continue;
			}

			FramebufferPutPixel(x + ix, y + iy, r, g, b);
		}
	}
}

void Test()
{
	printf("Hello from task with PID %d\n", processes[currentProcessIndex].pid);
}

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

	int bottom_margin = 5;
	ftCtx = flanterm_fb_init(
		NULL, NULL, framebuffer->address + 8, framebuffer->width,
		(framebuffer->height - 64) - bottom_margin, framebuffer->pitch,
		framebuffer->red_mask_size, framebuffer->red_mask_shift,
		framebuffer->green_mask_size, framebuffer->green_mask_shift,
		framebuffer->blue_mask_size, framebuffer->blue_mask_shift, NULL, NULL,
		NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 1, 0, 0, 0);

	ftCtx->cursor_enabled = false;
	ftCtx->full_refresh(ftCtx);

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

	if (moduleRequest.response == NULL) {
		KernelLog("ERROR: Failed to get modules!");
		HaltAndCatchFire();
	}

	moduleResponse = moduleRequest.response;
	int count = g_acpiCpuCount;

	uint8_t *image = (uint8_t *)moduleResponse->modules[0]->address;
	for (int i = 0; i < count; i++) {
		SeifDrawImage(image, 64 * i,
					  (framebuffer->height - 64) - bottom_margin);
	}

	SchedulerInitialize();
	PitInitialize();

	printf("Sphynx v1.0.0-dev running with %d cores\n", g_acpiCpuCount);
	u64 mem = PmmGetFree();

	printf("Free Memory: ");
	if (mem >= 1024 * 1024 * 1024)
		printf("%dGB", DIV_ROUND_UP(mem, (1024 * 1024 * 1024)));
	else if (mem >= 1024 * 1024)
		printf("%dMB", DIV_ROUND_UP(mem, (1024 * 1024)));
	else if (mem >= 1024)
		printf("%.2fKB", DIV_ROUND_UP(mem, 1024));
	else
		printf("%dB", mem);
	printf(" (%d pages)\n", DIV_ROUND_UP(mem, PAGE_SIZE));

	SchedulerCreateProcess(Test);
	SchedulerCreateProcess(Test);
	SchedulerCreateProcess(Test);
	SchedulerRun();

	HaltAndCatchFire();
}
