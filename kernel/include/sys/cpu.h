// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#pragma once

static void HaltAndCatchFire(void)
{
	asm("cli");
	for (;;) {
		asm("hlt");
	}
}

static void Halt(void)
{
	for (;;) {
		asm("hlt");
	}
}