static coroutine_fn int parallels_co_read(BlockDriverState *bs, int64_t sector_num,
                                          uint8_t *buf, int nb_sectors)
{
    int ret;
    BDRVParallelsState *s = bs->opaque;
    qemu_co_mutex_lock(&s->lock);
    ret = parallels_read(bs, sector_num, buf, nb_sectors);
    qemu_co_mutex_unlock(&s->lock);
    return ret;
}