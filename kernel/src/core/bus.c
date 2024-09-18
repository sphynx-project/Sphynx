// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#include <core/bus.h>

#define MAX_DEVICES 256

static DeviceHandle_t devices[MAX_DEVICES] = { { 0 } };

void DeviceRegister(u32 id, u8 (*poll)(void), u64 (*read)(void *),
					void (*write)(void *data, usize size))
{
	if (id >= MAX_DEVICES) {
		return;
	}
	devices[id].id = id;
	devices[id].poll = poll;
	devices[id].read = read;
	devices[id].write = write;
}

DeviceHandle_t *DeviceGet(u32 id)
{
	if (id >= MAX_DEVICES) {
		return NULL;
	}

	if (devices[id].read == 0) {
		return NULL;
	}

	return &devices[id];
}

void DeviceDeregister(u32 id)
{
	if (id < MAX_DEVICES) {
		devices[id].id = 0;
		devices[id].poll = NULL;
		devices[id].read = NULL;
		devices[id].write = NULL;
	}
}
