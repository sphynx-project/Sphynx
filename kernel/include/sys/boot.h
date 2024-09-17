// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#pragma once

#include <limine.h>
#include <sphynx.h>
#include <lib/std/types.h>
#include <flanterm/flanterm.h>
#include <flanterm/backends/fb.h>

#define LIMINE_REQUEST_SECTION __attribute__((used, section(".requests")))
#define LIMINE_START_MARKER_SECTION \
	__attribute__((used, section(".requests_start_marker")))
#define LIMINE_END_MARKER_SECTION \
	__attribute__((used, section(".requests_end_marker")))
#define LIMINE_REQUEST LIMINE_REQUEST_SECTION static volatile struct

#define LIMINE_START \
	LIMINE_START_MARKER_SECTION static volatile LIMINE_REQUESTS_START_MARKER;

#define LIMINE_END \
	LIMINE_END_MARKER_SECTION static volatile LIMINE_REQUESTS_END_MARKER;

extern struct flanterm_context *ftCtx;

extern struct limine_framebuffer *framebuffer;
extern struct limine_memmap_response *memoryMap;
extern struct limine_rsdp_response *rsdpResponse;
extern struct limine_module_response *moduleResponse;
extern struct limine_kernel_address_response *kernelAddressResponse;
extern u64 hhdmOffset;
