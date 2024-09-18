// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

// Credits to: https://github.com/asterd-og/MelOS

#pragma once

#include <lib/std/types.h>

typedef struct VmaRegion {
	u64 virtualAddr;
	u64 end;
	u64 pages;
	u64 flags;
	u64 physicalAddr;
	u64 refCount;
	struct VmaRegion *next;
	struct VmaRegion *prev;
} __attribute__((packed)) VmaRegion;

typedef struct PageMap {
	u64 *topLevel;
	VmaRegion *vmaHead;
} __attribute__((packed)) PageMap;

#define VMM_PRESENT 0x01
#define VMM_WRITE 0x02
#define VMM_USER 0x04
#define VMM_WRITE_THROUGH 0x08
#define VMM_NO_CACHE 0x10
#define VMM_ACCESSED 0x20
#define VMM_DIRTY 0x40
#define VMM_HUGE_PAGE 0x80
#define VMM_GLOBAL 0x100
#define VMM_EXECUTE 0x200

void VmmInitialize();
void VmmSwitchPageMap(PageMap *pageMap);
PageMap *VmmNewPageMap();
u64 VmmGetFlags(u8 present, u8 writable, u8 user, u8 nx);
void VmmMap(PageMap *pageMap, u64 virtualAddress, u64 physicalAddress,
			u64 flags);
PageMap *VmmGetKernelPageMap();
PageMap *VmmGetCurrentPageMap();
void *VmmAlloc(PageMap *pageMap, u64 pages, u64 flags);
void VmmFree(PageMap *pageMap, void *ptr);
