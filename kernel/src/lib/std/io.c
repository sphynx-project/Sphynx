// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#include <lib/std/io.h>

#define UNUSED 0x80

void SerialIOWait()
{
	SerialIOWriteByte(UNUSED, 0);
}

byte SerialIOWriteByte(u16 port, byte data)
{
	__asm__ volatile("outb %0, %1" : : "a"(data), "Nd"(port));
	return data;
}

byte SerialIOReadByte(u16 port)
{
	byte result;
	__asm__ volatile("inb %1, %0" : "=a"(result) : "Nd"(port));
	return result;
}

u16 SerialIOReadWord(u16 port)
{
	u16 result;
	__asm__ volatile("inw %1, %0" : "=a"(result) : "Nd"(port));
	return result;
}

void SerialIOWriteWord(u16 port, u16 data)
{
	__asm__ volatile("outw %0, %1" : : "a"(data), "Nd"(port));
}

u32 SerialIOReadDword(u16 port)
{
	u32 result;
	__asm__ volatile("inl %1, %0" : "=a"(result) : "Nd"(port));
	return result;
}

void SerialIOWriteDword(u16 port, u32 data)
{
	__asm__ volatile("outl %0, %1" : : "a"(data), "Nd"(port));
}
