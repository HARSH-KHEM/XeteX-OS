#include "arch/gdt.h"
#include "arch/idt.h"
#include "arch/pic.h"

#include "memory/pmm.h"
#include "memory/vmm.h"

#include "drivers/vga.h"

#include "lib/kprintf.h"
#include "include/log.h"

extern uint32_t kernel_start;
extern uint32_t kernel_end;

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

    pmm_reserve_range((uint32_t)&kernel_start,
                  (uint32_t)&kernel_end);

    LOG_INFO("Kernel reserved from 0x%x to 0x%x",
            (uint32_t)&kernel_start,
            (uint32_t)&kernel_end);

    LOG_OK("PMM initialized — %d pages free",
           pmm_get_free_pages());

    uint32_t page1 = pmm_alloc_page();

    LOG_INFO("Allocated page at: 0x%x",
             page1);

    pmm_free_page(page1);

    LOG_INFO("Page freed successfully");

    /* -------------- VMM -------------- */

    LOG_INFO("Initializing VMM...");

    vmm_init();

    LOG_OK("VMM initialized — paging enabled");


    LOG_OK("Phase 3 started. Paging enabled.");

    while (1)
    {
        __asm__ volatile("hlt");
    }
}