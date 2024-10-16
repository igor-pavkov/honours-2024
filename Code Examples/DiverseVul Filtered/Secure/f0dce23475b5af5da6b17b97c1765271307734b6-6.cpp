static int read_uint64(BlockDriverState *bs, int64_t offset, uint64_t *result)
{
    uint64_t buffer;
    int ret;

    ret = bdrv_pread(bs->file, offset, &buffer, 8);
    if (ret < 0) {
        return ret;
    }

    *result = be64_to_cpu(buffer);
    return 0;
}