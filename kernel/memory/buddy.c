#include "buddy.h"

typedef struct buddy_block
{
    struct buddy_block *next;
} buddy_block_t;

static buddy_block_t *free_list[MAX_ORDER + 1];

static void list_push(int order,
                      buddy_block_t *block)
{
    block->next = free_list[order];
    free_list[order] = block;
}

static buddy_block_t *list_pop(int order)
{
    buddy_block_t *block = free_list[order];

    if (block)
    {
        free_list[order] = block->next;
    }

    return block;
}

void buddy_init(void)
{
    for (int i = 0; i <= MAX_ORDER; i++)
    {
        free_list[i] = 0;
    }

    buddy_block_t *block =
        (buddy_block_t *)0x100000;

    block->next = 0;

    free_list[MAX_ORDER] = block;
}

uint32_t buddy_alloc(int order)
{
    int current = order;

    while (current <= MAX_ORDER &&
           free_list[current] == 0)
    {
        current++;
    }

    if (current > MAX_ORDER)
    {
        return 0;
    }

    buddy_block_t *block =
        list_pop(current);

    while (current > order)
    {
        current--;

        uint32_t size =
            PAGE_SIZE << current;

        buddy_block_t *buddy =
            (buddy_block_t *)((uint8_t *)block + size);

        list_push(current, buddy);
    }

    return (uint32_t)block;
}

void buddy_free(uint32_t addr,
                int order)
{
    buddy_block_t *block =
        (buddy_block_t *)addr;

    list_push(order, block);
}