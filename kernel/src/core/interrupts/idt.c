// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#include <core/interrupts/idt.h>
#include <core/interrupts/pic.h>
#include <lib/posix/stdio.h>
#include <sys/cpu.h>
#include <sphynx.h>

idtEntry_t idtEntries[IDT_ENTRY_COUNT];
idtPointer_t idtPointer;
extern u64 isrTable[];

irqHandler_t irqHandlers[IRQ_COUNT] = { 0 };

static const char *exceptionStrings[32] = { "Division By Zero",
											"Debug",
											"Nonmaskable Interrupt",
											"Breakpoint",
											"Overflow",
											"Bound Range Exceeded",
											"Invalid Opcode",
											"Device Not Available",
											"Double Fault",
											"Coprocessor Segment Overrun",
											"Invalid TSS",
											"Segment Not Present",
											"Stack Segment Fault",
											"General Protection Fault",
											"Page Fault",
											"Reserved",
											"x87 FPU Error",
											"Alignment Check",
											"Machine Check",
											"Simd Exception",
											"Virtualization Exception",
											"Control Protection Exception",
											"Reserved",
											"Reserved",
											"Reserved",
											"Reserved",
											"Reserved",
											"Reserved",
											"Hypervisor Injection Exception",
											"VMM Communication Exception",
											"Security Exception",
											"Reserved" };

void IdtSetGate(idtEntry_t idt[], int num, u64 base, u16 sel, u8 flags)
{
	idt[num].offsetLow = (base & 0xFFFF);
	idt[num].offsetMiddle = (base >> 16) & 0xFFFF;
	idt[num].offsetHigh = (base >> 32) & 0xFFFFFFFF;
	idt[num].selector = sel;
	idt[num].ist = 0;
	idt[num].flags = flags;
	idt[num].zero = 0;
}

void IdtInitialize()
{
	idtPointer.limit = sizeof(idtEntry_t) * IDT_ENTRY_COUNT - 1;
	idtPointer.base = (uptr)&idtEntries;

	for (usize i = 0; i < IRQ_COUNT; i++) {
		irqHandlers[i] = NULL;
	}

	// Initialize the PICs
	LegacyPicConfigure(PIC_REMAP_OFFSET, PIC_REMAP_OFFSET + 8, true);
	LegacyPicDisable();

	// Setup IDT entries for exceptions and IRQs
	for (int i = 0; i < 32; ++i) {
		IdtSetGate(idtEntries, i, isrTable[i], 0x08, 0x8E);
	}

	for (int i = IRQ_BASE; i < IRQ_BASE + IRQ_COUNT; ++i) {
		IdtSetGate(idtEntries, i, isrTable[i], 0x08, 0x8E);
	}

	// Load the IDT
	IdtLoad((u64)&idtPointer);

	// Enable the PICs
	LegacyPicEnable();
}

void IdtExcpHandler(intFrame_t frame)
{
	if (frame.vector < 0x20) {
		printf("\n\033[38;2;255;255;255m\033[48;2;255;0;0m");
		printf("#######################################\n");
		printf("###########  KERNEL PANIC  ############\n");
		printf("#######################################\033[0m\n\n");

		printf("\033[1mPanic at address: \033[0m0x%.16llx\n", frame.rip);
		printf("\033[1mException type:   \033[0m%d (%s)\n", frame.vector,
			   exceptionStrings[frame.vector]);
		printf("\033[1mError code:       \033[0m0x%.16llx\n\n", frame.err);

		if (frame.vector == 14) {
			unsigned long long p = frame.err & 1;
			unsigned long long wr = (frame.err >> 1) & 1;
			unsigned long long us = (frame.err >> 2) & 1;
			unsigned long long rsvd = (frame.err >> 3) & 1;
			unsigned long long id = (frame.err >> 4) & 1;

			printf("\033[1mPage Fault Error Code Details:\033[0m\n");
			printf("  \033[1mP:    \033[0m%s\n",
				   p ? "Protection violation" : "Non-present page");
			printf("  \033[1mWR:   \033[0m%s\n", wr ? "Write" : "Read");
			printf("  \033[1mUS:   \033[0m%s\n",
				   us ? "User mode" : "Supervisor mode");
			printf("  \033[1mRSVD: \033[0m%s\n",
				   rsvd ? "Reserved bit violation" :
						  "No reserved bit violation");
			printf("  \033[1mID:   \033[0m%s\n",
				   id ? "Instruction fetch" : "No instruction fetch");
			printf("\n");
		}

		HaltAndCatchFire();
	} else if (frame.vector >= IRQ_BASE &&
			   frame.vector < IRQ_BASE + IRQ_COUNT) {
		int irq = frame.vector - IRQ_BASE;

		if (irqHandlers[irq]) {
			irqHandlers[irq](&frame);
		}

		LegacyPicSendEndOfInterrupt(irq);
	} else if (frame.vector == 0x80) {
	}
}

void IdtIrqRegister(int irq, irqHandler_t handler)
{
	if (irq < 0 || irq >= IRQ_COUNT) {
		printf("Invalid IRQ: %d\n", irq);
		return;
	}
	irqHandlers[irq] = handler;
}

void IdtIrqDeregister(int irq)
{
	if (irq < 0 || irq >= IRQ_COUNT) {
		printf("Invalid IRQ: %d\n", irq);
		return;
	}
	irqHandlers[irq] = 0;
}