static inline bool rom_order_compare(Rom *rom, Rom *item)
{
    return ((uintptr_t)(void *)rom->as > (uintptr_t)(void *)item->as) ||
           (rom->as == item->as && rom->addr >= item->addr);
}