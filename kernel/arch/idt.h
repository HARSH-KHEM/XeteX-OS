#pragma once
#include <stdint.h>

// one IDT entry = 8 bytes, describes an interrupt gate
typedef struct {
    uint16_t offset_low;   // lower 16 bits of handler address
    uint16_t selector;     // kernel code segment selector (0x08)
    uint8_t  zero;         // always zero
    uint8_t  type_attr;    // gate type, DPL, present bit
    uint16_t offset_high;  // upper 16 bits of handler address
} __attribute__((packed)) idt_entry_t;

// IDTR register format -- pointer we pass to lidt
typedef struct {
    uint16_t limit;   // size of IDT - 1
    uint32_t base;    // address of IDT
} __attribute__((packed)) idt_ptr_t;

void idt_init(void);
