// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#pragma once

#include <core/scheduler.h>
#include <lib/std/types.h>
#include <dev/vfs.h>
#include <mm/pmm.h>
#include <mm/vmm.h>

#define ELF_MAGIC 0x464C457F

typedef struct {
	u32 e_magic;
	u8 e_class;
	u8 e_data;
	u8 e_version;
	u8 e_osabi;
	u8 e_abiversion;
	u8 e_pad[7];
	u16 e_type;
	u16 e_machine;
	u32 e_version2;
	u64 e_entry;
	u64 e_phoff;
	u64 e_shoff;
	u32 e_flags;
	u16 e_ehsize;
	u16 e_phentsize;
	u16 e_phnum;
	u16 e_shentsize;
	u16 e_shnum;
	u16 e_shstrndx;
} __attribute__((packed)) ElfHeader_t;

typedef struct {
	u32 p_type;
	u32 p_flags;
	u64 p_offset;
	u64 p_vaddr;
	u64 p_paddr;
	u64 p_filesz;
	u64 p_memsz;
	u64 p_align;
} __attribute__((packed)) ElfProgramHeader_t;

#define PT_NULL 0
#define PT_LOAD 1
#define PT_DYNAMIC 2
#define PT_INTERP 3
#define PT_NOTE 4
#define PT_SHLIB 5
#define PT_PHDR 6
#define PT_TLS 7

#define PF_X 0x1 // Execute
#define PF_W 0x2 // Write
#define PF_R 0x4 // Read

u64 SpawnElf(u8 *data, PageMap *pm);
