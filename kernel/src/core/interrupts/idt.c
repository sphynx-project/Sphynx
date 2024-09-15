// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#include <core/interrupts/idt.h>
#include <sys/cpu.h>
#include <dev/tty.h>

idtEntry_t idtEntries[IDT_ENTRY_COUNT];
idtPointer_t idtPointer;
extern u64 isrTable[];

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
											"x87 FPU Error"
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

	for (int i = 0; i < IDT_ENTRY_COUNT; ++i) {
		IdtSetGate(idtEntries, i, isrTable[i], 0x08, 0x8E);
	}

	IdtLoad((u64)&idtPointer);
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

		// Parse the error code for page faults
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

		printf("\033[1m\033[34mRegister dump:\033[0m\n");
		printf(
			"  \033[1mrax:\033[0m 0x%.16llx  \033[1mrbx:\033[0m 0x%.16llx  \033[1mrcx:\033[0m 0x%.16llx  \033[1mrdx:\033[0m 0x%.16llx\n",
			frame.rax, frame.rbx, frame.rcx, frame.rdx);
		printf(
			"  \033[1mrsp:\033[0m 0x%.16llx  \033[1mrbp:\033[0m 0x%.16llx  \033[1mrsi:\033[0m 0x%.16llx  \033[1mrdi:\033[0m 0x%.16llx\n",
			frame.rsp, frame.rbp, frame.rsi, frame.rdi);
		printf(
			"  \033[1mr8:\033[0m  0x%.16llx  \033[1mr9:\033[0m  0x%.16llx  \033[1mr10:\033[0m 0x%.16llx  \033[1mr11:\033[0m 0x%.16llx\n",
			frame.r8, frame.r9, frame.r10, frame.r11);
		printf(
			"  \033[1mr12:\033[0m 0x%.16llx  \033[1mr13:\033[0m 0x%.16llx  \033[1mr14:\033[0m 0x%.16llx  \033[1mr15:\033[0m 0x%.16llx\n",
			frame.r12, frame.r13, frame.r14, frame.r15);
		printf(
			"  \033[1mrfl:\033[0m 0x%.16llx  \033[1mrip:\033[0m 0x%.16llx  \033[1mcs:\033[0m  0x%.16llx  \033[1mss:\033[0m  0x%.16llx\n",
			frame.rflags, frame.rip, frame.cs, frame.ss);
		printf(
			"  \033[1mds:\033[0m  0x%.16llx  \033[1mcr2:\033[0m 0x%.16llx  \033[1mcr3:\033[0m 0x%.16llx\n",
			frame.ds, frame.cr2, frame.cr3);

		HaltAndCatchFire();
	} else if (frame.vector >= 0x20 && frame.vector <= 0x2f) {
		// TODO: Handle IRQs
	} else if (frame.vector == 0x80) {
		// TODO: Handle syscalls
	}
}
