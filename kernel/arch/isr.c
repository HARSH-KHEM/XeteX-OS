#include "isr.h"

#define VGA_BUFFER   0xB8000
#define WHITE_ON_RED 0x4F
#define VGA_COLS     80

extern uint32_t read_cr2(void);

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

static void int_to_str(uint32_t num, char *buffer)
{
    if (num == 0)
    {
        buffer[0] = '0';
        buffer[1] = '\0';
        return;
    }

    char reversed[12];
    int length = 0;

    while (num > 0)
    {
        reversed[length++] = '0' + (num % 10);
        num /= 10;
    }

    for (int i = 0; i < length; i++)
    {
        buffer[i] = reversed[length - 1 - i];
    }

    buffer[length] = '\0';
}

static void int_to_hex(uint32_t value, char *buffer)
{
    const char hex_digits[] = "0123456789ABCDEF";

    for (int i = 7; i >= 0; i--)
    {
        buffer[i] = hex_digits[value & 0xF];
        value >>= 4;
    }

    buffer[8] = '\0';
}

static void vga_write_text(int row, int *col, const char *text)
{
    volatile unsigned char *vga =
        (volatile unsigned char *)VGA_BUFFER;

    for (int i = 0;
         text[i] != '\0' && *col < VGA_COLS;
         i++)
    {
        uint32_t index =
            (row * VGA_COLS + *col) * 2;

        vga[index] = text[i];
        vga[index + 1] = WHITE_ON_RED;

        (*col)++;
    }
}

static void halt_after_exception(void)
{
    for (;;)
    {
        __asm__ volatile("cli; hlt");
    }
}

static void handle_page_fault(registers_t r)
{
    uint32_t fault_address = read_cr2();
    uint32_t error_code = r.err_code;

    int row = 2;
    int col = 0;

    vga_write_text(row, &col, "PAGE FAULT");

    row++;
    col = 0;

    vga_write_text(row, &col, "ADDRESS: 0x");

    char address_buffer[9];
    int_to_hex(fault_address, address_buffer);

    vga_write_text(row, &col, address_buffer);

    row++;
    col = 0;

    if (error_code & 0x1)
    {
        vga_write_text(
            row,
            &col,
            "REASON: PROTECTION VIOLATION"
        );
    }
    else
    {
        vga_write_text(
            row,
            &col,
            "REASON: PAGE NOT PRESENT"
        );
    }

    row++;
    col = 0;

    if (error_code & 0x2)
    {
        vga_write_text(
            row,
            &col,
            "OPERATION: WRITE"
        );
    }
    else
    {
        vga_write_text(
            row,
            &col,
            "OPERATION: READ"
        );
    }

    row++;
    col = 0;

    if (error_code & 0x4)
    {
        vga_write_text(
            row,
            &col,
            "MODE: USER"
        );
    }
    else
    {
        vga_write_text(
            row,
            &col,
            "MODE: KERNEL"
        );
    }

    halt_after_exception();
}

void isr_handler(registers_t r)
{
    if (r.int_no == 14)
    {
        handle_page_fault(r);
    }

    int row = 2;
    int col = 0;

    vga_write_text(row, &col, "EXCEPTION: ");

    if (r.int_no < 15)
    {
        vga_write_text(
            row,
            &col,
            exception_names[r.int_no]
        );
    }
    else
    {
        vga_write_text(row, &col, "INT #");

        char number_buffer[12];
        int_to_str(r.int_no, number_buffer);

        vga_write_text(
            row,
            &col,
            number_buffer
        );
    }

    halt_after_exception();
}