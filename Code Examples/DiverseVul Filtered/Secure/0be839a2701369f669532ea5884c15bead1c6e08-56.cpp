uint64_t skipped_mig_bytes_transferred(void)
{
    return acct_info.skipped_pages * TARGET_PAGE_SIZE;
}