// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

// Credits to: https://github.com/asterd-og/MelOS

#include <mm/vmm.h>
#include <mm/pmm.h>
#include <lib/posix/string.h>
#include <sys/boot.h>
#include <sphynx.h>

extern char __textStart[];
extern char __textEnd[];
extern char __roDataStart[];
extern char __roDataEnd[];
extern char __dataStart[];
extern char __dataEnd[];

PageMap *vmmKernelPageMap;
PageMap *vmmCurrentPageMap;

u64 GetPageSize()
{
	return 0x1000;
}

void WriteCR3(u64 addr)
{
	__asm__ volatile("mov %0, %%cr3" : : "r"(addr) : "memory");
}

u64 PteGetAddr(u64 pte)
{
	return pte & 0x000ffffffffff000;
}

u64 PteGetFlags(u64 pte)
{
	return pte & ~0x000ffffffffff000;
}

void VmmInitialize()
{
	vmmKernelPageMap = (PageMap *)PHYS_TO_VIRT(PmmRequestPages(1));
	memset(vmmKernelPageMap, 0, GetPageSize());

	vmmKernelPageMap->topLevel =
		(u64 *)(PageMap *)PHYS_TO_VIRT(PmmRequestPages(1));
	memset(vmmKernelPageMap->topLevel, 0, GetPageSize());

	vmmKernelPageMap->vmaHead = (VmaRegion *)PHYS_TO_VIRT(PmmRequestPages(1));
	memset(vmmKernelPageMap->vmaHead, 0, GetPageSize());

	vmmKernelPageMap->vmaHead->next = vmmKernelPageMap->vmaHead;
	vmmKernelPageMap->vmaHead->prev = vmmKernelPageMap->vmaHead;

	u64 physBase = kernelAddressResponse->physical_base;
	u64 virtBase = kernelAddressResponse->virtual_base;

	u64 textStart = ALIGN_DOWN((u64)__textStart, GetPageSize());
	u64 textEnd = ALIGN_UP((u64)__textEnd, GetPageSize());

	u64 roDataStart = ALIGN_DOWN((u64)__roDataStart, GetPageSize());
	u64 roDataEnd = ALIGN_UP((u64)__roDataEnd, GetPageSize());

	u64 dataStart = ALIGN_DOWN((u64)__dataStart, GetPageSize());
	u64 dataEnd = ALIGN_UP((u64)__dataEnd, GetPageSize());

	for (u64 text = textStart; text < textEnd; text += GetPageSize())
		VmmMap(vmmKernelPageMap, text, text - virtBase + physBase, 1);

	for (u64 roData = roDataStart; roData < roDataEnd; roData += GetPageSize())
		VmmMap(vmmKernelPageMap, roData, roData - virtBase + physBase,
			   1 | ((u64)1 << 63));

	for (u64 data = dataStart; data < dataEnd; data += GetPageSize())
		VmmMap(vmmKernelPageMap, data, data - virtBase + physBase,
			   1 | 2 | ((u64)1 << 63));

	for (u64 gb4 = 0; gb4 < 0x100000000; gb4 += GetPageSize()) {
		VmmMap(vmmKernelPageMap, gb4, gb4, 1 | 2);
		VmmMap(vmmKernelPageMap, (u64)PHYS_TO_VIRT(gb4), gb4, 1 | 2);
	}

	VmmSwitchPageMap(vmmKernelPageMap);
}

u64 VmmGetFlags(u8 present, u8 writable, u8 user, u8 nx)
{
	u64 flags = 0;
	if (present)
		flags |= 1;
	if (writable)
		flags |= 2;
	if (user)
		flags |= 4;
	if (nx)
		flags |= ((u64)1 << 63);
	return flags;
}

void VmmSwitchPageMap(PageMap *pageMap)
{
	vmmCurrentPageMap = pageMap;
	WriteCR3((u64)VIRT_TO_PHYS(pageMap->topLevel));
}

PageMap *VmmNewPageMap()
{
	PageMap *pageMap = (PageMap *)PHYS_TO_VIRT(PmmRequestPages(1));
	memset(pageMap, 0, GetPageSize());
	pageMap->topLevel = (u64 *)PHYS_TO_VIRT(PmmRequestPages(1));
	memset(pageMap->topLevel, 0, GetPageSize());
	for (int i = 256; i < 512; i++) {
		pageMap->topLevel[i] = vmmKernelPageMap->topLevel[i];
	}

	return pageMap;
}

u64 *VmmGetNextLevel(u64 *level, u64 entry)
{
	if (level[entry] & 1)
		return PHYS_TO_VIRT(PteGetAddr(level[entry]));
	u64 *PML = PHYS_TO_VIRT(PmmRequestPages(1));
	memset(PML, 0, GetPageSize());
	level[entry] = (u64)VIRT_TO_PHYS(PML) | 1 | 2 | 3;
	return PML;
}

void VmmMap(PageMap *pageMap, u64 virtualAddr, u64 physicalAddr, u64 flags)
{
	u64 PML1Entry = (virtualAddr >> 12) & 0x1ff;
	u64 PML2Entry = (virtualAddr >> 21) & 0x1ff;
	u64 PML3Entry = (virtualAddr >> 30) & 0x1ff;
	u64 PML4Entry = (virtualAddr >> 39) & 0x1ff;

	u64 *PML3 = VmmGetNextLevel(pageMap->topLevel, PML4Entry);
	u64 *PML2 = VmmGetNextLevel(PML3, PML3Entry);
	u64 *PML1 = VmmGetNextLevel(PML2, PML2Entry);

	PML1[PML1Entry] = physicalAddr | flags;
}

PageMap *VmmGetKernelPageMap()
{
	return vmmKernelPageMap;
}

PageMap *VmmGetCurrentPageMap()
{
	return vmmCurrentPageMap;
}

VmaRegion *VmmCreateRegion(PageMap *pageMap, u64 virtualAddr, u64 physicalAddr,
						   u64 pages, u64 flags)
{
	VmaRegion *region = (VmaRegion *)PHYS_TO_VIRT(PmmRequestPages(1));
	region->virtualAddr = virtualAddr;
	region->physicalAddr = physicalAddr;
	region->pages = pages;
	region->flags = flags;
	region->refCount = 0;
	region->next = pageMap->vmaHead;
	region->prev = pageMap->vmaHead->prev;
	region->end = virtualAddr + (pages * GetPageSize());

	pageMap->vmaHead->prev->next = region;
	pageMap->vmaHead->prev = region;

	return region;
}

VmaRegion *VmmInsertAfterRegion(VmaRegion *prev, u64 virtualAddr,
								u64 physicalAddr, u64 pages, u64 flags)
{
	VmaRegion *region = (VmaRegion *)PHYS_TO_VIRT(PmmRequestPages(1));
	region->virtualAddr = virtualAddr;
	region->physicalAddr = physicalAddr;
	region->pages = pages;
	region->flags = flags;
	region->refCount = 0;
	region->next = prev->next;
	region->prev = prev;
	prev->next->prev = region;
	prev->next = region;
	return region;
}

void VmmDeleteRegion(VmaRegion *region)
{
	region->prev->next = region->next;
	region->next->prev = region->prev;
	PmmFreePages((u64 *)VIRT_TO_PHYS(region), 1);
}

void *VmmAlloc(PageMap *pageMap, u64 pages, u64 flags)
{
	void *page = PmmRequestPages(pages);
	if (!page)
		return NULL;

	u64 vAddr = pageMap->vmaHead->prev->end + GetPageSize();
	VmaRegion *region;
	int found = 0;

	for (region = pageMap->vmaHead->next; region != pageMap->vmaHead;
		 region = region->next) {
		if (region->next->virtualAddr - region->end >=
			(pages * GetPageSize())) {
			vAddr = region->end;
			found = 1;
			region =
				VmmInsertAfterRegion(region, vAddr, (u64)page, pages, flags);
			break;
		}
	}

	if (!found)
		region = VmmCreateRegion(pageMap, vAddr, (u64)page, pages, flags);

	for (u64 i = 0; i < pages; i++)
		VmmMap(pageMap, vAddr + (i * GetPageSize()),
			   (u64)page + (i * GetPageSize()), flags);

	return (void *)vAddr;
}

void VmmFree(PageMap *pageMap, void *ptr)
{
	VmaRegion *region;
	if (pageMap->vmaHead == NULL) {
		return;
	}

	for (region = pageMap->vmaHead->next; region != pageMap->vmaHead;
		 region = region->next) {
		if (ptr >= (void *)region->virtualAddr && ptr < (void *)(region->end)) {
			u64 pages = (region->end - (u64)ptr) / GetPageSize();
			for (u64 i = 0; i < pages; i++)
				VmmMap(pageMap, (u64)ptr + (i * GetPageSize()), 0, 0);
			VmmDeleteRegion(region);
			return;
		}
	}
}