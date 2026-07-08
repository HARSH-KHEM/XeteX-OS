#include "pmm.h"

#include "bitmap.h"
#include "buddy.h"

#include "../include/config.h"

void pmm_init(uint32_t mem_size)
{
#if PMM_TYPE == PMM_BITMAP

    bitmap_init(mem_size);

#elif PMM_TYPE == PMM_BUDDY

    (void)mem_size;
    buddy_init();

#endif
}

uint32_t pmm_alloc_page(void)
{
#if PMM_TYPE == PMM_BITMAP

    return bitmap_alloc_page();

#elif PMM_TYPE == PMM_BUDDY

    return buddy_alloc(0);

#endif
}

void pmm_free_page(uint32_t addr)
{
#if PMM_TYPE == PMM_BITMAP

    bitmap_free_page(addr);

#elif PMM_TYPE == PMM_BUDDY

    buddy_free(addr, 0);

#endif
}

uint32_t pmm_get_free_pages(void)
{
#if PMM_TYPE == PMM_BITMAP

    return bitmap_get_free_pages();

#elif PMM_TYPE == PMM_BUDDY

    return 0;

#endif
}

void pmm_reserve_range(uint32_t start_addr, uint32_t end_addr)
{
#if PMM_TYPE == PMM_BITMAP

    uint32_t start_page_addr = start_addr & ~(PAGE_SIZE - 1);
    uint32_t end_page_addr   = (end_addr + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);

    for (uint32_t addr = start_page_addr; addr < end_page_addr; addr += PAGE_SIZE)
    {
        bitmap_reserve_page(addr);
    }

#elif PMM_TYPE == PMM_BUDDY

    (void)start_addr;
    (void)end_addr;

#endif
}