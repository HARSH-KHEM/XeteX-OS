#pragma once
#include <stdint.h>

// one GDT entry = 8 bytes, describes a memory segment
typedef struct {
    uint16_t limit_low;    // lower 16 bits of segment limit
    uint16_t base_low;     // lower 16 bits of base address
    uint8_t  base_mid;     // middle 8 bits of base
    uint8_t  access;       // ring level, type (code/data), present bit
    uint8_t  granularity;  // limit high + flags (32/64 bit, page size)
    uint8_t  base_high;    // upper 8 bits of base
} __attribute__((packed)) gdt_entry_t;

// GDTR register format — pointer we pass to lgdt
typedef struct {
    uint16_t limit;   // size of GDT - 1
    uint32_t base;    // address of GDT
} __attribute__((packed)) gdt_ptr_t;

void gdt_init(void);