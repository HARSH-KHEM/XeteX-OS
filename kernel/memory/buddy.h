#pragma once

#include <stdint.h>

#define PAGE_SIZE 4096

#define MAX_ORDER 10

void buddy_init(void);

uint32_t buddy_alloc(int order);

void buddy_free(uint32_t addr, int order);