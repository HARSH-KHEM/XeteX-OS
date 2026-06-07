#ifndef LOG_H
#define LOG_H

#include "../lib/kprintf.h"
#include "../drivers/vga.h"

// Color-coded log macros — partner uses these everywhere

#define LOG_OK(fmt, ...)                             \
    do                                               \
    {                                                \
        vga_set_color(VGA_LIGHT_GREEN, VGA_BLACK);   \
        kprintf("[ OK ]  " fmt "\n", ##__VA_ARGS__); \
        vga_set_color(VGA_WHITE, VGA_BLACK);         \
    } while (0)

#define LOG_INFO(fmt, ...)                           \
    do                                               \
    {                                                \
        vga_set_color(VGA_LIGHT_CYAN, VGA_BLACK);    \
        kprintf("[INFO]  " fmt "\n", ##__VA_ARGS__); \
        vga_set_color(VGA_WHITE, VGA_BLACK);         \
    } while (0)

#define LOG_WARN(fmt, ...)                           \
    do                                               \
    {                                                \
        vga_set_color(VGA_LIGHT_BROWN, VGA_BLACK);   \
        kprintf("[WARN]  " fmt "\n", ##__VA_ARGS__); \
        vga_set_color(VGA_WHITE, VGA_BLACK);         \
    } while (0)

#define LOG_PANIC(fmt, ...)                          \
    do                                               \
    {                                                \
        vga_set_color(VGA_WHITE, VGA_RED);           \
        kprintf("[PANIC] " fmt "\n", ##__VA_ARGS__); \
        for (;;)                                     \
            ;                                        \
    } while (0)

#endif