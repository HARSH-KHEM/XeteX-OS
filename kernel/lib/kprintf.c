#include "string.h"
#include "kprintf.h"
#include "../drivers/vga.h"
#include <stdarg.h>
#include <stdint.h>

static void print_str(const char *s)
{
    if (!s)
        s = "(null)";
    while (*s)
        vga_putchar(*s++);
}

static void print_uint(uint64_t n, int base, int pad, char pad_char)
{
    const char *digits = "0123456789abcdef";
    char buf[64];
    int i = 0;

    if (n == 0)
    {
        buf[i++] = '0';
    }
    else
    {
        while (n > 0)
        {
            buf[i++] = digits[n % base];
            n /= base;
        }
    }

    // padding
    while (i < pad)
        buf[i++] = pad_char;

    // print reversed
    while (i--)
        vga_putchar(buf[i]);
}

static void print_int(int64_t n)
{
    if (n < 0)
    {
        vga_putchar('-');
        n = -n;
    }
    print_uint((uint64_t)n, 10, 0, ' ');
}

void kprintf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    while (*fmt)
    {
        if (*fmt != '%')
        {
            vga_putchar(*fmt++);
            continue;
        }
        fmt++; // skip '%'

        // optional zero-padding: e.g. %08x
        char pad_char = ' ';
        int pad = 0;
        if (*fmt == '0')
        {
            pad_char = '0';
            fmt++;
        }
        while (*fmt >= '0' && *fmt <= '9')
        {
            pad = pad * 10 + (*fmt - '0');
            fmt++;
        }

        switch (*fmt)
        {
        case 'd':
            print_int(va_arg(args, int));
            break;
        case 'u':
            print_uint(va_arg(args, unsigned), 10, pad, pad_char);
            break;
        case 'x':
            print_uint(va_arg(args, unsigned), 16, pad, pad_char);
            break;
        case 'p':
            print_str("0x");
            print_uint((uint64_t)(uintptr_t)va_arg(args, void *), 16, 16, '0');
            break;
        case 's':
            print_str(va_arg(args, char *));
            break;
        case 'c':
            vga_putchar((char)va_arg(args, int));
            break;
        case '%':
            vga_putchar('%');
            break;
        default:
            vga_putchar('?');
            break;
        }
        fmt++;
    }

    va_end(args);
}