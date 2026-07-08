#include "isr.h"

#define VGA_BUFFER     0xB8000
#define WHITE_ON_RED   0x4F
#define VGA_COLS       80

// simple integer-to-string for exception number
static void int_to_str(uint32_t num, char *buf) {
    if (num == 0) {
        buf[0] = '0';
        buf[1] = '\0';
        return;
    }
    int i = 0;
    char tmp[12];
    while (num > 0) {
        tmp[i++] = '0' + (num % 10);
        num /= 10;
    }
    for (int j = 0; j < i; j++) {
        buf[j] = tmp[i - 1 - j];
    }
    buf[i] = '\0';
}

static const char *exception_names[] = {
    "Divide By Zero",
    "Debug",
    "Non-Maskable Interrupt",
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
};

void isr_handler(registers_t r) {
    volatile unsigned char *vga = (volatile unsigned char *)VGA_BUFFER;

    // print on row 2 (below the OS banner) in white-on-red
    int row = 2;
    int col = 0;

    const char *prefix = "EXCEPTION: ";
    for (int i = 0; prefix[i]; i++) {
        vga[(row * VGA_COLS + col) * 2]     = prefix[i];
        vga[(row * VGA_COLS + col) * 2 + 1] = WHITE_ON_RED;
        col++;
    }

    if (r.int_no < 15) {
        const char *name = exception_names[r.int_no];
        for (int i = 0; name[i]; i++) {
            vga[(row * VGA_COLS + col) * 2]     = name[i];
            vga[(row * VGA_COLS + col) * 2 + 1] = WHITE_ON_RED;
            col++;
        }
    } else {
        const char *unk = "INT #";
        for (int i = 0; unk[i]; i++) {
            vga[(row * VGA_COLS + col) * 2]     = unk[i];
            vga[(row * VGA_COLS + col) * 2 + 1] = WHITE_ON_RED;
            col++;
        }
        char num_buf[12];
        int_to_str(r.int_no, num_buf);
        for (int i = 0; num_buf[i]; i++) {
            vga[(row * VGA_COLS + col) * 2]     = num_buf[i];
            vga[(row * VGA_COLS + col) * 2 + 1] = WHITE_ON_RED;
            col++;
        }
    }

    // halt after exception
    for (;;) {}
}
