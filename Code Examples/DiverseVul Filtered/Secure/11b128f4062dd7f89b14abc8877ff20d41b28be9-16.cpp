static int validate_table_offset(BlockDriverState *bs, uint64_t offset,
                                 uint64_t entries, size_t entry_len)
{
    BDRVQcowState *s = bs->opaque;
    uint64_t size;

    /* Use signed INT64_MAX as the maximum even for uint64_t header fields,
     * because values will be passed to qemu functions taking int64_t. */
    if (entries > INT64_MAX / entry_len) {
        return -EINVAL;
    }

    size = entries * entry_len;

    if (INT64_MAX - size < offset) {
        return -EINVAL;
    }

    /* Tables must be cluster aligned */
    if (offset & (s->cluster_size - 1)) {
        return -EINVAL;
    }

    return 0;
}