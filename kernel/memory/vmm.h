#pragma once

#include <stdint.h>

#define PAGE_SIZE 4096

#define PAGE_PRESENT  0x1
#define PAGE_WRITABLE 0x2
#define PAGE_USER     0x4

void vmm_init(void);

void vmm_map_page(uint32_t virtual_addr,
                  uint32_t physical_addr,
                  uint32_t flags);