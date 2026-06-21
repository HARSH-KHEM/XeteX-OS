#include "arch/gdt.h"
#include "arch/idt.h"
#include "arch/pic.h"

#include "memory/pmm.h"
#include "memory/pmmBuddy.h"

#include "drivers/vga.h"

#include "lib/kprintf.h"
#include "include/log.h"

void kernel_main(void)
{
    vga_init();

    LOG_INFO("XETEX-OS booting...");
    LOG_INFO("Developed by Kartik & Harsh");

    /* ---------------- GDT ---------------- */

    gdt_init();
    LOG_OK("GDT initialized");

    /* ---------------- IDT ---------------- */

    idt_init();
    LOG_OK("IDT initialized");

    /* ---------------- PIC ---------------- */

    pic_init();
    LOG_OK("PIC initialized — IRQs remapped");

    /* ================================================= */
    /* BITMAP PMM TEST                                   */
    /* ================================================= */

    LOG_INFO("Testing Bitmap PMM...");

    pmm_init(128 * 1024 * 1024);

    LOG_OK("Bitmap PMM initialized");
    LOG_INFO("Free pages: %d", pmm_get_free_pages());

    uint32_t page1 = pmm_alloc_page();
    uint32_t page2 = pmm_alloc_page();

    LOG_INFO("Allocated page1: 0x%x", page1);
    LOG_INFO("Allocated page2: 0x%x", page2);

    LOG_INFO("Free pages after alloc: %d",
             pmm_get_free_pages());

    pmm_free_page(page1);

    LOG_INFO("Freed page1");

    LOG_INFO("Free pages after free: %d",
             pmm_get_free_pages());

    /* ================================================= */
    /* BUDDY PMM TEST                                    */
    /* ================================================= */

    LOG_INFO("Testing Buddy PMM...");

    buddy_init();

    LOG_OK("Buddy allocator initialized");

    uint32_t block1 = buddy_alloc(0);
    uint32_t block2 = buddy_alloc(0);

    LOG_INFO("Allocated block1: 0x%x", block1);
    LOG_INFO("Allocated block2: 0x%x", block2);

    buddy_free(block1, 0);

    LOG_INFO("Freed block1");

    LOG_INFO("Buddy allocator test complete");

    LOG_OK("Phase 2 PMM complete.");

    while (1)
    {
        __asm__ volatile("hlt");
    }
}