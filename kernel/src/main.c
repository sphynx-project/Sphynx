// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#include <sphynx.h>
#include <sys/boot.h>

#include <lib/posix/stdio.h>
#include <core/scheduler.h>
#include <dev/vfs.h>

void Idle()
{
	while (1) {
	}
}

void Init()
{
	const char *path = "A:\\System\\Welcome.txt";
	char buffer[64] = { 0 };
	u64 bytesRead = VfsRead(path, &buffer);
	if (bytesRead > 0) {
		printf("%s\n", buffer);
	} else {
		printf("Failed to read \"%s\"!\n", path);
	}
}