// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#include <core/proc/elf.h>
#include <lib/posix/stdio.h>

void SpawnElf(u8 *data, PageMap *pm)
{
	ElfHeader_t *header = (ElfHeader_t *)data;
	if (header->e_magic != ELF_MAGIC) {
		printf("ERROR: Invalid ELF magic number!\n");
		return;
	}

	ElfProgramHeader_t *progHeaders =
		(ElfProgramHeader_t *)(data + header->e_phoff);
	for (u16 i = 0; i < header->e_phnum; i++) {
		ElfProgramHeader_t *ph = &progHeaders[i];
		if (ph->p_type == PT_LOAD) {
			u64 start = ph->p_vaddr;
			u64 end = start + ph->p_memsz;
			u64 offset = ph->p_offset;

			for (u64 addr = start; addr < end; addr += 0x1000) {
				u64 pageOffset = addr - start + offset;
				VmmMap(pm, (u64)addr, (u64)(data + pageOffset), 1 | 2);
			}
		}
	}
}
