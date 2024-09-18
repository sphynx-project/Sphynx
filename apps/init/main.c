// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#include <string.h>
#include <syscall.h>
#include <types.h>

void puts(const char *str)
{
	sys_write(0, str, strlen(str));
}

void draw_rect(int x, int y, int w, int h, u8 r, u8 g, u8 b)
{
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			sys_put_pixel(x + j, y + i, r, g, b);
		}
	}
}

void _start(void)
{
	puts("SphynxOS booted yippie!\n");
	draw_rect(400, 400, 100, 100, 255, 255, 255);
	puts("time to die >:D\n");
	sys_exit(0);
}
