static bool is_byte_request_lun_aligned(int64_t offset, int count,
                                        IscsiLun *iscsilun)
{
    if (offset % iscsilun->block_size || count % iscsilun->block_size) {
        error_report("iSCSI misaligned request: "
                     "iscsilun->block_size %u, offset %" PRIi64
                     ", count %d",
                     iscsilun->block_size, offset, count);
        return false;
    }
    return true;
}