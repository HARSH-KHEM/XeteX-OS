#pragma once

#include <stdint.h>

#define PMM_PAGE_SIZE 4096

void bitmap_init(uint32_t mem_size);

uint32_t bitmap_alloc_page(void);

void bitmap_free_page(uint32_t addr);

uint32_t bitmap_get_free_pages(void);

void bitmap_reserve_page(uint32_t addr);