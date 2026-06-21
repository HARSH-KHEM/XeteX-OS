#pragma once
#include <stdint.h>

#define PAGE_SIZE 4096

/* Maximum block size:
 *
 * Order 0  = 4 KB
 * Order 1  = 8 KB
 * Order 2  = 16 KB
 * ...
 * Order 10 = 4 MB
 */
#define MAX_ORDER 10

void buddy_init(void);

uint32_t buddy_alloc(int order);

void buddy_free(uint32_t addr, int order);