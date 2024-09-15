// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#pragma once

#include <lib/std/types.h>

typedef struct {
	char signature[8];
	u8 checksum;
	char oemId[6];
	u8 revision;
	u32 rsdtAddress;
} __attribute__((packed)) rsdp_t;

typedef struct {
	rsdp_t rsdp;
	u32 length;
	u64 xsdtAddress;
	u8 checksum;
	u8 reserved[3];
} __attribute__((packed)) xsdp_t;

typedef struct {
	char signature[4];
	u32 length;
	u8 revision;
	u8 checksum;
	char oemId[6];
	char oem_tableid[8];
	u32 oem_revision;
	u32 creator_id;
	u32 creator_revision;
} __attribute__((packed)) sdtTable;

void AcpiInitialize();
void *AcpiFindSdt(char *signature);
bool AcpiUseXsdt();
