static coroutine_fn int qcow2_co_discard(BlockDriverState *bs,
    int64_t sector_num, int nb_sectors)
{
    int ret;
    BDRVQcowState *s = bs->opaque;

    qemu_co_mutex_lock(&s->lock);
    ret = qcow2_discard_clusters(bs, sector_num << BDRV_SECTOR_BITS,
        nb_sectors, QCOW2_DISCARD_REQUEST);
    qemu_co_mutex_unlock(&s->lock);
    return ret;
}