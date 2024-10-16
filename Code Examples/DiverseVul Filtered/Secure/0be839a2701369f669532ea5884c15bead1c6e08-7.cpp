static int ram_save_complete(QEMUFile *f, void *opaque)
{
    qemu_mutex_lock_ramlist();
    migration_bitmap_sync();

    ram_control_before_iterate(f, RAM_CONTROL_FINISH);

    /* try transferring iterative blocks of memory */

    /* flush all remaining blocks regardless of rate limiting */
    while (true) {
        int bytes_sent;

        bytes_sent = ram_find_and_save_block(f, true);
        /* no more blocks to sent */
        if (bytes_sent == 0) {
            break;
        }
        bytes_transferred += bytes_sent;
    }

    ram_control_after_iterate(f, RAM_CONTROL_FINISH);
    migration_end();

    qemu_mutex_unlock_ramlist();
    qemu_put_be64(f, RAM_SAVE_FLAG_EOS);

    return 0;
}