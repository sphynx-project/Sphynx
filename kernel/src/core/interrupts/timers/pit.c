// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#include <core/interrupts/timers/pit.h>
#include <core/interrupts/idt.h>
#include <core/scheduler.h>
#include <lib/posix/stdio.h>

static u64 uptimeSeconds = 0;
static u16 uptimeMilliseconds = 0;
static u64 countdown = 0;

void PitInt();

void PitSetDivisor(u16 divisor)
{
	if (divisor < 100) {
		divisor = 100;
	}
	SerialIOWriteByte(PIT_DATA, (u8)(divisor & 0x00FF));
	SerialIOWait();
	SerialIOWriteByte(PIT_DATA, (u8)((divisor & 0xFF00) >> 8));
	SerialIOWait();
}

u16 PitReadCount()
{
	u16 count = 0;
	__asm__ volatile("cli");
	SerialIOWriteByte(PIT_COMMAND, 0x00);
	SerialIOWait();
	count = SerialIOReadByte(PIT_DATA);
	count |= (SerialIOReadByte(PIT_DATA) << 8);
	__asm__ volatile("sti");
	return count;
}

void PitSetCount(u16 count)
{
	__asm__ volatile("cli");
	SerialIOWriteByte(PIT_DATA, count & 0xFF);
	SerialIOWait();
	SerialIOWriteByte(PIT_DATA, (count >> 8) & 0xFF);
	SerialIOWait();
	__asm__ volatile("sti");
}

void PitInterruptHandler(Context_t *frame)
{
	SchedulerTick(frame);
	PitInt();
}

void PitInitialize()
{
	PitSetCount(0);
	PitSetDivisor(1193182 / 1000);
	countdown = 0;
	uptimeMilliseconds = 0;
	uptimeSeconds = 0;
	IdtIrqRegister(0, PitInterruptHandler);
}

u64 PitGetUptimeSeconds()
{
	return uptimeSeconds;
}

u64 PitGetUptimeMilliseconds()
{
	return uptimeMilliseconds;
}

void PitInt()
{
	uptimeMilliseconds++;
	if (uptimeMilliseconds >= 1000) {
		uptimeSeconds++;
		uptimeMilliseconds = 0;
	}

	if (countdown > 0) {
		countdown--;
	}
}

void PitSleep(u64 milliseconds)
{
	countdown = milliseconds;
	while (countdown > 0) {
		__asm__ volatile("hlt");
	}
}