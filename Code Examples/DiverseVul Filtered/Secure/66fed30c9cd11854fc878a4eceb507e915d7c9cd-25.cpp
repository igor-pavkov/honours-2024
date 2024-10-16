static int coroutine_fn bdrv_mirror_top_flush(BlockDriverState *bs)
{
    if (bs->backing == NULL) {
        /* we can be here after failed bdrv_append in mirror_start_job */
        return 0;
    }
    return bdrv_co_flush(bs->backing->bs);
}