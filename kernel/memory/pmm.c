#include "pmm.h"
#include "../lib/string.h"

/*
 * Bitmap-based Physical Memory Manager
 *
 * We support up to 4 GB of physical address space.
 *   4 GB / 4 KB = 1,048,576 pages  →  1,048,576 / 8 = 131,072 bytes bitmap
 *
 * Bit encoding:
 *   0 = free
 *   1 = used (allocated or reserved)
 */

#define MAX_PAGES    (1024 * 1024)     /* 4 GB / 4 KB */
#define BITMAP_SIZE  (MAX_PAGES / 8)   /* 128 KB bitmap */

/* first 1 MB is always reserved (BIOS, VGA, kernel, etc.) */
#define RESERVED_PAGES  (0x100000 / PMM_PAGE_SIZE)  /* 256 pages */

static uint8_t  bitmap[BITMAP_SIZE];
static uint32_t total_pages;
static uint32_t used_pages;

/* ── bitmap helpers ───────────────────────────────────────────── */

static inline void bitmap_set(uint32_t page) {
    bitmap[page / 8] |= (1 << (page % 8));
}

static inline void bitmap_clear(uint32_t page) {
    bitmap[page / 8] &= ~(1 << (page % 8));
}

static inline int bitmap_test(uint32_t page) {
    return (bitmap[page / 8] >> (page % 8)) & 1;
}

/* ── public API ───────────────────────────────────────────────── */

/*
 * pmm_init — initialise the bitmap for `mem_size` bytes of RAM.
 *
 * Strategy:
 *   1. Mark ALL pages as used  (memset bitmap to 0xFF)
 *   2. Free the pages that actually correspond to usable RAM
 *   3. Re-reserve the first 1 MB (BIOS/VGA/kernel live there)
 */
void pmm_init(uint32_t mem_size) {
    total_pages = mem_size / PMM_PAGE_SIZE;
    if (total_pages > MAX_PAGES) {
        total_pages = MAX_PAGES;
    }

    /* step 1: everything is used */
    memset(bitmap, 0xFF, BITMAP_SIZE);
    used_pages = total_pages;

    /* step 2: mark usable RAM pages as free */
    for (uint32_t i = 0; i < total_pages; i++) {
        bitmap_clear(i);
        used_pages--;
    }

    /* step 3: reserve the first 1 MB */
    for (uint32_t i = 0; i < RESERVED_PAGES; i++) {
        bitmap_set(i);
        used_pages++;
    }
}

/*
 * pmm_alloc_page — find the first free page, mark it used,
 *                  return its physical address.
 *
 * Returns 0 on failure (out of memory).
 */
uint32_t pmm_alloc_page(void) {
    for (uint32_t i = 0; i < total_pages; i++) {
        if (!bitmap_test(i)) {
            bitmap_set(i);
            used_pages++;
            return i * PMM_PAGE_SIZE;
        }
    }
    return 0;  /* OOM */
}

/*
 * pmm_free_page — mark the page at physical address `addr` as free.
 *
 * Silently ignores addresses in the reserved first 1 MB.
 */
void pmm_free_page(uint32_t addr) {
    uint32_t page = addr / PMM_PAGE_SIZE;

    /* don't free reserved memory */
    if (page < RESERVED_PAGES) {
        return;
    }
    if (page >= total_pages) {
        return;
    }

    if (bitmap_test(page)) {
        bitmap_clear(page);
        used_pages--;
    }
}

/*
 * pmm_get_free_pages — return the number of free page frames.
 */
uint32_t pmm_get_free_pages(void) {
    return total_pages - used_pages;
}
