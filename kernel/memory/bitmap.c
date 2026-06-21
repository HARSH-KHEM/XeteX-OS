#include "bitmap.h"
#include "../lib/string.h"

#define MAX_PAGES (1024 * 1024)
#define BITMAP_SIZE (MAX_PAGES / 8)

#define RESERVED_PAGES (0x100000 / PMM_PAGE_SIZE)

static uint8_t bitmap[BITMAP_SIZE];

static uint32_t total_pages;
static uint32_t used_pages;

static inline void bitmap_set(uint32_t page)
{
    bitmap[page / 8] |= (1 << (page % 8));
}

static inline void bitmap_clear(uint32_t page)
{
    bitmap[page / 8] &= ~(1 << (page % 8));
}

static inline int bitmap_test(uint32_t page)
{
    return (bitmap[page / 8] >> (page % 8)) & 1;
}

void bitmap_init(uint32_t mem_size)
{
    total_pages = mem_size / PMM_PAGE_SIZE;

    if (total_pages > MAX_PAGES)
    {
        total_pages = MAX_PAGES;
    }

    memset(bitmap, 0xFF, BITMAP_SIZE);

    used_pages = total_pages;

    for (uint32_t i = 0; i < total_pages; i++)
    {
        bitmap_clear(i);
        used_pages--;
    }

    for (uint32_t i = 0; i < RESERVED_PAGES; i++)
    {
        bitmap_set(i);
        used_pages++;
    }
}

uint32_t bitmap_alloc_page(void)
{
    for (uint32_t i = 0; i < total_pages; i++)
    {
        if (!bitmap_test(i))
        {
            bitmap_set(i);
            used_pages++;

            return i * PMM_PAGE_SIZE;
        }
    }

    return 0;
}

void bitmap_free_page(uint32_t addr)
{
    uint32_t page = addr / PMM_PAGE_SIZE;

    if (page < RESERVED_PAGES)
    {
        return;
    }

    if (page >= total_pages)
    {
        return;
    }

    if (bitmap_test(page))
    {
        bitmap_clear(page);
        used_pages--;
    }
}

uint32_t bitmap_get_free_pages(void)
{
    return total_pages - used_pages;
}