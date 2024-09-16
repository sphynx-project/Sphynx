// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#pragma once

#include <lib/std/types.h>

#define PIC_REMAP_OFFSET 0x20

void LegacyPicConfigure(u8 offsetPic1, u8 offsetPic2, bool autoEoi);
void LegacyPicDisable();
void LegacyPicEnable();
void LegacyPicMask(int irq);
void LegacyPicUnmask(int irq);
void LegacyPicSendEndOfInterrupt(int irq);

u16 LegacyPicReadIrqRequestRegister();
u16 LegacyPicReadInServiceRegister();