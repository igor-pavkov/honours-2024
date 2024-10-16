static int read_uint32(BlockDriverState *bs, int64_t offset, uint32_t *result)
{
    uint32_t buffer;
    int ret;

    ret = bdrv_pread(bs->file, offset, &buffer, 4);
    if (ret < 0) {
        return ret;
    }

    *result = be32_to_cpu(buffer);
    return 0;
}