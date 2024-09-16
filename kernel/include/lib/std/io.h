// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#pragma once

#include <lib/std/types.h>

void SerialIOWait();
byte SerialIOWriteByte(u16 port, byte data);
byte SerialIOReadByte(u16 port);
u16 SerialIOReadWord(u16 port);
void SerialIOWriteWord(u16 port, u16 data);
u32 SerialIOReadDword(u16 port);
void SerialIOWriteDword(u16 port, u32 data);
