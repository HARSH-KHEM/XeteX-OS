#pragma once
#include <stdint.h>

/*
 * Physical Memory Manager — bitmap allocator
 *
 * Each bit in the bitmap represents one 4 KB page frame.
 *   bit = 0  →  page is free
 *   bit = 1  →  page is used
 */

#define PMM_PAGE_SIZE  4096  /* 4 KB */

void     pmm_init(uint32_t mem_size);
uint32_t pmm_alloc_page(void);
void     pmm_free_page(uint32_t addr);
uint32_t pmm_get_free_pages(void);
