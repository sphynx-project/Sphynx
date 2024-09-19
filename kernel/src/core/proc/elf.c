// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#include <core/proc/elf.h>
#include <lib/posix/stdio.h>
#include <lib/posix/string.h>
#include <mm/vmm.h>

u64 SpawnElf(u8 *data, PageMap *pm)
{
	if (data == NULL) {
		mprintf("ERROR: Invalid ELF buffer passed, null!\n");
		return 0;
	}

	ElfHeader_t *header = (ElfHeader_t *)data;
	if (header->e_magic != ELF_MAGIC) {
		mprintf("ERROR: Invalid ELF magic number!\n");
		return 0;
	}

	ElfProgramHeader_t *progHeaders =
		(ElfProgramHeader_t *)(data + header->e_phoff);
	for (u16 i = 0; i < header->e_phnum; i++) {
		ElfProgramHeader_t *ph = &progHeaders[i];

		if (ph->p_type != PT_LOAD) {
			continue;
		}

		u64 start = ph->p_vaddr;
		u64 end = start + ph->p_memsz;
		u64 fileEnd = start + ph->p_filesz;
		u64 offset = ph->p_offset;

		int flags = VMM_PRESENT | VMM_WRITE;
		if (ph->p_flags & PF_X) {
			flags |= VMM_EXECUTE;
		}
		if (!(ph->p_flags & PF_W)) {
			flags &= ~VMM_WRITE;
		}

		for (u64 addr = start; addr < end; addr += 0x1000) {
			u64 pageOffset = addr - start + offset;
			u8 *src = (addr < fileEnd) ? (data + pageOffset) : NULL;
			VmmMap(pm, addr, (u64)src, flags);
		}
	}

	return header->e_entry;
}
