BlockDriverAIOCB *bdrv_aio_writev(BlockDriverState *bs, int64_t sector_num,
                                  QEMUIOVector *qiov, int nb_sectors,
                                  BlockDriverCompletionFunc *cb, void *opaque)
{
    trace_bdrv_aio_writev(bs, sector_num, nb_sectors, opaque);

    return bdrv_co_aio_rw_vector(bs, sector_num, qiov, nb_sectors, 0,
                                 cb, opaque, true);
}