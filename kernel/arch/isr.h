#pragma once
#include <stdint.h>

// must match the push order in isr_common_stub (idt.asm)
typedef struct {
    // pushed by isr_common_stub
    uint32_t ds;                                       // saved data segment
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;  // pushed by pusha
    // pushed by our ISR stub
    uint32_t int_no, err_code;
    // pushed by CPU automatically
    uint32_t eip, cs, eflags, useresp, ss;
} __attribute__((packed)) registers_t;

void isr_handler(registers_t r);
