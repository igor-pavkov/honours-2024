static int coroutine_fn bdrv_mirror_top_pdiscard(BlockDriverState *bs,
    int64_t offset, int bytes)
{
    return bdrv_mirror_top_do_write(bs, MIRROR_METHOD_DISCARD, offset, bytes,
                                    NULL, 0);
}