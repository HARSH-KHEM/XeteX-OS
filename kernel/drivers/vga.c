#include "vga.h"

#define VGA_BASE ((volatile uint16_t *)0xB8000)
#define VGA_COLS 80
#define VGA_ROWS 25

static int cursor_row = 0;
static int cursor_col = 0;
static uint8_t current_color;

static uint16_t vga_entry(char c, uint8_t color)
{
    return (uint16_t)c | ((uint16_t)color << 8);
}

static uint8_t make_color(vga_color_t fg, vga_color_t bg)
{
    return fg | (bg << 4);
}

void vga_set_color(vga_color_t fg, vga_color_t bg)
{
    current_color = make_color(fg, bg);
}

void vga_init(void)
{
    current_color = make_color(VGA_WHITE, VGA_BLACK);
    vga_clear();
}

void vga_clear(void)
{
    for (int i = 0; i < VGA_COLS * VGA_ROWS; i++)
        VGA_BASE[i] = vga_entry(' ', current_color);
    cursor_row = cursor_col = 0;
}

static void vga_scroll(void)
{
    // shift every row up by one
    for (int r = 1; r < VGA_ROWS; r++)
        for (int c = 0; c < VGA_COLS; c++)
            VGA_BASE[(r - 1) * VGA_COLS + c] = VGA_BASE[r * VGA_COLS + c];
    // blank last row
    for (int c = 0; c < VGA_COLS; c++)
        VGA_BASE[(VGA_ROWS - 1) * VGA_COLS + c] = vga_entry(' ', current_color);
    cursor_row = VGA_ROWS - 1;
}

void vga_putchar(char c)
{
    if (c == '\n')
    {
        cursor_col = 0;
        cursor_row++;
    }
    else if (c == '\r')
    {
        cursor_col = 0;
    }
    else if (c == '\t')
    {
        cursor_col = (cursor_col + 8) & ~7; // align to 8
    }
    else
    {
        VGA_BASE[cursor_row * VGA_COLS + cursor_col] = vga_entry(c, current_color);
        cursor_col++;
        if (cursor_col >= VGA_COLS)
        {
            cursor_col = 0;
            cursor_row++;
        }
    }
    if (cursor_row >= VGA_ROWS)
        vga_scroll();
}

void vga_puts(const char *str)
{
    while (*str)
        vga_putchar(*str++);
}