static coroutine_fn int qcow2_co_write_zeroes(BlockDriverState *bs,
    int64_t sector_num, int nb_sectors, BdrvRequestFlags flags)
{
    int ret;
    BDRVQcowState *s = bs->opaque;

    /* Emulate misaligned zero writes */
    if (sector_num % s->cluster_sectors || nb_sectors % s->cluster_sectors) {
        return -ENOTSUP;
    }

    /* Whatever is left can use real zero clusters */
    qemu_co_mutex_lock(&s->lock);
    ret = qcow2_zero_clusters(bs, sector_num << BDRV_SECTOR_BITS,
        nb_sectors);
    qemu_co_mutex_unlock(&s->lock);

    return ret;
}