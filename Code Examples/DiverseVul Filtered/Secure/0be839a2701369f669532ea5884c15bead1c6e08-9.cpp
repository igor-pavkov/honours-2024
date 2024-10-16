static uint64_t ram_save_pending(QEMUFile *f, void *opaque, uint64_t max_size)
{
    uint64_t remaining_size;

    remaining_size = ram_save_remaining() * TARGET_PAGE_SIZE;

    if (remaining_size < max_size) {
        qemu_mutex_lock_iothread();
        migration_bitmap_sync();
        qemu_mutex_unlock_iothread();
        remaining_size = ram_save_remaining() * TARGET_PAGE_SIZE;
    }
    return remaining_size;
}