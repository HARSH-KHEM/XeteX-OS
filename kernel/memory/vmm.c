#include "vmm.h"
#include "pmm.h"

#include "../lib/string.h"
#include "../lib/kprintf.h"

extern void load_page_directory(uint32_t page_directory);
extern void enable_paging(void);

static uint32_t page_directory[1024] __attribute__((aligned(4096)));
static uint32_t first_page_table[1024] __attribute__((aligned(4096)));

void vmm_map_page(uint32_t virtual_addr,
                  uint32_t physical_addr,
                  uint32_t flags)
{
    uint32_t pd_index = virtual_addr >> 22;
    uint32_t pt_index = (virtual_addr >> 12) & 0x3FF;

    uint32_t *page_table;

    if (!(page_directory[pd_index] & PAGE_PRESENT))
    {
        uint32_t new_table_phys = pmm_alloc_page();

        page_table = (uint32_t *)new_table_phys;
        memset(page_table, 0, PAGE_SIZE);

        page_directory[pd_index] =
            new_table_phys | flags | PAGE_PRESENT;
    }
    else
    {
        page_table =
            (uint32_t *)(page_directory[pd_index] & 0xFFFFF000);
    }

    page_table[pt_index] =
        (physical_addr & 0xFFFFF000) | flags | PAGE_PRESENT;
}

void vmm_init(void)
{
    memset(page_directory, 0, PAGE_SIZE);
    memset(first_page_table, 0, PAGE_SIZE);

    for (uint32_t i = 0; i < 1024; i++)
    {
        first_page_table[i] =
            (i * PAGE_SIZE) | PAGE_PRESENT | PAGE_WRITABLE;
    }

    page_directory[0] =
        ((uint32_t)first_page_table) | PAGE_PRESENT | PAGE_WRITABLE;

    load_page_directory((uint32_t)page_directory);
    enable_paging();

    kprintf("VMM initialized: identity paging enabled\n");
}