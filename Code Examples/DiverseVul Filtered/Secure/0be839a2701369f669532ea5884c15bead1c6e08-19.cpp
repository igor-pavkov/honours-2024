void acct_update_position(QEMUFile *f, size_t size, bool zero)
{
    uint64_t pages = size / TARGET_PAGE_SIZE;
    if (zero) {
        acct_info.dup_pages += pages;
    } else {
        acct_info.norm_pages += pages;
        bytes_transferred += size;
        qemu_update_position(f, size);
    }
}