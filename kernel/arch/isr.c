#include "isr.h"

extern uint32_t read_cr2(void);
extern uint32_t read_cr2(void);

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

void isr_handler(registers_t r)
{
    volatile unsigned char *vga =
        (volatile unsigned char *)VGA_BUFFER;

    int row = 2;
    int col = 0;

    /*
     * Special handling for page fault.
     */
    if (r.int_no == 14)
    {
        uint32_t fault_address = read_cr2();
        uint32_t error_code = r.err_code;

        const char *title = "PAGE FAULT";

        for (int i = 0; title[i]; i++)
        {
            vga[(row * VGA_COLS + col) * 2] = title[i];
            vga[(row * VGA_COLS + col) * 2 + 1] = WHITE_ON_RED;
            col++;
        }

        row++;
        col = 0;

        const char *address_text = "ADDRESS: 0x";

        for (int i = 0; address_text[i]; i++)
        {
            vga[(row * VGA_COLS + col) * 2] = address_text[i];
            vga[(row * VGA_COLS + col) * 2 + 1] = WHITE_ON_RED;
            col++;
        }

        char address_buffer[12];
        int_to_hex(fault_address, address_buffer);

        for (int i = 0; address_buffer[i]; i++)
        {
            vga[(row * VGA_COLS + col) * 2] = address_buffer[i];
            vga[(row * VGA_COLS + col) * 2 + 1] = WHITE_ON_RED;
            col++;
        }

        row++;
        col = 0;

        const char *reason;

        if (error_code & 0x1)
        {
            reason = "REASON: PROTECTION VIOLATION";
        }
        else
        {
            reason = "REASON: PAGE NOT PRESENT";
        }

        for (int i = 0; reason[i]; i++)
        {
            vga[(row * VGA_COLS + col) * 2] = reason[i];
            vga[(row * VGA_COLS + col) * 2 + 1] = WHITE_ON_RED;
            col++;
        }

        row++;
        col = 0;

        const char *operation;

        if (error_code & 0x2)
        {
            operation = "OPERATION: WRITE";
        }
        else
        {
            operation = "OPERATION: READ";
        }

        for (int i = 0; operation[i]; i++)
        {
            vga[(row * VGA_COLS + col) * 2] = operation[i];
            vga[(row * VGA_COLS + col) * 2 + 1] = WHITE_ON_RED;
            col++;
        }

        row++;
        col = 0;

        const char *privilege;

        if (error_code & 0x4)
        {
            privilege = "MODE: USER";
        }
        else
        {
            privilege = "MODE: KERNEL";
        }

        for (int i = 0; privilege[i]; i++)
        {
            vga[(row * VGA_COLS + col) * 2] = privilege[i];
            vga[(row * VGA_COLS + col) * 2 + 1] = WHITE_ON_RED;
            col++;
        }

        for (;;)
        {
            __asm__ volatile("cli; hlt");
        }
    }

    /*
     * Generic exception handling.
     */
    const char *prefix = "EXCEPTION: ";

    for (int i = 0; prefix[i]; i++)
    {
        vga[(row * VGA_COLS + col) * 2] = prefix[i];
        vga[(row * VGA_COLS + col) * 2 + 1] = WHITE_ON_RED;
        col++;
    }

    if (r.int_no < 15)
    {
        const char *name = exception_names[r.int_no];

        for (int i = 0; name[i]; i++)
        {
            vga[(row * VGA_COLS + col) * 2] = name[i];
            vga[(row * VGA_COLS + col) * 2 + 1] = WHITE_ON_RED;
            col++;
        }
    }
    else
    {
        const char *unknown = "INT #";

        for (int i = 0; unknown[i]; i++)
        {
            vga[(row * VGA_COLS + col) * 2] = unknown[i];
            vga[(row * VGA_COLS + col) * 2 + 1] = WHITE_ON_RED;
            col++;
        }

        char number_buffer[12];
        int_to_str(r.int_no, number_buffer);

        for (int i = 0; number_buffer[i]; i++)
        {
            vga[(row * VGA_COLS + col) * 2] = number_buffer[i];
            vga[(row * VGA_COLS + col) * 2 + 1] = WHITE_ON_RED;
            col++;
        }
    }

    for (;;)
    {
        __asm__ volatile("cli; hlt");
    }
}

static void handle_page_fault(registers_t *regs)
{
    uint32_t fault_address = read_cr2();
    uint32_t error_code = regs->err_code;

    int protection_violation = error_code & 0x1;
    int write_operation      = error_code & 0x2;
    int user_mode            = error_code & 0x4;
    int reserved_bit         = error_code & 0x8;
    int instruction_fetch    = error_code & 0x10;

    kprintf("\n========== PAGE FAULT ==========\n");

    kprintf("Faulting virtual address: 0x%x\n",
            fault_address);

    kprintf("Error code: 0x%x\n",
            error_code);

    if (protection_violation)
    {
        kprintf("Reason: Page protection violation\n");
    }
    else
    {
        kprintf("Reason: Page not present\n");
    }

    if (write_operation)
    {
        kprintf("Operation: Write\n");
    }
    else
    {
        kprintf("Operation: Read\n");
    }

    if (user_mode)
    {
        kprintf("Privilege: User mode\n");
    }
    else
    {
        kprintf("Privilege: Kernel mode\n");
    }

    if (reserved_bit)
    {
        kprintf("Reserved paging bit was overwritten\n");
    }

    if (instruction_fetch)
    {
        kprintf("Fault occurred during instruction fetch\n");
    }

    kprintf("================================\n");

    while (1)
    {
        __asm__ volatile("cli; hlt");
    }
}