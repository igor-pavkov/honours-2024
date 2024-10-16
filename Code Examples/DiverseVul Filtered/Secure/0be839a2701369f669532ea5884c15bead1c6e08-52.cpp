static ram_addr_t ram_save_remaining(void)
{
    return migration_dirty_pages;
}