// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#include <dev/vfs.h>
#include <lib/posix/string.h>
#include <lib/posix/stdio.h>
#include <core/bus.h>

static VFS_t vfs;

static void NormalizePath(char *path)
{
	while (*path) {
		if (*path == '\\') {
			*path = '/';
		}
		path++;
	}
}

u8 VfsDevicePoll()
{
	return DEVICE_NOT_READY;
}

u64 VfsDeviceRead(void *out)
{
	(void)out;
	return 0;
}

void VfsDeviceWrite(void *in, usize len)
{
	(void)in;
	(void)len;
}

void VfsInitialize()
{
	memset(&vfs, 0, sizeof(VFS_t));
	DeviceRegister(1, VfsDevicePoll, VfsDeviceRead, VfsDeviceWrite);
}

void VfsMount(DiskRead_t read, DiskWrite_t write, u8 id)
{
	if (id < 'A' || id > 'Z') {
		return;
	}

	u8 index = id - 'A';
	vfs.disks[index].read = read;
	vfs.disks[index].write = write;
	vfs.disks[index].id = id;
	vfs.disks[index].uuid = 0;
	vfs.disks[index].parent = &vfs;
}

void VfsUnMount(u8 id)
{
	if (id < 'A' || id > 'Z') {
		return;
	}

	u8 index = id - 'A';
	memset(&vfs.disks[index], 0, sizeof(Disk_t));
}

u64 VfsRead(const char *path, void *dataOut)
{
	if (path == NULL || dataOut == NULL) {
		return 0;
	}

	char normalizedPath[256];
	strncpy(normalizedPath, path, sizeof(normalizedPath) - 1);
	normalizedPath[sizeof(normalizedPath) - 1] = '\0';
	NormalizePath(normalizedPath);

	if (normalizedPath[0] < 'A' || normalizedPath[0] > 'Z') {
		return 0;
	}

	u8 diskId = (normalizedPath[1] == '/' || normalizedPath[0] == '/') ?
					normalizedPath[1] - 'A' :
					normalizedPath[0] - 'A';
	Disk_t *disk = &vfs.disks[diskId];

	if (disk->read == NULL) {
		return 0;
	}

	return disk->read(disk, normalizedPath + 3, dataOut);
}

u64 VfsWrite(const char *path, void *dataIn, usize dataInSize)
{
	if (path == NULL || dataIn == NULL) {
		return 0;
	}

	char normalizedPath[256];
	strncpy(normalizedPath, path, sizeof(normalizedPath) - 1);
	normalizedPath[sizeof(normalizedPath) - 1] = '\0';

	NormalizePath(normalizedPath);

	if (normalizedPath[1] < 'A' || normalizedPath[1] > 'Z') {
		return 0;
	}

	u8 diskId = (normalizedPath[0] == '/' || normalizedPath[0] == '/') ?
					normalizedPath[1] - 'A' :
					normalizedPath[0] - 'A';
	Disk_t *disk = &vfs.disks[diskId];

	if (disk->write == NULL) {
		return 0;
	}

	return disk->write(disk, normalizedPath + 3, dataIn, dataInSize);
}
