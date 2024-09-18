// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.


#include <core/proc/elf.h>
#include <lib/posix/stdio.h>
#include <dev/vfs.h>

void ElfSpawn(const char* path) {
	char* buffer = VmmAlloc(VmmGetKernelPageMap(), 1, 1 | 2);
    if(buffer == NULL) {
		mprintf("Failed to allocate memory for ELF buffer!\n");
		return;
    }

	u64 bytesRead = VfsRead(path, &buffer);
	if (bytesRead <= 0) {
		mprintf("Failed to read ELF: \"%s\"!\n", path);
        return;
	}

	// TODO: Parse elf and spawn a new task.
    mprintf("%s\n", buffer);
}