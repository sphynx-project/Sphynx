// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#pragma once

#include <lib/std/types.h>

// Forward declare Disk and VFS structs
struct Disk;
struct VFS;

typedef struct Disk Disk_t;
typedef struct VFS VFS_t;

typedef u64 (*DiskRead_t)(Disk_t *, const char *, void *);
typedef u64 (*DiskWrite_t)(Disk_t *, const char *, void *, usize);

struct Disk {
	VFS_t *parent; // Pointer to parent VFS
	u8 id; // A-Z
	u64 uuid; // 0 for now
	DiskRead_t read; // u64 read(disk, "\\Hello.txt", &out);
	DiskWrite_t write; // u64 write(disk, "\\Hello.txt", &in, size);
} __attribute__((packed));

struct VFS {
	Disk_t disks[26]; // A-Z
	u64 uuid; // 0 for now
} __attribute__((packed));

void VfsInitialize();
void VfsMount(DiskRead_t read, DiskWrite_t write, u8 id);
void VfsUnMount(u8 id);
u64 VfsRead(const char *path, void *dataOut);
u64 VfsWrite(const char *path, void *dataIn, usize dataInSize);
