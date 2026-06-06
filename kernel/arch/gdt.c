#include "gdt.h"
extern void gdt_flush(uint32_t); 

#define GDT_ENTRIES 5

static gdt_entry_t gdt[GDT_ENTRIES];
static gdt_ptr_t   gdt_ptr;

// access byte breakdown:
// bit 7   = present (must be 1)
// bits 5-6 = privilege ring (0=kernel, 3=user)
// bit 4   = descriptor type (1=code/data)
// bit 3   = executable (1=code segment)
// bit 1   = readable/writable

static void gdt_set_entry(int i, uint32_t base, uint32_t limit,
                           uint8_t access, uint8_t gran) {
    gdt[i].base_low  = (base & 0xFFFF);
    gdt[i].base_mid  = (base >> 16) & 0xFF;
    gdt[i].base_high = (base >> 24) & 0xFF;

    gdt[i].limit_low   = (limit & 0xFFFF);
    gdt[i].granularity = ((limit >> 16) & 0x0F) | (gran & 0xF0);

    gdt[i].access = access;
}

void gdt_init(void) {
    gdt_ptr.limit = (sizeof(gdt_entry_t) * GDT_ENTRIES) - 1;
    gdt_ptr.base  = (uint32_t)&gdt;

    gdt_set_entry(0, 0, 0,          0x00, 0x00); // null — required
    gdt_set_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // kernel code ring 0
    gdt_set_entry(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // kernel data ring 0
    gdt_set_entry(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // user code   ring 3
    gdt_set_entry(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // user data   ring 3

    gdt_flush((uint32_t)&gdt_ptr); // defined in gdt.asm
}