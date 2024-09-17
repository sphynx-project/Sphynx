// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#include <sphynx.h>
#include <sys/boot.h>

#include <lib/posix/stdio.h>
#include <core/scheduler.h>
#include <mm/pmm.h>
#include <core/acpi/madt.h>

void Init()
{
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
}