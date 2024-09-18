// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#pragma once

#include <lib/std/types.h>

#define DEVICE_READY 0;
#define DEVICE_NOT_READY 0

typedef struct {
	u32 id;
	u8 (*poll)(void);
	u64 (*read)(void *);
	void (*write)(void *data, usize size);
} DeviceHandle_t;

void DeviceRegister(u32 id, u8 (*poll)(void), u64 (*read)(void *),
					void (*write)(void *data, usize size));
DeviceHandle_t *DeviceGet(u32 id);
void DeviceDeregister(u32 id);