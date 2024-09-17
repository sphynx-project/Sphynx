// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#include <dev/fs/ramfs.h>
#include <dev/vfs.h>
#include <lib/posix/string.h>
#include <lib/posix/stdio.h>

#define TAR_BLOCK_SIZE 512

typedef struct {
	char name[100];
	char mode[8];
	char uid[8];
	char gid[8];
	char size[12];
	char mtime[12];
	char checksum[8];
	char typeflag;
	char linkname[100];
	char magic[6];
	char version[2];
	char uname[32];
	char gname[32];
	char devmajor[8];
	char devminor[8];
	char prefix[155];
} __attribute__((packed)) TarHeader_t;

typedef struct {
	const char *name;
	const void *data;
	u64 size;
	u8 isDirectory;
} RamFile_t;

static RamFile_t ramFiles[128];
static u32 fileCount = 0;

static u32 OctalToDecimal(const char *octalStr)
{
	u32 result = 0;
	while (*octalStr) {
		result = (result << 3) + (*octalStr++ - '0');
	}
	return result;
}

static void ParseTar(const void *buffer, usize size)
{
	const char *ptr = (const char *)buffer;
	const char *end = ptr + size;

	while (ptr < end) {
		const TarHeader_t *header = (const TarHeader_t *)ptr;
		if (header->name[0] == '\0') {
			break;
		}

		u32 fileSize = OctalToDecimal(header->size);
		char typeFlag = header->typeflag;

		if (fileCount < 128) {
			ramFiles[fileCount].name = header->name;
			ramFiles[fileCount].size = fileSize;
			ramFiles[fileCount].data = (const void *)(ptr + TAR_BLOCK_SIZE);
			ramFiles[fileCount].isDirectory = (typeFlag == '5');
			fileCount++;
		}

		ptr +=
			TAR_BLOCK_SIZE +
			((fileSize + TAR_BLOCK_SIZE - 1) / TAR_BLOCK_SIZE) * TAR_BLOCK_SIZE;
	}
}

static u64 RamfsRead(Disk_t *disk, const char *path, void *dataOut)
{
	(void)disk;
	for (u32 i = 0; i < fileCount; i++) {
		if (strcmp(ramFiles[i].name, path) == 0) {
			if (ramFiles[i].isDirectory) {
				printf("ERROR: \"%s\" is a directory!\n", path);
				return 0;
			} else {
				memcpy(dataOut, ramFiles[i].data, ramFiles[i].size);
				return ramFiles[i].size;
			}
		}
	}

	printf("ERROR: Didnt find file \"%s\" in ramfs!\n", path);

	return 0;
}

static u64 RamfsWrite(Disk_t *disk, const char *path, void *dataIn, usize size)
{
	(void)disk;
	(void)path;
	(void)dataIn;
	(void)size;
	return 0; // Read-only file system
}

void RamfsInit(void *buffer, usize size)
{
	ParseTar(buffer, size);
	VfsMount(RamfsRead, RamfsWrite, 'A');
}
