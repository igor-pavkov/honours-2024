static inline bool migration_bitmap_set_dirty(ram_addr_t addr)
{
    bool ret;
    int nr = addr >> TARGET_PAGE_BITS;

    ret = test_and_set_bit(nr, migration_bitmap);

    if (!ret) {
        migration_dirty_pages++;
    }
    return ret;
}