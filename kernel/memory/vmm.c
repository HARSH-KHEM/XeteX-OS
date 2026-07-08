#include "vmm.h"
#include "pmm.h"

#include "../lib/string.h"
#include "../lib/kprintf.h"

#define PAGE_ENTRIES 1024
#define PAGE_MASK    0xFFFFF000

extern void load_page_directory(uint32_t page_directory);
extern void enable_paging(void);
extern void flush_tlb(uint32_t virtual_addr);

static uint32_t page_directory[PAGE_ENTRIES] __attribute__((aligned(4096)));
static uint32_t first_page_table[PAGE_ENTRIES] __attribute__((aligned(4096)));

static uint32_t get_pd_index(uint32_t virtual_addr)
{
    return virtual_addr >> 22;
}

static uint32_t get_pt_index(uint32_t virtual_addr)
{
    return (virtual_addr >> 12) & 0x3FF;
}

void vmm_map_page(uint32_t virtual_addr,
                  uint32_t physical_addr,
                  uint32_t flags)
{
    uint32_t pd_index = get_pd_index(virtual_addr);
    uint32_t pt_index = get_pt_index(virtual_addr);

    uint32_t *page_table;

    if (!(page_directory[pd_index] & PAGE_PRESENT))
    {
        uint32_t new_table_phys = pmm_alloc_page();

        if (new_table_phys == 0)
        {
            kprintf("VMM ERROR: Failed to allocate page table\n");
            return;
        }

        page_table = (uint32_t *)new_table_phys;
        memset(page_table, 0, PAGE_SIZE);

        page_directory[pd_index] =
            (new_table_phys & PAGE_MASK) | PAGE_PRESENT | PAGE_WRITABLE;
    }
    else
    {
        page_table =
            (uint32_t *)(page_directory[pd_index] & PAGE_MASK);
    }

    page_table[pt_index] =
        (physical_addr & PAGE_MASK) | flags | PAGE_PRESENT;

    flush_tlb(virtual_addr);
}

void vmm_unmap_page(uint32_t virtual_addr)
{
    uint32_t pd_index = get_pd_index(virtual_addr);
    uint32_t pt_index = get_pt_index(virtual_addr);

    if (!(page_directory[pd_index] & PAGE_PRESENT))
    {
        return;
    }

    uint32_t *page_table =
        (uint32_t *)(page_directory[pd_index] & PAGE_MASK);

    page_table[pt_index] = 0;

    flush_tlb(virtual_addr);
}

uint32_t vmm_get_physical(uint32_t virtual_addr)
{
    uint32_t pd_index = get_pd_index(virtual_addr);
    uint32_t pt_index = get_pt_index(virtual_addr);
    uint32_t offset   = virtual_addr & 0xFFF;

    if (!(page_directory[pd_index] & PAGE_PRESENT))
    {
        return 0;
    }

    uint32_t *page_table =
        (uint32_t *)(page_directory[pd_index] & PAGE_MASK);

    if (!(page_table[pt_index] & PAGE_PRESENT))
    {
        return 0;
    }

    return (page_table[pt_index] & PAGE_MASK) + offset;
}

void vmm_init(void)
{
    memset(page_directory, 0, PAGE_SIZE);
    memset(first_page_table, 0, PAGE_SIZE);

    for (uint32_t i = 0; i < PAGE_ENTRIES; i++)
    {
        first_page_table[i] =
            (i * PAGE_SIZE) | PAGE_PRESENT | PAGE_WRITABLE;
    }

    page_directory[0] =
        ((uint32_t)first_page_table & PAGE_MASK)
        | PAGE_PRESENT
        | PAGE_WRITABLE;

    load_page_directory((uint32_t)page_directory);
    enable_paging();

    kprintf("VMM initialized: identity paging enabled\n");
}