#pragma once

#include <stdint.h>

void pmm_init(uint32_t mem_size);

uint32_t pmm_alloc_page(void);

void pmm_free_page(uint32_t addr);

uint32_t pmm_get_free_pages(void);