static inline bool iscsi_allocmap_is_valid(IscsiLun *iscsilun,
                                           int64_t offset, int64_t bytes)
{
    unsigned long size;
    if (iscsilun->allocmap_valid == NULL) {
        return false;
    }
    assert(iscsilun->cluster_size);
    size = DIV_ROUND_UP(offset + bytes, iscsilun->cluster_size);
    return (find_next_zero_bit(iscsilun->allocmap_valid, size,
                               offset / iscsilun->cluster_size) == size);
}