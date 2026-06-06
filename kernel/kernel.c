#include "gdt.h"
#include "idt.h"

#define VGA_BUFFER     0xB8000
#define WHITE_ON_BLACK 0x0F

void kernel_main(void) {
    gdt_init();  // set up global descriptor table
    idt_init();  // set up interrupt descriptor table

    volatile unsigned char *vga = (volatile unsigned char *)VGA_BUFFER;

    for (int i = 0; i < 80 * 25 * 2; i += 2) {
        vga[i]     = ' ';
        vga[i + 1] = WHITE_ON_BLACK;
    }

    const char *msg = "XETEX OS - Now running in C!";
    for (int i = 0; msg[i] != '\0'; i++) {
        vga[i * 2]     = msg[i];
        vga[i * 2 + 1] = WHITE_ON_BLACK;
    }

    while (1) {}
}
