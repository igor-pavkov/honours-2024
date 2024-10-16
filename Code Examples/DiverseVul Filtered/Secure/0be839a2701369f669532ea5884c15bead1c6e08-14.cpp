uint64_t norm_mig_bytes_transferred(void)
{
    return acct_info.norm_pages * TARGET_PAGE_SIZE;
}