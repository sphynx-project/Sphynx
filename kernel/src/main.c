// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#include <sys/boot.h>
#include <sys/cpu.h>

#include <flanterm/flanterm.h>
#include <flanterm/backends/fb.h>

#include <stdbool.h>
#include <stddef.h>

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

	struct limine_framebuffer *framebuffer =
		framebuffer_request.response->framebuffers[0];

	ftCtx = flanterm_fb_init(
		NULL, NULL, framebuffer->address, framebuffer->width,
		framebuffer->height, framebuffer->pitch, framebuffer->red_mask_size,
		framebuffer->red_mask_shift, framebuffer->green_mask_size,
		framebuffer->green_mask_shift, framebuffer->blue_mask_size,
		framebuffer->blue_mask_shift, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		NULL, 0, 0, 1, 0, 0, 0);

	ftCtx->cursor_enabled = false;
	ftCtx->full_refresh(ftCtx);

	const char msg[] = "Hello, World!\n";
	flanterm_write(ftCtx, msg, sizeof(msg));

	HaltAndCatchFire();
}
