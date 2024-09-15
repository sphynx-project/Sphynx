// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#include <sys/boot.h>
#include <sys/cpu.h>

#include <flanterm/flanterm.h>
#include <flanterm/backends/fb.h>

#include <dev/tty.h>

#include <stdbool.h>
#include <stddef.h>

LIMINE_START

LIMINE_REQUEST_SECTION static volatile LIMINE_BASE_REVISION(2);
LIMINE_REQUEST limine_framebuffer_request
	framebuffer_request = { .id = LIMINE_FRAMEBUFFER_REQUEST, .revision = 0 };

LIMINE_END

struct limine_framebuffer *framebuffer;
struct flanterm_context *ftCtx;

void KernelEntry(void)
{
	if (LIMINE_BASE_REVISION_SUPPORTED == false) {
		HaltAndCatchFire();
	}

	if (framebuffer_request.response == NULL ||
		framebuffer_request.response->framebuffer_count < 1) {
		HaltAndCatchFire();
	}

	framebuffer = framebuffer_request.response->framebuffers[0];

	ftCtx = flanterm_fb_init(
		NULL, NULL, framebuffer->address, framebuffer->width,
		framebuffer->height, framebuffer->pitch, framebuffer->red_mask_size,
		framebuffer->red_mask_shift, framebuffer->green_mask_size,
		framebuffer->green_mask_shift, framebuffer->blue_mask_size,
		framebuffer->blue_mask_shift, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		NULL, 0, 0, 1, 0, 0, 0);

	ftCtx->cursor_enabled = false;
	ftCtx->full_refresh(ftCtx);

	printf("Sphynx 1.0.0-dev\n");

	HaltAndCatchFire();
}
