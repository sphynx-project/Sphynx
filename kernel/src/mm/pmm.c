// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#include <mm/pmm.h>

#include <lib/posix/assert.h>
#include <lib/posix/string.h>
#include <sys/boot.h>
#include <sphynx.h>

u8 *bitmap;
u64 bitmapPages;
u64 bitmapSize;

// TODO: Move all these functions into its own files.
void BitmapSet(u8 *bitmap, u64 bit)
{
	bitmap[bit / 8] |= 1 << (bit % 8);
}

void BitmapClear(u8 *bitmap, u64 bit)
{
	bitmap[bit / 8] &= ~(1 << (bit % 8));
}

u8 BitmapGet(u8 *bitmap, u64 bit)
{
	return bitmap[bit / 8] & (1 << (bit % 8));
}

void PmmInitialize()
{
	u64 topAddress;
	u64 higherAddress = 0;

	for (u64 entryCount = 0; entryCount < memoryMap->entry_count;
		 entryCount++) {
		struct limine_memmap_entry *entry = memoryMap->entries[entryCount];

		if (entry->type == LIMINE_MEMMAP_USABLE) {
			topAddress = entry->base + entry->length;
			if (topAddress > higherAddress)
				higherAddress = topAddress;
		}
	}
	bitmapPages = higherAddress / PAGE_SIZE;
	bitmapSize = ALIGN_UP(bitmapPages / 8, PAGE_SIZE);

	for (u64 entryCount = 0; entryCount < memoryMap->entry_count;
		 entryCount++) {
		struct limine_memmap_entry *entry = memoryMap->entries[entryCount];

		if (entry->type == LIMINE_MEMMAP_USABLE) {
			if (entry->length >= bitmapSize) {
				bitmap = (u8 *)(entry->base + hhdmOffset);
				memset(bitmap, 0xFF, bitmapSize);
				entry->base += bitmapSize;
				entry->length -= bitmapSize;
				break;
			}
		}
	}

	for (u64 entryCount = 0; entryCount < memoryMap->entry_count;
		 entryCount++) {
		struct limine_memmap_entry *entry = memoryMap->entries[entryCount];

		if (entry->type == LIMINE_MEMMAP_USABLE) {
			for (u64 i = 0; i < entry->length; i += PAGE_SIZE) {
				BitmapClear(bitmap, (entry->base + i) / PAGE_SIZE);
			}
		}
	}
}

void *PmmRequestPages(usize numPages)
{
	u64 last_allocated_index = 0;

	while (1) {
		if (!BitmapGet(bitmap, last_allocated_index)) {
			usize consecutive_free_pages = 1;

			for (usize i = 1; i < numPages; ++i) {
				if (!BitmapGet(bitmap, last_allocated_index + i)) {
					++consecutive_free_pages;
				} else {
					consecutive_free_pages = 0;
					break;
				}
			}

			if (consecutive_free_pages == numPages) {
				for (usize i = 0; i < numPages; ++i) {
					BitmapSet(bitmap, last_allocated_index + i);
				}

				return (void *)(last_allocated_index * PAGE_SIZE);
			}
		}

		++last_allocated_index;

		if (last_allocated_index >= bitmapPages) {
			return NULL;
		}
	}
}

void PmmFreePages(void *ptr, usize numPages)
{
	assert(ptr != NULL);
	assert((u64)ptr % PAGE_SIZE == 0);
	assert(numPages > 0);

	u64 startBitIdx = ((u64)ptr / PAGE_SIZE);

	for (usize i = startBitIdx; i < numPages; ++i) {
		BitmapClear(bitmap, i);
	}
}