#include "pic.h"

/*
 * 8259 PIC I/O ports
 */
#define PIC1_COMMAND  0x20
#define PIC1_DATA     0x21
#define PIC2_COMMAND  0xA0
#define PIC2_DATA     0xA1

/*
 * ICW (Initialization Command Words)
 */
#define ICW1_INIT     0x10   /* ICW1: begin initialization sequence */
#define ICW1_ICW4     0x01   /* ICW1: ICW4 will be sent */
#define ICW4_8086     0x01   /* ICW4: 8086/88 mode */

#define PIC_EOI       0x20   /* End Of Interrupt command */

/* ── inline port I/O helpers ──────────────────────────────────── */

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port) : "memory");
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port) : "memory");
    return ret;
}

/* small I/O delay — writing to port 0x80 wastes just enough time */
static inline void io_wait(void) {
    __asm__ volatile ("outb %%al, $0x80" : : "a"(0) : "memory");
}

/* ── PIC functions ────────────────────────────────────────────── */

/*
 * pic_init — remap PIC so hardware IRQs don't collide with
 *            CPU exception vectors 0-31.
 *
 *   Master PIC: IRQ 0-7  → INT 0x20-0x27
 *   Slave  PIC: IRQ 8-15 → INT 0x28-0x2F
 *
 * After remapping we mask everything except IRQ0 (timer)
 * and IRQ1 (keyboard).
 */
void pic_init(void) {
    /* save current masks */
    uint8_t mask1 = inb(PIC1_DATA);
    uint8_t mask2 = inb(PIC2_DATA);

    /* ICW1: start init sequence (cascade mode, ICW4 needed) */
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();

    /* ICW2: vector offsets */
    outb(PIC1_DATA, 0x20);      /* master: IRQ0 → INT 0x20 */
    io_wait();
    outb(PIC2_DATA, 0x28);      /* slave:  IRQ8 → INT 0x28 */
    io_wait();

    /* ICW3: tell master that slave is on IRQ2 (bit 2) */
    outb(PIC1_DATA, 0x04);      /* master: slave on IRQ2 */
    io_wait();
    outb(PIC2_DATA, 0x02);      /* slave:  cascade identity = 2 */
    io_wait();

    /* ICW4: 8086 mode */
    outb(PIC1_DATA, ICW4_8086);
    io_wait();
    outb(PIC2_DATA, ICW4_8086);
    io_wait();

    /*
     * Mask all IRQs, then unmask only the ones we need right now:
     *   IRQ0 = timer    (master bit 0)
     *   IRQ1 = keyboard (master bit 1)
     *
     * mask byte: 1 = disabled, 0 = enabled
     *   master: 0xFC  → 1111 1100  (IRQ0 & IRQ1 enabled)
     *   slave:  0xFF  → all masked
     */
    (void)mask1;
    (void)mask2;
    outb(PIC1_DATA, 0xFC);
    outb(PIC2_DATA, 0xFF);
}

/*
 * pic_send_eoi — acknowledge an IRQ so the PIC can send the next one.
 *
 * If the IRQ came from the slave (IRQ 8-15) we must send EOI
 * to both the slave AND the master (since the slave is chained
 * through master IRQ2).
 */
void pic_send_eoi(uint8_t irq) {
    if (irq >= 8) {
        outb(PIC2_COMMAND, PIC_EOI);
    }
    outb(PIC1_COMMAND, PIC_EOI);
}

/*
 * pic_mask_irq — disable a single IRQ line.
 */
void pic_mask_irq(uint8_t irq) {
    uint16_t port;

    if (irq < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq -= 8;
    }

    uint8_t value = inb(port) | (1 << irq);
    outb(port, value);
}

/*
 * pic_unmask_irq — enable a single IRQ line.
 */
void pic_unmask_irq(uint8_t irq) {
    uint16_t port;

    if (irq < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq -= 8;
    }

    uint8_t value = inb(port) & ~(1 << irq);
    outb(port, value);
}
