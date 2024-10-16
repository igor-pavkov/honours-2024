static void migration_bitmap_sync_range(ram_addr_t start, ram_addr_t length)
{
    ram_addr_t addr;
    unsigned long page = BIT_WORD(start >> TARGET_PAGE_BITS);

    /* start address is aligned at the start of a word? */
    if (((page * BITS_PER_LONG) << TARGET_PAGE_BITS) == start) {
        int k;
        int nr = BITS_TO_LONGS(length >> TARGET_PAGE_BITS);
        unsigned long *src = ram_list.dirty_memory[DIRTY_MEMORY_MIGRATION];

        for (k = page; k < page + nr; k++) {
            if (src[k]) {
                unsigned long new_dirty;
                new_dirty = ~migration_bitmap[k];
                migration_bitmap[k] |= src[k];
                new_dirty &= src[k];
                migration_dirty_pages += ctpopl(new_dirty);
                src[k] = 0;
            }
        }
    } else {
        for (addr = 0; addr < length; addr += TARGET_PAGE_SIZE) {
            if (cpu_physical_memory_get_dirty(start + addr,
                                              TARGET_PAGE_SIZE,
                                              DIRTY_MEMORY_MIGRATION)) {
                cpu_physical_memory_reset_dirty(start + addr,
                                                TARGET_PAGE_SIZE,
                                                DIRTY_MEMORY_MIGRATION);
                migration_bitmap_set_dirty(start + addr);
            }
        }
    }
}