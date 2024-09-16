// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#include <core/interrupts/pic.h>
#include <lib/std/io.h>

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1

enum {
	PIC_ICW1_ICW4 = 0x01,
	PIC_ICW1_SINGLE = 0x02,
	PIC_ICW1_INTERVAL4 = 0x04,
	PIC_ICW1_LEVEL = 0x08,
	PIC_ICW1_INITIALIZE = 0x10
} PIC_ICW1;

enum {
	PIC_ICW4_8086 = 0x1,
	PIC_ICW4_AUTO_EOI = 0x2,
	PIC_ICW4_BUFFER_MASTER = 0x4,
	PIC_ICW4_BUFFER_SLAVE = 0x0,
	PIC_ICW4_BUFFERRED = 0x8,
	PIC_ICW4_SFNM = 0x10,
} PIC_ICW4;

enum {
	PIC_CMD_END_OF_INTERRUPT = 0x20,
	PIC_CMD_READ_IRR = 0x0A,
	PIC_CMD_READ_ISR = 0x0B,
} PIC_CMD;

static u16 g_PicMask = 0xFFFF;

void LegacyPicSetMask(u16 newMask)
{
	g_PicMask = newMask;
	SerialIOWriteByte(PIC1_DATA, g_PicMask & 0xFF);
	SerialIOWait();
	SerialIOWriteByte(PIC2_DATA, g_PicMask >> 8);
	SerialIOWait();
}

u16 LegacyPicGetMask()
{
	u16 mask = SerialIOReadByte(PIC1_DATA) | (SerialIOReadByte(PIC2_DATA) << 8);
	return mask;
}

void LegacyPicConfigure(u8 offsetPic1, u8 offsetPic2, bool autoEoi)
{
	LegacyPicSetMask(0xFFFF);

	SerialIOWriteByte(PIC1_COMMAND, PIC_ICW1_INITIALIZE | PIC_ICW1_ICW4);
	SerialIOWait();
	SerialIOWriteByte(PIC2_COMMAND, PIC_ICW1_INITIALIZE | PIC_ICW1_ICW4);
	SerialIOWait();

	SerialIOWriteByte(PIC1_DATA, offsetPic1);
	SerialIOWait();
	SerialIOWriteByte(PIC2_DATA, offsetPic2);
	SerialIOWait();

	SerialIOWriteByte(PIC1_DATA, 0x04); // PIC1 has a slave PIC2 at IRQ2
	SerialIOWait();
	SerialIOWriteByte(PIC2_DATA, 0x02); // PIC2 is connected to PIC1
	SerialIOWait();

	u8 icw4 = PIC_ICW4_8086;
	if (autoEoi) {
		icw4 |= PIC_ICW4_AUTO_EOI;
	}

	SerialIOWriteByte(PIC1_DATA, icw4);
	SerialIOWait();
	SerialIOWriteByte(PIC2_DATA, icw4);
	SerialIOWait();

	LegacyPicSetMask(0xFFFF); // Mask all IRQs initially
}

void LegacyPicSendEndOfInterrupt(int irq)
{
	if (irq >= 8) {
		SerialIOWriteByte(PIC2_COMMAND, PIC_CMD_END_OF_INTERRUPT);
		SerialIOWait();
	}
	SerialIOWriteByte(PIC1_COMMAND, PIC_CMD_END_OF_INTERRUPT);
	SerialIOWait();
}

void LegacyPicDisable()
{
	LegacyPicSetMask(0xFFFF); // Mask all IRQs
}

void LegacyPicEnable()
{
	// Reconfigure PICs
	SerialIOWriteByte(PIC1_COMMAND, PIC_ICW1_INITIALIZE | PIC_ICW1_ICW4);
	SerialIOWait();
	SerialIOWriteByte(PIC2_COMMAND, PIC_ICW1_INITIALIZE | PIC_ICW1_ICW4);
	SerialIOWait();
	SerialIOWriteByte(PIC1_DATA, PIC_REMAP_OFFSET);
	SerialIOWait();
	SerialIOWriteByte(PIC2_DATA, PIC_REMAP_OFFSET + 8);
	SerialIOWait();
	SerialIOWriteByte(PIC1_DATA, 0x04);
	SerialIOWait();
	SerialIOWriteByte(PIC2_DATA, 0x02);
	SerialIOWait();
	SerialIOWriteByte(PIC1_DATA, PIC_ICW4_8086);
	SerialIOWait();
	SerialIOWriteByte(PIC2_DATA, PIC_ICW4_8086);
	SerialIOWait();

	// Unmask specific IRQs
	u8 mask1 = SerialIOReadByte(PIC1_DATA) & ~(1 << 2);
	SerialIOWriteByte(PIC1_DATA, mask1);
	SerialIOWait();

	u8 mask2 = SerialIOReadByte(PIC2_DATA) & ~0x80;
	SerialIOWriteByte(PIC2_DATA, mask2);
	SerialIOWait();
}

void LegacyPicMask(int irq)
{
	LegacyPicSetMask(g_PicMask | (1 << irq));
}

void LegacyPicUnmask(int irq)
{
	LegacyPicSetMask(g_PicMask & ~(1 << irq));
}

u16 LegacyPicReadIrqRequestRegister()
{
	SerialIOWriteByte(PIC1_COMMAND, PIC_CMD_READ_IRR);
	SerialIOWait();
	u16 irr = SerialIOReadByte(PIC1_DATA);
	SerialIOWriteByte(PIC2_COMMAND, PIC_CMD_READ_IRR);
	SerialIOWait();
	irr |= ((u16)SerialIOReadByte(PIC2_DATA)) << 8;
	return irr;
}

u16 LegacyPicReadInServiceRegister()
{
	SerialIOWriteByte(PIC1_COMMAND, PIC_CMD_READ_ISR);
	SerialIOWait();
	u16 isr = SerialIOReadByte(PIC1_DATA);
	SerialIOWriteByte(PIC2_COMMAND, PIC_CMD_READ_ISR);
	SerialIOWait();
	isr |= ((u16)SerialIOReadByte(PIC2_DATA)) << 8;
	return isr;
}
