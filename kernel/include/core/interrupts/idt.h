// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#pragma once

#include <lib/std/types.h>

#define IDT_ENTRY_COUNT 256
#define IRQ_BASE 0x20
#define IRQ_COUNT 16

typedef struct {
	u16 offsetLow;
	u16 selector;
	u8 ist;
	u8 flags;
	u16 offsetMiddle;
	u32 offsetHigh;
	u32 zero;
} __attribute__((packed)) idtEntry_t;

typedef struct {
	u16 limit;
	u64 base;
} __attribute__((packed)) idtPointer_t;

typedef struct {
	u64 ds;
	u64 cr2;
	u64 cr3;

	u64 r15;
	u64 r14;
	u64 r13;
	u64 r12;
	u64 r11;
	u64 r10;
	u64 r9;
	u64 r8;
	u64 rbp;
	u64 rdi;
	u64 rsi;
	u64 rdx;
	u64 rcx;
	u64 rbx;
	u64 rax;

	u64 vector;
	u64 err;

	u64 rip;
	u64 cs;
	u64 rflags;
	u64 rsp;
	u64 ss;
} __attribute__((packed)) Context_t;

extern idtEntry_t idtEntries[IDT_ENTRY_COUNT];

typedef void (*irqHandler_t)(Context_t *);
void IdtInitialize();
void IdtLoad(u64);

void IdtSetGate(idtEntry_t idt[], int num, u64 base, u16 sel, u8 flags);

void IdtIrqRegister(int irq, irqHandler_t handler);
void IdtIrqDeregister(int irq);
