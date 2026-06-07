#pragma once
#include <stdint.h>

/*
 * 8259 PIC (Programmable Interrupt Controller)
 *
 * After remapping:
 *   Master PIC: IRQ 0-7  → INT 0x20-0x27
 *   Slave  PIC: IRQ 8-15 → INT 0x28-0x2F
 */

void pic_init(void);
void pic_send_eoi(uint8_t irq);
void pic_mask_irq(uint8_t irq);
void pic_unmask_irq(uint8_t irq);
