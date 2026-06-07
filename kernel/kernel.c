#include "arch/gdt.h"
#include "arch/idt.h"
#include "arch/pic.h"
#include "memory/pmm.h"
#include "drivers/vga.h"
#include "lib/kprintf.h"
#include "include/log.h"

void kernel_main(void) {
    vga_init();
    LOG_INFO("XETEX-OS booting...");
    LOG_INFO("Developed by Kartik & Harsh");

    gdt_init();
    LOG_OK("GDT initialized");

    idt_init();
    LOG_OK("IDT initialized");

    pic_init();
    LOG_OK("PIC initialized — IRQs remapped");

    pmm_init(128 * 1024 * 1024);  /* 128 MB RAM */
    LOG_OK("PMM initialized — %d pages free", pmm_get_free_pages());

    /* quick PMM sanity test */
    uint32_t page1 = pmm_alloc_page();
    LOG_INFO("Allocated page at: 0x%x", page1);
    pmm_free_page(page1);
    LOG_INFO("Page freed successfully");

    LOG_INFO("Phase 1 complete. All systems go.");
    while (1) {}
}