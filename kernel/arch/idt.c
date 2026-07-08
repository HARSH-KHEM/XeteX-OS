#include "idt.h"
#include "isr.h"

extern void idt_flush(uint32_t);

// ISR stubs defined in idt.asm
extern void isr0(void);
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);
extern void isr4(void);
extern void isr5(void);
extern void isr6(void);
extern void isr7(void);
extern void isr14(void);

#define IDT_ENTRIES 256

static idt_entry_t idt[IDT_ENTRIES];
static idt_ptr_t   idt_ptr;

// type_attr breakdown:
// bit 7   = present (must be 1)
// bits 5-6 = DPL (0 = kernel only)
// bit 4   = storage segment (0 for interrupt/trap gates)
// bits 0-3 = gate type (0xE = 32-bit interrupt gate)

static void idt_set_gate(uint8_t num, uint32_t base,
                         uint16_t sel, uint8_t flags) {
    idt[num].offset_low  = (base & 0xFFFF);
    idt[num].offset_high = (base >> 16) & 0xFFFF;

    idt[num].selector  = sel;
    idt[num].zero      = 0;
    idt[num].type_attr = flags;
}

void idt_init(void) {
    idt_ptr.limit = (sizeof(idt_entry_t) * IDT_ENTRIES) - 1;
    idt_ptr.base  = (uint32_t)&idt;

    // zero all entries first
    for (int i = 0; i < IDT_ENTRIES; i++) {
        idt_set_gate(i, 0, 0, 0);
    }

    // CPU exceptions 0-7
    // selector 0x08 = kernel code segment, flags 0x8E = present + ring 0 + 32-bit interrupt gate
    idt_set_gate(0, (uint32_t)isr0, 0x08, 0x8E);  // divide by zero
    idt_set_gate(1, (uint32_t)isr1, 0x08, 0x8E);  // debug
    idt_set_gate(2, (uint32_t)isr2, 0x08, 0x8E);  // NMI
    idt_set_gate(3, (uint32_t)isr3, 0x08, 0x8E);  // breakpoint
    idt_set_gate(4, (uint32_t)isr4, 0x08, 0x8E);  // overflow
    idt_set_gate(5, (uint32_t)isr5, 0x08, 0x8E);  // bound range exceeded
    idt_set_gate(6, (uint32_t)isr6, 0x08, 0x8E);  // invalid opcode
    idt_set_gate(7, (uint32_t)isr7, 0x08, 0x8E);  // device not available
    idt_set_gate(14, (uint32_t)isr14, 0x08, 0x8E); // page fault

    idt_flush((uint32_t)&idt_ptr);  // defined in idt.asm
}
