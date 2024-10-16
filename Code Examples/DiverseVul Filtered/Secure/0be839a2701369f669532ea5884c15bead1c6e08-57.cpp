uint64_t dup_mig_bytes_transferred(void)
{
    return acct_info.dup_pages * TARGET_PAGE_SIZE;
}